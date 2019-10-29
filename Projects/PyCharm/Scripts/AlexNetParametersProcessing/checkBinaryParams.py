import struct
import numpy as np

with open('output/alexnetParams', 'rb') as f:
	pNum = struct.unpack('i', f.read(4))[0]
	print(pNum)
	for _ in range(pNum):
		dNum = struct.unpack('i', f.read(4))[0]
		dims = struct.unpack('i' * dNum, f.read(4 * dNum))
		print(dims)
		dims = np.array(dims).prod()
		weights = struct.unpack('f' * dims, f.read(4 * dims))
f.close()
print("Done")
exit()
