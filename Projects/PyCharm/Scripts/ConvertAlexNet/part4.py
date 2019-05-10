import helper
import torch
from torch import nn
from torchvision import datasets, transforms
from torch import optim


def loadData():
	# Define a transform to normalize the data
	# transform = transforms.Compose([
	# 	transforms.ToTensor(),
	# 	transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))
	# ])

	# Define a transform to normalize the data
	transform = transforms.Compose([
		transforms.ToTensor(),
		transforms.Normalize((0.5,), (0.5,)),
	])

	# Download and load the training data
	trainset = datasets.FashionMNIST('~/.pytorch/F_MNIST_data/', download=True, train=True, transform=transform)
	trainloader = torch.utils.data.DataLoader(trainset, batch_size=64, shuffle=True)

	# Download and load the test data
	testset = datasets.FashionMNIST('~/.pytorch/F_MNIST_data/', download=True, train=False, transform=transform)
	testloader = torch.utils.data.DataLoader(testset, batch_size=64, shuffle=True)

	# image, label = next(iter(trainloader))
	# helper.imshow(image[0, :])
	return trainloader, testloader


def getModel():
	model = nn.Sequential(
		nn.Linear(784, 256),
		nn.ReLU(),
		nn.Linear(256, 128),
		nn.ReLU(),
		nn.Linear(128, 64),
		nn.ReLU(),
		nn.Linear(64, 10),
		nn.LogSoftmax(dim=1)
	)
	criterion = nn.NLLLoss()
	optimizer = optim.Adam(model.parameters(), lr=0.003)

	return model, criterion, optimizer


def trainNetwork(trainloader, model, criterion, optimizer, epochs=10):
	for e in range(epochs):
		running_loss = 0
		for images, labels in trainloader:
			images = images.view(images.shape[0], -1)

			optimizer.zero_grad()
			output = model(images)
			loss = criterion(output, labels)
			loss.backward()
			optimizer.step()

			running_loss += loss.item()
		else:
			print(f"Training loss: {running_loss / len(trainloader)}")


def testNetwork(testloader, model):
	dataiter = iter(testloader)
	images, labels = dataiter.next()
	img = images[0]
	# Convert 2D image to 1D vector
	img = img.resize_(1, 784)

	# Calculate the class probabilities (softmax) for img
	ps = torch.exp(model(img))
	print(ps)
	# Plot the image and probabilities
	# helper.view_classify(img.resize_(1, 28, 28), ps, version='Fashion')


def main():
	trainloader, testloader = loadData()
	model, criterion, optimizer = getModel()
	trainNetwork(trainloader, model, criterion, optimizer, epochs=10)
	testNetwork(testloader, model)


if __name__ == '__main__':
	main()
