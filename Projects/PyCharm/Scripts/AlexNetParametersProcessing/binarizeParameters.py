import torch.utils.model_zoo as model_zoo
import numpy


def joinNumArray(arr):
	if type(arr) is numpy.ndarray:
		return ' '.join(f"{el:.30f}" for el in arr)
	return ' '.join(str(el) for el in arr)


def joinMatrix(arr):
	dim1 = 1
	for dim in arr.shape[:-1]:
		dim1 *= dim
	dim2 = arr.shape[-1]

	return '\n'.join(joinNumArray(el) for el in state_dict[k].reshape((dim1, dim2)))


model_urls = {'alexnet': 'https://download.pytorch.org/models/alexnet-owt-4df8aa71.pth'}
state_dict = model_zoo.load_url(model_urls['alexnet'])
state_dict = {key: state_dict[key].detach().numpy() for key in state_dict}

string = str(len(state_dict.keys())) + '\n'
for k in state_dict.keys():
	string += str(len(state_dict[k].shape)) + ' ' + joinNumArray(state_dict[k].shape) + '\n'
	if len(state_dict[k].shape) == 1:
		string += joinNumArray(state_dict[k]) + '\n'
	else:
		string += joinMatrix(state_dict[k]) + '\n'


with open("binaryParametersBig.txt", 'w') as fp:
	fp.write(string)

print("Done")
