import argparse
import json
import time
from PIL import Image
import torch.nn as nn
import torch.utils.model_zoo as model_zoo
from torchvision import datasets
from torchvision import transforms
from torchvision.models import alexnet
from tqdm import tqdm
import torch


USE_GPU = False
IMAGES_DIRECTORY = 'Data'
LABELS_FILE = 'labels.json'
model_urls = {'alexnet': 'https://download.pytorch.org/models/alexnet-owt-4df8aa71.pth'}


def inference(model, batch_size, workers):
	labelNames = list(json.loads(open(LABELS_FILE, 'r').read()).values())
	preprocess = transforms.Compose([
		transforms.Resize(256),
		transforms.CenterCrop(224),
		transforms.ToTensor(),
	])

	dataset = datasets.ImageFolder(IMAGES_DIRECTORY, transform=preprocess)
	dataloader = torch.utils.data.DataLoader(dataset, batch_size=batch_size, shuffle=False, num_workers=workers)

	totalTime = 0
	totalCUDATime = 0
	forwardTime = 0
	with torch.no_grad():
		for x, _ in dataloader:
			startTime = time.time()
			if USE_GPU:
				x = x.to('cuda')

			cudaTime = time.time()
			x = model(x)
			endTime = time.time()
			totalTime = totalTime + endTime - startTime
			totalCUDATime = totalCUDATime + cudaTime - startTime
			forwardTime = forwardTime + endTime - cudaTime

			top_class = x.argmax()
	print('\n---------------- Timings ----------------')
	print('- Average Latency (ms): ', round(totalTime / len(dataloader) * 1000, 2))
	print('- Average Throughput (images/sec): ', round(len(dataset.imgs) / totalTime, 2))
	print('- Cuda time (s): ', totalCUDATime)
	print('- Forward time (s): ', forwardTime)
	print('- Total time (s): ', totalTime)
	print('-----------------------------------------')


def main():
	global USE_GPU
	parser = argparse.ArgumentParser(description='Measure timings over CPU and GPU inferences of AlexNet')
	parser.add_argument("-b", default=1, help='Batch Size', required=False, type=int)
	parser.add_argument("-w", default=1, help='Workers', required=False, type=int)
	parser.add_argument("--gpu", action='store_true', default=False, help='use GPU for inference (default: CPU)')
	args = parser.parse_args()
	if args.gpu:
		USE_GPU = True

	print('\n\n-----------------------------------------')
	print('Running AlexNet on ' + ('GPU' if USE_GPU else 'CPU'))
	print('Num of Workers: ' + str(args.w))
	print('Batch Size ' + str(args.b))

	model = alexnet(pretrained=True)
	model.eval()
	if USE_GPU:
		model.cuda()
	inference(model, args.b , args.w)


if __name__ == '__main__':
	main()
