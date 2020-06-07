import argparse
import json
import time
from PIL import Image
import torch.nn as nn
import torch.utils.model_zoo as model_zoo
from torchvision import datasets
from torchvision import transforms
from torchvision.models import alexnet
import torch
import numpy as np

USE_GPU = False
USE_PRETRANSFORMED_IMAGES = True
USE_DROPOUT = False
ORIGINAL_IMAGES_DIRECTORY = 'Data/original'
PRETRANSFORMED_IMAGES_DIRECTORY = 'Data/pretransformed'
LABELS_FILE = 'Data/labels.json'
model_urls = {'alexnet': 'https://download.pytorch.org/models/alexnet-owt-4df8aa71.pth'}


def noTransform(t):
	return t


def toFloat16(t):
	t = np.array(t, dtype='float16')
	return t


TRANS_FUNC = noTransform
TRANS_FUNCS = [
	toFloat16
]


def typeConvert(t):
	t = t.detach().numpy()
	t = TRANS_FUNC(t)
	t = np.array(t, dtype='float32')
	t = torch.Tensor(t)
	# t = torch.HalfTensor(t)
	return t


class CustomPyTorchAlexNet(nn.Module):
	def __init__(self, pretrained=False, num_classes=1000):
		super(CustomPyTorchAlexNet, self).__init__()
		self.l1 = nn.Conv2d(3, 64, kernel_size=11, stride=4, padding=2)
		self.l2 = nn.ReLU(inplace=True)
		self.l3 = nn.MaxPool2d(kernel_size=3, stride=2)
		self.l4 = nn.Conv2d(64, 192, kernel_size=5, padding=2)
		self.l5 = nn.ReLU(inplace=True)
		self.l6 = nn.MaxPool2d(kernel_size=3, stride=2)
		self.l7 = nn.Conv2d(192, 384, kernel_size=3, padding=1)
		self.l8 = nn.ReLU(inplace=True)
		self.l9 = nn.Conv2d(384, 256, kernel_size=3, padding=1)
		self.l10 = nn.ReLU(inplace=True)
		self.l11 = nn.Conv2d(256, 256, kernel_size=3, padding=1)
		self.l12 = nn.ReLU(inplace=True)
		self.l13 = nn.MaxPool2d(kernel_size=3, stride=2)
		self.dropout1 = nn.Dropout(p=0.5)
		self.l14 = nn.Linear(256 * 6 * 6, 4096)
		self.l15 = nn.ReLU(inplace=True)
		self.dropout2 = nn.Dropout(p=0.5)
		self.l16 = nn.Linear(4096, 4096)
		self.l17 = nn.ReLU(inplace=True)
		self.l18 = nn.Linear(4096, num_classes)

		if pretrained:
			originalStateDict = model_zoo.load_url(model_urls['alexnet'])
			loss = 0
			totalParams = 0
			maxLoss = 0
			for key in originalStateDict.keys():
				old = originalStateDict[key]
				new = typeConvert(old)
				oldFlat = old.detach().numpy().flatten()
				totalParams += oldFlat.size
				lossArr = abs(oldFlat - new.detach().numpy().flatten())
				maxLoss = max(maxLoss, max(lossArr))
				loss += sum(lossArr)
				originalStateDict[key] = new
			print('Quantization Loss', loss, 'Loss per param:', loss / totalParams, 'Max Loss:', maxLoss, '\n')

			transformedStateDict = {
				"l1.weight": originalStateDict["features.0.weight"],
				"l1.bias": originalStateDict["features.0.bias"],
				"l4.weight": originalStateDict["features.3.weight"],
				"l4.bias": originalStateDict["features.3.bias"],
				"l7.weight": originalStateDict["features.6.weight"],
				"l7.bias": originalStateDict["features.6.bias"],
				"l9.weight": originalStateDict["features.8.weight"],
				"l9.bias": originalStateDict["features.8.bias"],
				"l11.weight": originalStateDict["features.10.weight"],
				"l11.bias": originalStateDict["features.10.bias"],
				"l14.weight": originalStateDict["classifier.1.weight"],
				"l14.bias": originalStateDict["classifier.1.bias"],
				"l16.weight": originalStateDict["classifier.4.weight"],
				"l16.bias": originalStateDict["classifier.4.bias"],
				"l18.weight": originalStateDict["classifier.6.weight"],
				"l18.bias": originalStateDict["classifier.6.bias"],
			}
			self.load_state_dict(transformedStateDict)

	def forward(self, x):
		x = typeConvert(x)
		x = typeConvert(self.l1(x))
		x = typeConvert(self.l2(x))
		x = typeConvert(self.l3(x))
		x = typeConvert(self.l4(x))
		x = typeConvert(self.l5(x))
		x = typeConvert(self.l6(x))
		x = typeConvert(self.l7(x))
		x = typeConvert(self.l8(x))
		x = typeConvert(self.l9(x))
		x = typeConvert(self.l10(x))
		x = typeConvert(self.l11(x))
		x = typeConvert(self.l12(x))
		x = typeConvert(self.l13(x))
		x = typeConvert(x.view(x.size(0), 256 * 6 * 6))
		if USE_DROPOUT:
			x = typeConvert(self.dropout1(x))
		x = typeConvert(self.l14(x))
		x = typeConvert(self.l15(x))
		if USE_DROPOUT:
			x = typeConvert(self.dropout2(x))
		x = typeConvert(self.l16(x))
		x = typeConvert(self.l17(x))
		x = typeConvert(self.l18(x))
		return x


def inference(model):
	labelNames = list(json.loads(open(LABELS_FILE, 'r').read()).values())
	if USE_PRETRANSFORMED_IMAGES:
		preprocess = transforms.Compose([
			transforms.ToTensor(),
		])
		imagesDirectory = PRETRANSFORMED_IMAGES_DIRECTORY
	else:
		preprocess = transforms.Compose([
			transforms.Resize(256),
			transforms.CenterCrop(224),
			transforms.ToTensor(),
		])
		imagesDirectory = ORIGINAL_IMAGES_DIRECTORY

	dataset = datasets.ImageFolder(imagesDirectory, transform=preprocess)

	imageClass = {}
	totalTime = 0
	for image, label in dataset.imgs:
		x = Image.open(image)
		x = preprocess(x).unsqueeze(0)

		startTime = time.time()
		x = model(x)
		endTime = time.time()
		totalTime = totalTime + endTime - startTime

		top_class = int(x.argmax())
		imageName = image.split('/\\')[-1]
		imageClass[imageName] = top_class
		print(imageName + ': ' + labelNames[top_class])
	print('\n---------------- Timings ----------------')
	print('- Average Latency (ms): ', round(totalTime / len(dataset.imgs) * 1000, 2))
	print('- Average Throughput (images/sec): ', round(len(dataset.imgs) / totalTime, 2))
	print('-----------------------------------------')
	return imageClass


def main():
	global USE_GPU, USE_DROPOUT, TRANS_FUNC, TRANS_FUNCS
	parser = argparse.ArgumentParser(description='Measure timings over CPU and GPU inferences of AlexNet')
	parser.add_argument("--gpu", action='store_true', default=False, help='use GPU for inference (default: CPU)')
	parser.add_argument("--use_dropout", action='store_true', default=False, help='use GPU for inference (default: CPU)')
	args = parser.parse_args()
	if args.gpu:
		USE_GPU = True
	if args.use_dropout:
		USE_DROPOUT = True

	# Get baseline
	TRANS_FUNC = noTransform
	model = CustomPyTorchAlexNet(pretrained=True)
	model.eval()
	if USE_GPU:
		model.cuda()
	baseline = inference(model)

	with open('output/baseline.json', 'w') as f:
		f.write(json.dumps(baseline))
		f.close()

	inferences = []
	for transFunc in TRANS_FUNCS:
		TRANS_FUNC = transFunc
		model = CustomPyTorchAlexNet(pretrained=True)
		model.eval()
		if USE_GPU:
			model.cuda()
		inferences.append(inference(model))

	for index, infer in enumerate(inferences):
		same = [1 if infer[key] != baseline[key] else 0 for key in baseline]
		errorRate = sum(same) / len(baseline.keys())
		print(TRANS_FUNCS[index].__name__, 'Error rate:', errorRate)


if __name__ == '__main__':
	main()
