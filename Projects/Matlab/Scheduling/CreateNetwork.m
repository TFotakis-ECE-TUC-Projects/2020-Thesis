function n = CreateNetwork()
global p printTimings;
n.c1 = TConv2DReLU(224, 224, 3, 64, 11, 4, 2, p.conv1, 'Conv1');
n.m1 = TMaxPool(64, 55, 55, 3, 2, p.maxpool1, 'MaxPool1');
n.c2 = TConv2DReLU(27, 27, 64, 192, 5, 1, 2, p.conv2, 'Conv2');
n.m2 = TMaxPool(192, 27, 27, 3, 2, p.maxpool2, 'MaxPool2');
n.c3 = TConv2DReLU(13, 13, 192, 384, 3, 1, 1, p.conv3, 'Conv3');
n.c4 = TConv2DReLU(13, 13, 384, 256, 3, 1, 1, p.conv4, 'Conv4');
n.c5 = TConv2DReLU(13, 13, 256, 256, 3, 1, 1, p.conv5, 'Conv5');
n.m3 = TMaxPool(256, 13, 13, 3, 2, p.maxpool3, 'MaxPool3');
n.l1 = TLinear(256 * 6 * 6, 4096, true, p.linear1, 'Linear1');
n.l2 = TLinear(4096, 4096, true, p.linear2, 'Linear2');
n.l3 = TLinear(4096, 1000, false, p.linear3, 'Linear3');
n.arr = [n.c1 n.m1 n.c2 n.m2 n.c3 n.c4 n.c5 n.m3 n.l1 n.l2 n.l3];

n.totalWeights = 0;
n.totalMemory = 0;
n.totalOutputMemory = 3 * 224 * 224; %one byte per color of the input image
n.totalOutputs = 2 * 224 * 224; % data for every color of the input image
for i = 1:11
	n.totalWeights = n.totalWeights + n.arr(i).totalWeights;
    n.totalMemory = n.totalMemory + n.arr(i).totalMemory;
    n.totalOutputMemory = n.totalOutputMemory + n.arr(i).totalOutputMemory;
    n.totalOutputs = n.totalOutputs + n.arr(i).to;
end

if printTimings
fprintf("Image: Outputs %d$ & %d & %0.2f \\\\\n", ...
        3 * 224 * 224, 3 * 224 * 224, ...
        3 * 224 * 224 / n.totalOutputMemory * 100);
end

for i = 1:11
    n.arr(i).totalMemoryPerc = n.arr(i).totalMemory / n.totalMemory * 100;
    n.arr(i).totalOutputMemoryPerc = n.arr(i).totalOutputMemory / n.totalOutputMemory * 100;
	
	if printTimings
	fprintf("%s: Weights %d$ & %d & %0.2f \\\\, Outputs %d$ & %d & %0.2f \\\\\n", ...
        n.arr(i).name, n.arr(i).totalWeights, n.arr(i).totalMemory, ...
        n.arr(i).totalMemoryPerc, ...
        n.arr(i).to, n.arr(i).totalOutputMemory, ...
        n.arr(i).totalOutputMemoryPerc);
	end
end

if printTimings
fprintf("Totals: Weights %d$ & %d & %0.2f \\\\, Outputs %d$ & %d & %0.2f \\\\\n", ...
        n.totalWeights, n.totalMemory, 100, ...
        n.totalOutputs, n.totalOutputMemory, 100);
end
end