import json
import numpy as np
import matplotlib.pyplot as plt

with open('/home/tzanis/Workspace/Thesis/Projects/PyCharm/Scripts/AlexNetParametersProcessing/alexnetFlatParameters.json', 'r') as fp:
	x = json.load(fp)


bits = [3, 7]
plotRange = [-1, 1]

for bitwidth in bits:
	bins = 2 ** bitwidth
	fig = plt.figure()
	fig.suptitle('#Bins: ' + str(bins))

	plt.subplot(311)
	plt.hist(x, bins=bins)

	plt.subplot(312)
	plt.hist(x, bins=bins)
	plt.yscale('log')

	plt.subplot(313)
	plt.hist(np.abs(x), bins=bins)
	plt.yscale('log')

	plt.show()
	fig.savefig('bins ' + str(bins) + '.jpg')

	fig = plt.figure()
	fig.suptitle('#Bins: ' + str(bins) + ' Range: ' + str(plotRange))

	plt.subplot(311)
	plt.hist(x, bins=bins, range=plotRange)

	plt.subplot(312)
	plt.hist(x, bins=bins, range=plotRange)
	plt.yscale('log')

	plt.subplot(313)
	plt.hist(np.abs(x), bins=bins, range=[0, 1])
	plt.yscale('log')

	plt.show()
	fig.savefig('bins ' + str(bins) + ', range (-1, 1).jpg')

print('This is the end')
