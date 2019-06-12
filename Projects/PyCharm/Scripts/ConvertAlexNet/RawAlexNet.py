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


class PyTorchAlexNet(nn.Module):
	def __init__(self, pretrained=False, num_classes=1000):
		super(PyTorchAlexNet, self).__init__()
		self.features = nn.Sequential(
			nn.Conv2d(3, 64, kernel_size=11, stride=4, padding=2),
			nn.ReLU(inplace=True),
			nn.MaxPool2d(kernel_size=3, stride=2),
			nn.Conv2d(64, 192, kernel_size=5, padding=2),
			nn.ReLU(inplace=True),
			nn.MaxPool2d(kernel_size=3, stride=2),
			nn.Conv2d(192, 384, kernel_size=3, padding=1),
			nn.ReLU(inplace=True),
			nn.Conv2d(384, 256, kernel_size=3, padding=1),
			nn.ReLU(inplace=True),
			nn.Conv2d(256, 256, kernel_size=3, padding=1),
			nn.ReLU(inplace=True),
			nn.MaxPool2d(kernel_size=3, stride=2),
		)
		self.avgpool = nn.AdaptiveAvgPool2d((6, 6))
		self.classifier = nn.Sequential(
			nn.Dropout(),
			nn.Linear(256 * 6 * 6, 4096),
			nn.ReLU(inplace=True),
			nn.Dropout(),
			nn.Linear(4096, 4096),
			nn.ReLU(inplace=True),
			nn.Linear(4096, num_classes),
			nn.LogSoftmax(dim=1)
		)
		if pretrained:
			self.load_state_dict(model_zoo.load_url(model_urls['alexnet']))

	def forward(self, x):
		x = x.reshape(1, x.shape[0], x.shape[1], x.shape[2])
		x = self.features(x)
		x = self.avgpool(x)
		x = x.view(x.size(0), 256 * 6 * 6)
		x = self.classifier(x)
		return x


def inference(model):
	labelNames = list(json.loads(open(LABELS_FILE, 'r').read()).values())
	preprocess = transforms.Compose([
		transforms.Resize(256),
		transforms.CenterCrop(224),
		transforms.ToTensor(),
		# transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225])
	])

	# dataset = datasets.ImageFolder(data_dir + '/train', transform=preprocess)
	dataset = datasets.ImageFolder(data_dir + '/test', transform=preprocess)

	for image, label in dataset.imgs:
		pil = Image.open(image)
		pil_processed = preprocess(pil)
		fc_out = model(pil_processed)
		top_class = fc_out.argmax()
		print(dataset.classes[label] + ': ' + labelNames[top_class])


def main():
	# model = PyTorchAlexNet(pretrained=True)
	# model = alexnet(pretrained=True)
	model = RawAlexNet()
	inference(model)


if __name__ == '__main__':
	main()
