function n = CreateNetwork()
global p;
n.c1 = TConv2DReLU(224, 224, 3, 96, 11, 4, 2, p.conv1, 'Conv1');
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
end