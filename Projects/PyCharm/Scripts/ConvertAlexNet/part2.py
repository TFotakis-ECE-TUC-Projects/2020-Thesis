import helper
# import numpy as np
import matplotlib.pyplot as plt
import torch
import torch.nn.functional as F
from torch import nn
from torchvision import datasets, transforms


def activation(x):
	""" Sigmoid activation function

		Arguments
		---------
		x: torch.Tensor
	"""
	return 1 / (1 + torch.exp(-x))


def softmax(x):
	e = torch.exp(x)
	esum = e.sum(dim=1)
	n_esum = len(esum)
	esum = esum.reshape((n_esum, 1))
	y = e / esum
	return y


def buildNetworkWithTorch(flatImages):
	# Define the size of each layer in our network
	n_batchImages, n_input = flatImages.shape  # Number of input units, must match number of input features
	n_hidden = 256  # Number of hidden units
	n_output = 10  # Number of output units

	# Weights for inputs to hidden layer
	W1 = torch.randn(n_input, n_hidden)
	# Weights for hidden layer to output layer
	W2 = torch.randn(n_hidden, n_output)

	# and bias terms for hidden and output layers
	B1 = torch.randn((1, n_hidden))
	B2 = torch.randn((1, n_output))

	h = activation(torch.mm(flatImages, W1) + B1)
	y = activation(torch.mm(h, W2) + B2)
	# print(y.shape)
	# print(y)
	out = y

	probabilities = softmax(out)
	print(probabilities.shape)
	print(probabilities.sum(dim=1))


class DemoNetwork(nn.Module):
	def __init__(self):
		super().__init__()
		# Inputs to hidden layer linear transformation
		self.hidden = nn.Linear(784, 256)
		# Output layer, 10 units - one for each digit
		self.output = nn.Linear(256, 10)

	def forward(self, x):
		# Hidden layer with sigmoid activation
		x = F.sigmoid(self.hidden(x))
		# Output layer with softmax activation
		x = F.softmax(self.output(x), dim=1)

		return x


class Network(nn.Module):
	def __init__(self):
		super().__init__()
		self.fc1 = nn.Linear(784, 128)
		self.fc2 = nn.Linear(128, 64)
		self.fc3 = nn.Linear(64, 10)

	def forward(self, x):
		x = F.relu(self.fc1(x))
		x = F.relu(self.fc2(x))
		x = F.softmax(self.fc3(x), dim=1)
		return x


def buildNetworkWithPyTorch(flatImages, trainloader):
	model = Network()
	print(model.fc1.weight)
	print(model.fc1.bias)
	# Set biases to all zeros
	model.fc1.bias.data.fill_(0)
	# sample from random normal with standard dev = 0.01
	model.fc1.weight.data.normal_(std=0.01)
	# Grab some data
	dataiter = iter(trainloader)
	images, labels = dataiter.next()

	# Resize images into a 1D vector, new shape is (batch size, color channels, image pixels)
	images.resize_(64, 1, 784)
	# or images.resize_(images.shape[0], 1, 784) to automatically get batch size

	# Forward pass through the network
	img_idx = 0
	ps = model.forward(images[img_idx, :])

	img = images[img_idx]
	helper.view_classify(img.view(1, 28, 28), ps)


def getData():
	# Define a transform to normalize the data
	transform = transforms.Compose([
		transforms.ToTensor(),
		transforms.Normalize((0.5,), (0.5,)),
	])

	# Download and load the training data
	trainset = datasets.MNIST('~/.pytorch/MNIST_data/', download=True, train=True, transform=transform)
	trainloader = torch.utils.data.DataLoader(trainset, batch_size=64, shuffle=True)

	dataiter = iter(trainloader)
	images, labels = dataiter.next()
	# print(type(images))
	# print(images.shape)
	# print(labels.shape)

	plt.imshow(images[1].numpy().squeeze(), cmap='Greys_r')

	flatImages = images.flatten(1)
	# print(flatImages.shape)
	return flatImages, trainloader


def main():
	flatImages, trainloader = getData()
	buildNetworkWithTorch(flatImages)
	buildNetworkWithPyTorch(flatImages, trainloader)


main()
