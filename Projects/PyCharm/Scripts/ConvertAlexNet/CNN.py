# Find image datasets on: https://github.com/kensanata/numbers
import numpy as np


def convolute(image, pattern):
	xDim = image.shape[0] - pattern.shape[0] + 1
	yDim = image.shape[1] - pattern.shape[1] + 1
	res = np.zeros((xDim, yDim))

	for y in range(yDim):
		for x in range(xDim):
			for b in range(pattern.shape[1]):
				for a in range(pattern.shape[0]):
					res[x, y] += image[x + a, y + b] * pattern[a, b]
			res[x, y] = round(res[x, y] / (pattern.shape[0] * pattern.shape[1]), 2)
	return res


def ReLU(arr):
	for y in range(arr.shape[1]):
		for x in range(arr.shape[0]):
			arr[x, y] = 0 if arr[x, y] <= 0 else arr[x, y]
	return arr


def pool(image, windowSize, stride):
	a = windowSize + windowSize - (windowSize - stride)
	n = 0
	b = windowSize + n*stride
	xDim = np.ceil((image.shape[0] - windowSize) / stride) + 1
	yDim = np.ceil((image.shape[1] - windowSize) / stride) + 1
	res = np.zeros((xDim, yDim))

	return res


def main():
	image = np.array([
		[-1, -1, -1, -1, -1, -1, -1, -1, -1],
		[-1, 1, -1, -1, -1, -1, -1, 1, -1],
		[-1, -1, 1, -1, -1, -1, 1, -1, -1],
		[-1, -1, -1, 1, -1, 1, -1, -1, -1],
		[-1, -1, -1, -1, 1, -1, -1, -1, -1],
		[-1, -1, -1, 1, -1, 1, -1, -1, -1],
		[-1, -1, 1, -1, -1, -1, 1, -1, -1],
		[-1, 1, -1, -1, -1, -1, -1, 1, -1],
		[-1, -1, -1, -1, -1, -1, -1, -1, -1]
	])

	pattern1 = np.array([
		[1, -1, -1],
		[-1, 1, -1],
		[-1, -1, 1]
	])

	pattern2 = np.array([
		[-1, -1, 1],
		[-1, 1, -1],
		[1, -1, -1]
	])

	pattern3 = np.array([
		[1, -1, 1],
		[-1, 1, -1],
		[1, -1, 1]
	])

	conv1, conv2, conv3 = convolute(image, pattern1), convolute(image, pattern2), convolute(image, pattern3)
	print('Convolutions')
	print(conv1, conv2, conv3)

	relu1, relu2, relu3 = ReLU(conv1), ReLU(conv2), ReLU(conv3)
	print('ReLUs')
	print(relu1, relu2, relu3)


if __name__ == '__main__':
	main()
