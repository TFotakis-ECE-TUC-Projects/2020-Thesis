import torch
from torch import nn
from torch import optim
from torch.utils.data import DataLoader
from torchvision import datasets
from torchvision import transforms

from Torchvision.alexnet2 import alexnet

data_dir = 'Cat_Dog_data'

preprocess = transforms.Compose([
	transforms.Resize(256),
	transforms.CenterCrop(224),
	transforms.ToTensor(),
	transforms.Normalize(
		mean=[0.485, 0.456, 0.406],
		std=[0.229, 0.224, 0.225]
	)
])

train_data = datasets.ImageFolder(data_dir + '/train', transform=preprocess)
test_data = datasets.ImageFolder(data_dir + '/test', transform=preprocess)

trainloader = DataLoader(train_data, batch_size=64, shuffle=True)
testloader = DataLoader(test_data, batch_size=64)

model = alexnet(pretrained=True, num_classes=2)

criterion = nn.NLLLoss()
optimizer = optim.Adam(model.classifier.parameters(), lr=0.003)

epochs = 2
steps = 0
running_loss = 0
print_every = 5

for epoch in range(epochs):
	for images, labels in trainloader:
		optimizer.zero_grad()
		logps = model(images)
		loss = criterion(logps, labels)
		loss.backward()
		optimizer.step()

		running_loss += loss.item()

		if steps % print_every == 0:
			model.eval()
			test_loss = 0
			accuracy = 0

			for images, labels in testloader:
				logps = model(images)
				loss = criterion(logps, labels)
				test_loss += loss.item()

				ps = torch.exp(logps)
				top_ps, top_class = ps.topk(1, dim=1)
				equality = top_class == labels.view(*top_class.shape)
				accuracy += torch.mean(equality.type(torch.FloatTensor)).item()

			train_loss = running_loss / print_every
			test_loss = test_loss / len(testloader)
			accuracy = accuracy / len(testloader)
			print(f"Epoch: {epoch + 1}/{epochs} | Train loss: {train_loss:.3f} | Test loss: {test_loss:.3f} | Accuracy: {accuracy:.3f}")

			running_loss = 0
			model.train()