import argparse
import json
import time
from PIL import Image
import torch.nn as nn
import torch.utils.model_zoo as model_zoo
from torchvision import datasets
from torchvision import transforms
from torchvision.models import alexnet

USE_GPU = False
USE_PRETRANSFORMED_IMAGES = True
USE_DROPOUT = False
ORIGINAL_IMAGES_DIRECTORY = 'Data/original'
PRETRANSFORMED_IMAGES_DIRECTORY = 'Data/pretransformed'
LABELS_FILE = 'Data/labels.json'
model_urls = {'alexnet': 'https://download.pytorch.org/models/alexnet-owt-4df8aa71.pth'}


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
			transformedStateDict = {
				"l1.weight": originalStateDict["features.0.weight"],
				"l1.bias": originalStateDict["features.0.bias"],
				"l4.weight": originalStateDict["features.3.weight"],
				"l4.bias": originalStateDict["features.3.bias"],
				"l7.weight": originalStateDict["features.6.weight"],
				"l7.bias": originalStateDict["features.6.bias"],
				"l9.weight": originalStateDict["features.8.weight"],
				"l9.bias": originalStateDict["features.8.bias"],
				"l7.bias": originalStateDict["features.6.bias"],
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
		x = self.l1(x)
		x = self.l2(x)
		x = self.l3(x)
		x = self.l4(x)
		x = self.l5(x)
		x = self.l6(x)
		x = self.l7(x)
		x = self.l8(x)
		x = self.l9(x)
		x = self.l10(x)
		x = self.l11(x)
		x = self.l12(x)
		x = self.l13(x)
		x = x.view(x.size(0), 256 * 6 * 6)
		if USE_DROPOUT:
			x = self.dropout1(x)
		x = self.l14(x)
		x = self.l15(x)
		if USE_DROPOUT:
			x = self.dropout2(x)
		x = self.l16(x)
		x = self.l17(x)
		x = self.l18(x)
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

	totalTime = 0
	for image, label in dataset.imgs:
		x = Image.open(image)
		x = preprocess(x).unsqueeze(0)

		startTime = time.time()
		x = model(x)
		endTime = time.time()
		totalTime = totalTime + endTime - startTime

		top_class = x.argmax()
		print(image.split('/')[-1] + ': ' + labelNames[top_class])
	print('\n---------------- Timings ----------------')
	print('- Average Latency (ms): ', round(totalTime / len(dataset.imgs) * 1000, 2))
	print('- Average Throughput (images/sec): ', round(len(dataset.imgs) / totalTime, 2))
	print('-----------------------------------------')


def main():
	global USE_GPU
	parser = argparse.ArgumentParser(description='Measure timings over CPU and GPU inferences of AlexNet')
	parser.add_argument("--gpu", action='store_true', default=False, help='use GPU for inference (default: CPU)')
	parser.add_argument("--use_dropout", action='store_true', default=False, help='use GPU for inference (default: CPU)')
	args = parser.parse_args()
	if args.gpu:
		USE_GPU = True
	if args.use_dropout:
		USE_DROPOUT = True

	# model = alexnet(pretrained=True)
	model = CustomPyTorchAlexNet(pretrained=True)
	model.eval()
	if USE_GPU:
		model.cuda()
	inference(model)


if __name__ == '__main__':
	main()
