import torch
from torch import nn
from torch import optim
from torchvision import datasets, transforms, models
from Torchvision.alexnet2 import alexnet
data_dir = 'Cat_Dog_data'

# TODO: Define transforms for the training data and testing data
train_transforms = transforms.Compose([
	# transforms.RandomRotation(30),
	transforms.RandomResizedCrop(224),
	transforms.RandomHorizontalFlip(),
	transforms.ToTensor(),
	transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])
])

test_transforms = transforms.Compose([
	transforms.Resize(255),
	transforms.CenterCrop(224),
	transforms.ToTensor(),
	transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])
])

normalize = transforms.Normalize(
	mean=[0.485, 0.456, 0.406],
	std=[0.229, 0.224, 0.225]
)
preprocess = transforms.Compose([
	transforms.Resize(256),
	transforms.CenterCrop(224),
	transforms.ToTensor(),
	normalize
])


# Pass transforms in here, then run the next cell to see how the transforms look
# train_data = datasets.ImageFolder(data_dir + '/train', transform=train_transforms)
# test_data = datasets.ImageFolder(data_dir + '/test', transform=test_transforms)
train_data = datasets.ImageFolder(data_dir + '/train', transform=preprocess)
test_data = datasets.ImageFolder(data_dir + '/test', transform=preprocess)

trainloader = torch.utils.data.DataLoader(train_data, batch_size=64, shuffle=True)
testloader = torch.utils.data.DataLoader(test_data, batch_size=64)

# Use GPU if it's available
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

# model = alexnet(num_classes=2)
model = alexnet(pretrained=True, num_classes=2)
# model = models.densenet121(pretrained=True)
print(model)

# Freeze parameters so we don't backprop through them
for param in list(model.parameters())[:-1]:
	param.requires_grad = False

# model.classifier = nn.Sequential(
# 	nn.Dropout(0.5),
# 	nn.Linear(9216, 4096, bias=True),
# 	nn.ReLU(inplace=True),
# 	nn.Dropout(0.5),
# 	nn.Linear(4096, 4096, bias=True),
# 	nn.ReLU(inplace=True),
# 	nn.Linear(4096, 1000, bias=True),
# 	nn.ReLU(inplace=True),
# 	nn.Linear(1000, 500, bias=True),
# 	nn.ReLU(inplace=True),
# 	nn.Linear(500, 2, bias=True),
# 	nn.ReLU(inplace=True),
# 	nn.LogSoftmax(dim=1)
# )

criterion = nn.NLLLoss()

# Only train the classifier parameters, feature parameters are frozen
optimizer = optim.Adam(model.classifier.parameters(), lr=0.003)

model.to(device)

epochs = 2
steps = 0
running_loss = 0
print_every = 30

train_losses, test_losses = [], []

for epoch in range(epochs):
	for inputs, labels in trainloader:
		steps += 1
		# Move input and label tensors to the default device
		inputs, labels = inputs.to(device), labels.to(device)

		optimizer.zero_grad()

		logps = model.forward(inputs)
		loss = criterion(logps, labels)
		loss.backward()
		optimizer.step()

		running_loss += loss.item()

		if steps % print_every == 0:
			# test_loss = 0
			# accuracy = 0
			# model.eval()
			# with torch.no_grad():
			# 	for inputs, labels in testloader:
			# 		inputs, labels = inputs.to(device), labels.to(device)
			# 		logps = model.forward(inputs)
			# 		batch_loss = criterion(logps, labels)
			#
			# 		test_loss += batch_loss.item()
			#
			# 		# Calculate accuracy
			# 		ps = torch.exp(logps)
			# 		top_p, top_class = ps.topk(1, dim=1)
			# 		equals = top_class == labels.view(*top_class.shape)
			# 		accuracy += torch.mean(equals.type(torch.FloatTensor)).item()
			#
			# print(f"Epoch {epoch + 1}/{epochs}.. "
			#       f"Train loss: {running_loss / print_every:.3f}.. "
			#       f"Test loss: {test_loss / len(testloader):.3f}.. "
			#       f"Test accuracy: {accuracy / len(testloader):.3f}")
			# running_loss = 0
			# model.train()
			test_loss = 0
			accuracy = 0
			with torch.no_grad():
				for images, labels in testloader:
					log_ps = model.forward(images)
					# log_ps = model(images)
					test_loss += criterion(log_ps, labels)
					ps = torch.exp(log_ps)
					top_p, top_class = ps.topk(1, dim=1)
					equals = top_class == labels.view(*top_class.shape)
					accuracy += torch.mean(equals.type(torch.FloatTensor))
			running_loss_percentage = running_loss / len(trainloader)
			test_loss_percentage = test_loss / len(testloader)
			train_losses.append(running_loss_percentage)
			test_losses.append(test_loss_percentage)

			print(
				'Epoch: {}/{}'.format(epoch + 1, epochs),
				'Training Loss: {:.3f}'.format(running_loss_percentage),
				'Testing Loss: {:.3f}'.format(test_loss_percentage),
				'Test Accuracy: {:.3f}'.format(accuracy / len(testloader))
			)
