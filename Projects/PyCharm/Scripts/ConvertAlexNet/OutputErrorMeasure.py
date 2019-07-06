import numpy as np

pf = open('/home/tzanis/Workspace/Thesis/Projects/PyCharm/Scripts/ConvertAlexNet/outputWhole.txt', 'r')
cf = open('/home/tzanis/Workspace/Thesis/Projects/CLion/AlexNet/output.txt', 'r')

plines = pf.readlines()
clines = cf.readlines()

pf.close()
cf.close()

if len(plines) != len(clines):
	exit()

p = np.array([[float(el) for el in line.split()] for line in plines])
c = np.array([[float(el) for el in line.split()] for line in clines])

picErr = abs(p - c).sum(axis=1)
minErr = picErr.min()
minIndex = picErr.argmin()
maxErr = picErr.max()
maxIndex = picErr.argmax()
err = picErr.sum() / len(p)
correct = sum(p.argmax(1) == c.argmax(1))

print('ME: ', err)
print('Min: ', minErr, ' Min Index: ', minIndex)
print('Max: ', maxErr, ' Max Index: ', maxIndex)
print('Correct: ', correct)
