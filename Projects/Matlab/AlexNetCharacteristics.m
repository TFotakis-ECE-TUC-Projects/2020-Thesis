clear;
close all;
clc;

PlotLayerCharacteristics(224, 55, 3, 96, 11, 4, 2, 'FilterFirst', 'Conv1')
PlotLayerCharacteristics(55, 27, 96, 96, 3, 2, 0, 'FilterFirst', 'MaxPool1')
PlotLayerCharacteristics(27, 27, 96, 256, 5, 1, 2, 'FilterFirst', 'Conv2')
PlotLayerCharacteristics(27, 13, 256, 256, 3, 2, 0, 'FilterFirst', 'MaxPool2')
PlotLayerCharacteristics(13, 13, 256, 384, 3, 1, 1, 'FilterFirst', 'Conv3')
PlotLayerCharacteristics(13, 13, 384, 384, 3, 1, 1, 'FilterFirst', 'Conv4')
PlotLayerCharacteristics(13, 13, 384, 256, 3, 1, 1, 'FilterFirst', 'Conv5')
PlotLayerCharacteristics(13, 6, 256, 256, 3, 2, 0, 'FilterFirst', 'MaxPool3')

function PlotLayerCharacteristics(din, dout, ic, oc, k, s, p, order, layerName)
figure('Name', layerName, 'units','normalized','outerposition',[0 0 1 1])
arr = OutputAddressGenerator(dout, oc, order);
PrintAddressNew(arr);
PrintMemoryLocallity(arr, din, k, s, p, ic)
end

function res = OutputAddressGenerator(d, oc, order)
% Order: 'ImageFirst' or 'FilterFirst'
res = zeros(d * d * oc, 3);

if strcmp(order, 'ImageFirst')
	index = 1;
	for k=0:oc-1
		for i=0:d-1
			for j=0:i
				res(index, :) = [k, i, j];
				index = index + 1;
			end
			for j=j - 1:-1:0
				res(index, :) = [k, j, i];
				index = index + 1;
			end
		end
	end
elseif strcmp(order, 'FilterFirst')
	index = 1;
	for i=0:d-1
		for j=0:i
			for k=0:oc-1
				res(index, :) = [k, i, j];
				index = index + 1;
			end
		end
		for j=j - 1:-1:0
			for k=0:oc-1
				res(index, :) = [k, j, i];
				index = index + 1;
			end
		end
	end
end
end

function res = InputAddressGenerator(outAddr, d, k, s, p, ic)
rs = outAddr(1) * s - p;
rf = rs + k;
rf = rf * (rf <= d) + d * (rf > d);
rs = rs * (rs >= 0);

cs = outAddr(2) * s - p;
cf = cs + k;
cf = cf * (cf <= d) + d * (cf > d);
cs = cs * (cs >= 0);

res = zeros((rf - rs) * (cf - cs) * ic, 3);
index = 1;
for k=0:ic - 1
	for i=rs:rf - 1
		for j=cs:cf - 1
			res(index, :) = [k, i, j];
			index = index + 1;
		end
	end
end
end

function PrintMemoryLocallity(arr, din, k, s, p, ic)
arr = arr(arr(:, 1)==0, 2:3);

im = zeros(din);
for i=1:length(arr)
	inputs = InputAddressGenerator(arr(i, :), din, k, s, p, ic);
	inputs = inputs(inputs(:, 1)==0, 2:3) + 1;
	
	for j=1:length(inputs)
		im(inputs(j, 1), inputs(j, 2)) = im(inputs(j, 1), inputs(j, 2)) + 1;
	end
end

subplot(2, 2, 1)
image(im, 'CDataMapping','scaled')
colorbar('Ticks', unique(im));
title("Input image's pixels usage frequency")

subplot(2, 2, 2)
histogram(im)
set(gca, 'xtick', unique(im));
title("Input image's pixels usage histogram")
end

function PrintAddress(arr)
subplot(2, 2, 3);
lineWidth = 20;

oc = max(arr(:, 1));
d = max(arr(:, 2));
filterFirst = arr(1, 2:3) == arr(2, 2:3);

arr = arr(arr(:, 1)==0, 2:3);
arr = arr.*[-1, 1];

a = [[arr(:, 2)]' + 0.2; [arr(:, 2) + 0.8]'];
b = [arr(:, 1)'; arr(:, 1)'];

plot(a, b, 'LineWidth', lineWidth, 'Color', 'Blue');

if filterFirst
	labels = string(0:oc:oc * length(arr) - 1);
else
	labels = string(0:length(arr) - 1);
end
text(arr(:, 2)' + 0.2, arr(:, 1)', labels, 'Color', 'White')

set(gca, 'ytick', [], 'xtick', []);
ylim([-(max(arr(:, 2)) + 1), -(min(arr(:, 2)) - 1)])

if filterFirst
	title('Filter First, OC: ' + string(oc));
else
	title('Image First');
end

end

function PrintAddressNew(arr)
subplot(2, 2, 3);

d = max(arr(:, 2));

arr = arr(arr(:, 1)==0, 2:3) + 1;

im = zeros(d);
index = 0;
for i=1:length(arr)
	im(arr(i, 1), arr(i, 2)) = index;
	index = index + 1;
end
image(im, 'CDataMapping','scaled')
colorbar('Ticks', [0, index-1], 'TickLabels', {'Start', 'End'}, 'Direction','reverse')
title('Time when outputs are created')
end