import torch.utils.model_zoo as model_zoo
import numpy as np

model_urls = {'alexnet': 'https://download.pytorch.org/models/alexnet-owt-4df8aa71.pth'}
state_dict = model_zoo.load_url(model_urls['alexnet'])
state_dict = {key: state_dict[key].detach().numpy() for key in state_dict}

with open("output/alexnetParams", 'wb') as fp:
	arr = np.array([len(state_dict.keys())], dtype='int32')
	arr.tofile(fp)
	for k in state_dict.keys():
		arr = np.array([len(state_dict[k].shape)] + list(state_dict[k].shape), dtype='int32')
		arr.tofile(fp)
		arr = state_dict[k].flatten()
		arr.tofile(fp)
fp.close()
print("Done")
exit()
