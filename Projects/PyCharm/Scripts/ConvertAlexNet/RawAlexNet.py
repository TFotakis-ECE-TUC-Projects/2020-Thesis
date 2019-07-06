import json

import numpy as np
import torch
import torch.nn as nn
import torch.utils.model_zoo as model_zoo
from PIL import Image
from torch.nn import functional as F
from torchvision import datasets
from torchvision import transforms
from torchvision.models import alexnet

USE_PRETRANSFORMED_IMAGES = True

if USE_PRETRANSFORMED_IMAGES:
	data_dir = '../../Data/ConvertAlexNet/Cat_Dog_data/pretransformed'
# data_dir = '../../Data/ConvertAlexNet/Cat_Dog_data/pretransformedBinary'
# data_dir = '../../Data/ConvertAlexNet/Cat_Dog_data/solidColors'
else:
	data_dir = '../../Data/ConvertAlexNet/Cat_Dog_data'

LABELS_FILE = '../../Data/ConvertAlexNet/labels.json'
model_urls = {'alexnet': 'https://download.pytorch.org/models/alexnet-owt-4df8aa71.pth'}


class RawAlexNet:
	def __init__(self, state_dict=None):
		if state_dict is None:
			state_dict = model_zoo.load_url(model_urls['alexnet'])
			state_dict = {key: state_dict[key].detach().numpy() for key in state_dict}
		self.state_dict = state_dict

	def Conv2d(self, x, weights, bias, in_channels, out_channels, kernel_size, stride=1, padding=0, check=True):
		Hout = (x.shape[1] + 2 * padding - kernel_size) // stride + 1
		Wout = (x.shape[2] + 2 * padding - kernel_size) // stride + 1

		arr = np.zeros((x.shape[0], x.shape[1] + 2 * padding, x.shape[2] + 2 * padding))
		arr[:, padding:x.shape[1] + padding, padding:x.shape[2] + padding] = x

		res = np.zeros((out_channels, Hout, Wout))

		for out_channel in range(out_channels):
			for oh in range(Hout):
				for ow in range(Wout):
					imgStartH = oh * stride
					imgEndH = imgStartH + kernel_size
					imgStartW = ow * stride
					imgEndW = imgStartW + kernel_size

					pixel = 0
					for in_channel in range(in_channels):
						img = arr[in_channel, imgStartH: imgEndH, imgStartW: imgEndW]
						conv = img * weights[out_channel, in_channel]
						pixel += conv.sum()

					res[out_channel, oh, ow] = pixel + bias[out_channel]

		if check:
			x1 = x.reshape((1, x.shape[0], x.shape[1], x.shape[2]))
			x1 = torch.tensor(x1, dtype=torch.double)
			w1 = torch.tensor(weights, dtype=torch.double)
			b1 = torch.tensor(bias, dtype=torch.double)
			x1 = F.conv2d(x1, w1, b1, stride, padding)
			x1 = x1[0]
			x1 = x1.detach().numpy()
			err = abs(x1 - res)
			err = err.sum()
			print('Conv2d Error: ', err)
		return res

	def ReLU(self, x, inplace=False, check=True):
		res = [el if el > 0 else 0 for el in x.reshape(x.size)]
		res = np.array(res)
		res = res.reshape(x.shape)

		if check:
			x1 = torch.tensor(x)
			x1 = torch.relu_(x1)
			x1 = x1.detach().numpy()
			err = abs(x1 - res)
			err = err.sum()
			print('ReLU Error: ', err)
		return res

	def MaxPool2d(self, x, kernel_size=1, stride=1, check=True):
		Hout = (x.shape[1] - kernel_size) // stride + 1
		Wout = (x.shape[2] - kernel_size) // stride + 1
		res = np.zeros((x.shape[0], Hout, Wout))
		for k in range(x.shape[0]):
			for i in range(Hout):
				for j in range(Wout):
					a = i * stride
					b = j * stride
					res[k, i, j] = np.max(x[k, a: a + kernel_size, b: b + kernel_size])

		if check:
			x1 = torch.tensor(x, dtype=torch.double)
			x1 = F.max_pool2d(x1, kernel_size, stride)
			x1 = x1.detach().numpy()
			err = abs(x1 - res)
			err = err.sum()
			print('MaxPool2d Error: ', err)
		return res

	def AdaptiveAvgPool2d(self, x, sizes=(1, 1), check=True):
		def startInd(a, b, c):
			return int(np.floor((a * c) / b))

		def endInd(a, b, c):
			return int(np.ceil(((a + 1) * c) / b))

		sizeD = x.shape[0]
		isizeH = x.shape[1]
		isizeW = x.shape[2]

		osizeH = sizes[0]
		osizeW = sizes[1]

		res = np.zeros((sizeD, osizeH, osizeW))

		for d in range(sizeD):
			for oh in range(osizeH):
				istartH = startInd(oh, osizeH, isizeH)
				iendH = endInd(oh, osizeH, isizeH)
				kH = iendH - istartH

				for ow in range(osizeW):
					istartW = startInd(ow, osizeW, isizeW)
					iendW = endInd(ow, osizeW, isizeW)
					kW = iendW - istartW

					for ih in range(istartH, iendH):
						for iw in range(istartW, iendW):
							res[d, oh, ow] += x[d, ih, iw]

					res[d, oh, ow] = res[d, oh, ow] / kW / kH

		if check:
			x1 = torch.tensor(x, dtype=torch.double)
			x1 = F.adaptive_avg_pool2d(x1, sizes)
			x1 = x1.detach().numpy()
			err = abs(x1 - res)
			err = err.sum()
			print('AdaptiveAvgPool2d Error: ', err)

		return res

	def Linear(self, x, weights, bias, in_features, out_features, check=True):
		if weights.shape[0] == out_features:
			fitted_weights = weights.transpose()
		else:
			fitted_weights = weights
		res = x.dot(fitted_weights) + bias

		if check:
			x1 = torch.tensor(x, dtype=torch.double)
			w1 = torch.tensor(weights, dtype=torch.double)
			b1 = torch.tensor(bias, dtype=torch.double)
			x1 = F.linear(x1, w1, b1)
			x1 = x1.detach().numpy()
			err = abs(x1 - res)
			err = err.sum()
			print('Linear Error: ', err)

		return res

	def LogSoftmax(self, x, dim=0, check=True):
		# LogSoftmax(xi) = log⎛⎝(exp(xi))/( ∑jexp(xj))⎞⎠
		exp = np.exp(x)
		sum = exp.sum()
		res = [np.log(np.exp(el) / sum) for el in x][0]

		if check:
			x1 = torch.tensor(x, dtype=torch.double)
			x1 = F.log_softmax(x1, dim)
			x1 = x1.detach().numpy()
			err = abs(x1 - res)
			err = err.sum()
			print('LogSoftmax Error: ', err)

		return res

	def forward(self, x):
		x = x.numpy()
		x = self.Conv2d(x, weights=self.state_dict['features.0.weight'], bias=self.state_dict['features.0.bias'], in_channels=3, out_channels=64, kernel_size=11, stride=4, padding=2)
		x = self.ReLU(x, inplace=True)
		x = self.MaxPool2d(x, kernel_size=3, stride=2)
		x = self.Conv2d(x, weights=self.state_dict['features.3.weight'], bias=self.state_dict['features.3.bias'], in_channels=64, out_channels=192, kernel_size=5, padding=2)
		x = self.ReLU(x, inplace=True)
		x = self.MaxPool2d(x, kernel_size=3, stride=2)
		x = self.Conv2d(x, weights=self.state_dict['features.6.weight'], bias=self.state_dict['features.6.bias'], in_channels=192, out_channels=384, kernel_size=3, padding=1)
		x = self.ReLU(x, inplace=True)
		x = self.Conv2d(x, weights=self.state_dict['features.8.weight'], bias=self.state_dict['features.8.bias'], in_channels=384, out_channels=256, kernel_size=3, padding=1)
		x = self.ReLU(x, inplace=True)
		x = self.Conv2d(x, weights=self.state_dict['features.10.weight'], bias=self.state_dict['features.10.bias'], in_channels=256, out_channels=256, kernel_size=3, padding=1)
		x = self.ReLU(x, inplace=True)
		x = self.MaxPool2d(x, kernel_size=3, stride=2)

		# b = [[[float(i + j * 6 + k * 36)/(256*6*6) for i in range(6)] for j in range(6)] for k in range(256)]
		# x = np.array(b)

		x = self.AdaptiveAvgPool2d(x, sizes=(6, 6))
		x = x.reshape(1, 256 * 6 * 6)
		x = self.Linear(x, weights=self.state_dict['classifier.1.weight'], bias=self.state_dict['classifier.1.bias'], in_features=256 * 6 * 6, out_features=4096)
		x = self.ReLU(x, inplace=True)
		x = self.Linear(x, weights=self.state_dict['classifier.4.weight'], bias=self.state_dict['classifier.4.bias'], in_features=4096, out_features=4096)
		x = self.ReLU(x, inplace=True)
		x = self.Linear(x, weights=self.state_dict['classifier.6.weight'], bias=self.state_dict['classifier.6.bias'], in_features=4096, out_features=1000)
		x = self.LogSoftmax(x, dim=1)
		return x

	def __call__(self, x):
		return self.forward(x)


def printX(x):
	# return
	print("Sum: ", float(x.abs().sum()), " Min: ", float(x.min()), " Max: ", float(x.max()))


class PyTorchAlexNet(nn.Module):
	def __init__(self, pretrained=False, num_classes=1000):
		super(PyTorchAlexNet, self).__init__()
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
		self.l14 = nn.Linear(256 * 6 * 6, 4096)
		self.l15 = nn.ReLU(inplace=True)
		self.l16 = nn.Linear(4096, 4096)
		self.l17 = nn.ReLU(inplace=True)
		self.l18 = nn.Linear(4096, num_classes)

		if pretrained:
			self.load_state_dict(model_zoo.load_url(model_urls['alexnet']))

	def forward(self, x):
		printX(x)
		x = self.l1(x)
		printX(x)
		x = self.l2(x)
		printX(x)
		x = self.l3(x)
		printX(x)
		x = self.l4(x)
		printX(x)
		x = self.l5(x)
		printX(x)
		x = self.l6(x)
		printX(x)
		x = self.l7(x)
		printX(x)
		x = self.l8(x)
		printX(x)
		x = self.l9(x)
		printX(x)
		x = self.l10(x)
		printX(x)
		x = self.l11(x)
		printX(x)
		x = self.l12(x)
		printX(x)
		x = self.l13(x)
		printX(x)
		x = x.view(x.size(0), 256 * 6 * 6)
		printX(x)
		x = self.l14(x)
		printX(x)
		x = self.l15(x)
		printX(x)
		x = self.l16(x)
		printX(x)
		x = self.l17(x)
		printX(x)
		x = self.l18(x)
		printX(x)
		return x


def roundToDecimals(x, decimals):
	return int(x * (10 ** decimals)) / (10 ** decimals)


def openPPM(path):
	with open(path, 'rb') as f:
		line = f.readline()
		if line.decode('ascii') != 'P6\n':
			return
		line = f.readline()
		while line.decode('ascii')[0] == '#':
			line = f.readline()

		width, height = [int(el) for el in line.split()]
		maxValue = int(f.readline())
		x = torch.zeros((1, 3, height, width), dtype=torch.float)
		for h in range(height):
			for w in range(width):
				for c in range(3):
					b = f.read(1)
					# while b.decode('ascii') == '\n':
					# 	b = f.read(1)
					num = int.from_bytes(b, byteorder='big')
					num = num / maxValue
					x[0, c, h, w] = round(num, 10)
	return x


def writeToFile(x):
	with open('output.txt', 'a+') as f:
		f.write(' '.join(f'{el:.10f}' for el in x.flatten().tolist()) + '\n')


def inference(model):
	labelNames = list(json.loads(open(LABELS_FILE, 'r').read()).values())
	if USE_PRETRANSFORMED_IMAGES:
		preprocess = transforms.Compose([
			transforms.ToTensor(),
		])
	else:
		preprocess = transforms.Compose([
			transforms.Resize(256),
			transforms.CenterCrop(224),
			transforms.ToTensor(),
			# transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225])
		])

	dataset = datasets.ImageFolder(data_dir + '/test', transform=preprocess)

	f = open('output.txt', 'w+')
	for image, label in dataset.imgs:
		# x = Image.open(image)
		# x = preprocess(x).unsqueeze(0)

		x = openPPM(image)
		x = model(x)
		top_class = x.argmax()

		f.write(' '.join(f'{el:.10f}' for el in x.flatten().tolist()) + '\n')

		print(image.split('/')[-1] + ': ' + labelNames[top_class])
	f.close()


def loadCustomStateDict(model):
	stateDict = {}
	with open("/home/tzanis/Workspace/Thesis/Projects/PyCharm/Scripts/AlexNetParametersProcessing/binaryParametersBig.txt") as f:
		parNum = int(f.readline())
		for key in model.state_dict().keys():
			dims = [int(el) for el in f.readline().split()]
			dimsNum = dims[0]
			dims = dims[1:]
			rows = 1
			for i in range(dimsNum - 1):
				rows *= dims[i]
			arr = [[float(el) for el in f.readline().split()] for _ in range(rows)]
			arr = np.array(arr)
			arr = arr.reshape(dims)
			stateDict[key] = torch.tensor(arr)
	model.load_state_dict(stateDict)


def main():
	model = PyTorchAlexNet()
	# model = alexnet(pretrained=True)
	# model.double()
	loadCustomStateDict(model)
	model.eval()

	# model = RawAlexNet()

	inference(model)


if __name__ == '__main__':
	main()
