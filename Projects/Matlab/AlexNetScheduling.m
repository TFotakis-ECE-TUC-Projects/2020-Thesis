clear;
% close all;
clc;


%% Definitions
% c: cycles per operation
global c p
c.memRead = 1;
c.mul = 1;
c.add = 1;
c.branch = 1;
c.memWrite = 1;
c.useTimings = 0;


%% Network
p.conv1 = 1;
p.conv2 = 1;
p.conv3 = 1;
p.conv4 = 1;
p.conv5 = 1;
p.maxpool1 = 1;
p.maxpool2 = 1;
p.maxpool3 = 1;
p.linear1 = 1;
p.linear2 = 1;
p.linear3 = 1;

subplot(3,2,1)
n1 = CreateNetwork();
n1 = ScheduleSerial(n1);
PrintSchedule(n1.arr, 'Serial');

subplot(3,2,2)
n2 = CreateNetwork();
n2 = SchedulePipelined(n2);
PrintSchedule(n2.arr, 'Pipelined 1x');

p.conv1 = 11*11;
p.conv2 = 5*5;
p.conv3 = 3*3;
p.conv4 = 3*3;
p.conv5 = 3*3;
p.maxpool1 = 3*3;
p.maxpool2 = 3*3;
p.maxpool3 = 3*3;
p.linear1 = 1024;
p.linear2 = 1024;
p.linear3 = 1024;

subplot(3,2,3)
n3 = CreateNetwork();
n3 = ScheduleSerial(n3);
PrintSchedule(n3.arr, 'Serial Kernels Parallelism');

subplot(3,2,4)
n4 = CreateNetwork();
n4 = SchedulePipelined(n4);
PrintSchedule(n4.arr, 'Pipelined Kernels Parallelism');

p.conv1 = 11*11*3;
p.conv2 = 5*5*64;
p.conv3 = 3*3*192;
p.conv4 = 3*3*384;
p.conv5 = 3*3*256;
p.maxpool1 = 3*3;
p.maxpool2 = 3*3;
p.maxpool3 = 3*3;
p.linear1 = 1024;
p.linear2 = 1024;
p.linear3 = 1024;

subplot(3,2,5)
n5 = CreateNetwork();
n5 = ScheduleSerial(n5);
PrintSchedule(n5.arr, 'Serial K*OC Parallelism');

subplot(3,2,6)
n6 = CreateNetwork();
n6 = SchedulePipelined(n6);
PrintSchedule(n6.arr, 'Pipelined K*OC Parallelism');


%% Layer Functions
function n = CreateNetwork()
global p;
n.c1 = TConv2DReLU(224, 224, 3, 64, 11, 4, 2, p.conv1, 'Conv1');
n.m1 = TMaxPool(64, 55, 55, 3, 2, p.maxpool1, 'MaxPool1');
n.c2 = TConv2DReLU(27, 27, 64, 192, 5, 1, 2, p.conv2, 'Conv2');
n.m2 = TMaxPool(192, 27, 27, 3, 2, p.maxpool2, 'MaxPool2');
n.c3 = TConv2DReLU(13, 13, 192, 384, 3, 1, 1, p.conv3, 'Conv3');
n.c4 = TConv2DReLU(13, 13, 384, 256, 3, 1, 1, p.conv4, 'Conv4');
n.c5 = TConv2DReLU(7, 7, 256, 256, 3, 1, 1, p.conv5, 'Conv5');
n.m3 = TMaxPool(256, 4, 4, 3, 2, p.maxpool3, 'MaxPool3');
n.l1 = TLinear(256 * 6 * 6, 4096, true, p.linear1, 'Linear1');
n.l2 = TLinear(4096, 4096, true, p.linear2, 'Linear2');
n.l3 = TLinear(4096, 1000, false, p.linear3, 'Linear3');
n.arr = [n.c1 n.m1 n.c2 n.m2 n.c3 n.c4 n.c5 n.m3 n.l1 n.l2 n.l3];
end


function ret = TConv2DReLU(h, w, ic, oc, k, s, p, parallel, name)
% h: Input height
% w: Input width
% ic: Input channels
% oc: Output channels
% k: Kernel size
% s: Stride
% p: Padding
% ipo: Input per output
% cpi: Cycles per input
% cpo: Cycles per output
% tc: Total cycles needed
% to: Total output

global c;

ipo = ic * k * k;
cpi = (1 - c.useTimings) + c.useTimings * (3 * c.memRead + 1 * c.mul + ...
	1 * c.add + 1 * c.branch + 2 * c.memWrite);

% oh: Output height
oh = (h + 2 * p - k) / s + 1;
oh = fix(oh);

% ow: Output width
ow = (w + 2 * p - k) / s + 1;
ow = fix(ow);

to = oc * oh * ow;

cpo = ipo * cpi / parallel;
tc = to * cpo;


ret.h = h;
ret.w = w;
ret.ic = ic;
ret.oc = oc;
ret.k = k;
ret.s = s;
ret.p = p;
ret.cpi = cpi;
ret.ipo = ipo;
ret.cpo = cpo;
ret.tc = tc;
ret.to = to;
ret.st = 0;
ret.en = 0;
ret.name = name;
end


function ret = TMaxPool(ic, h, w, k, s, parallel, name)
% ic: Input channels
% h: Input height
% w: Input width
% k: Kernel size
% s: Stride
% ipo: Input per output
% cpi: Cycles per input
% cpo: Cycles per output
% tc: Total cycles needed
% to: Total output
global c;

ipo = k * k;

% oh: Output height
oh = (h - k) / s + 1;
oh = fix(oh);

% ow: Output width
ow = (w - k) / s + 1;
ow = fix(ow);

cpi = (1 - c.useTimings) + c.useTimings * (2 * c.memRead + ...
	1 * c.branch * 1 * c.memWrite);

cpo = ipo * cpi / parallel;
to = ic * oh * ow;
tc = to * cpo;

ret.ic = ic;
ret.h = h;
ret.w = w;
ret.k = k;
ret.s = s;
ret.p = 0;
ret.oc = ic;
ret.ipo = ipo;
ret.cpi = cpi;
ret.cpo = cpo;
ret.tc = tc;
ret.to = to;
ret.st = 0;
ret.en = 0;
ret.name = name;
end


function ret = TLinear(ic, oc, useReLU, parallel, name)
% ic: Input channels
% oc: Output channels
% ipo: Input per output
% cpi: Cycles per input
% cpo: Cycles per output
% tc: Total cycles needed

global c;

cpi = (1 - c.useTimings) + c.useTimings * (3 * c.memRead + 1 * c.mul + ...
	1 * c.add + 1 * useReLU * c.branch + 1 * c.memWrite);
ipo = ic;
cpo = ipo * cpi / parallel;
tc = oc * cpo;

ret.ic = ic;
ret.oc = oc;
ret.ipo = ipo;
ret.cpi = cpi;
ret.cpo = cpo;
ret.tc = tc;
ret.st = 0;
ret.en = 0;

ret.h = 0;
ret.w = 0;
ret.k = 0;
ret.s = 0;
ret.p = 0;
ret.to = oc;
ret.name = name;
end


%% Calc Funcitons
function n = ScheduleSerial(n)
n.arr(1).st = 0;
n.arr(1).en = n.c1.tc;
for i = 2:11
	n.arr(i).st = n.arr(i-1).en;
	n.arr(i).en = n.arr(i).st + n.arr(i).tc;
end
end


function t = GetStartingTime(arr, index)
if index == 1
	t = 0;
	return;
end

[~, layer] = max([arr(1:index-1).tc]);

y = @(x, c) min([(c.k + (x - 1) * c.s) - c.p, c.h, c.w]);
inputs = 1;
for i = index:-1:layer+1
	inputs = y(inputs, arr(i));
end

if contains([arr(layer+1:index).name], 'Conv')
	oc = arr(layer).oc;
else
	oc = 1;
end

t = inputs^2 * oc * arr(layer).cpo + arr(layer).st;
for i = index-1:-1:layer+1
	t = t + arr(i).cpo;
end
end


function t = GetEndingTime(arr, index)
if index == 1
	t = arr(index).tc;
elseif arr(index - 1).en - arr(index - 1).st > arr(index).tc
	t = arr(index - 1).en + arr(index).cpo;
else
	t = arr(index).st + arr(index).tc;
end
end


function n = SchedulePipelined(n)
for i=1:8
	n.arr(i).st = GetStartingTime(n.arr, i);
	n.arr(i).en = GetEndingTime(n.arr, i);
end

n.arr(9).st = n.arr(8).en;
n.arr(9).en = n.arr(9).st + n.arr(9).tc;

n.arr(10).st = n.arr(9).en;
n.arr(10).en = n.arr(10).st + n.arr(10).tc;

n.arr(11).st = n.arr(10).en;
n.arr(11).en = n.arr(11).st + n.arr(11).tc;
end


function PrintSchedule(arr, figureName)
cla;
lineWidth = 10;
% figure('Name', figureName,'NumberTitle','off')
title(figureName);
xlabel('Clock cycles');
ylim([0 12]);
% xlim([0 7e8]);
set(gca, 'ytick', [])
hold on;

a = [[arr(:).st]; [arr(:).en]];
b = [11:-1:1; 11:-1:1];
plot(a, b, 'LineWidth', lineWidth);
text([arr(:).st], 11:-1:1, string({arr(:).name}))
end