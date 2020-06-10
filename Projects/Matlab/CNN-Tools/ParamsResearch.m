clear;
close all;
clc;

p = LoadParams("Data/alexnetParams");
nBins = 100;

%% ---------------------------------------------------------------
%{
subplot(2, 2, 1);
flat = FlattenParams(p(1:2:end));
h = histogram(flat, nBins);
set(gca, "YScale", "log");
lim = max(abs(h.BinEdges));
xlim([-lim, lim]);
title("Weight distribution of all layers");
xlabel("Weight Value");
ylabel("Count");

subplot(2, 2, 2);
threshold = 0.02;
flatZeroed = flat;
flatZeroed(flatZeroed > -threshold & flatZeroed < threshold) = [];
h = histogram(flatZeroed, nBins);
set(gca, "YScale", "log");
lim = max(abs(h.BinEdges));
xlim([-lim, lim]);
title(strcat("Weight distribution of all layers, pruned with threshold: ", num2str(threshold)));
xlabel("Weight Value");
ylabel("Count");

subplot(2, 2, 3);
flatLinear = FlattenParams(p(11:2:end));
h = histogram(flatLinear, nBins);
set(gca, "YScale", "log");
lim = max(abs(h.BinEdges));
xlim([-lim, lim]);
title("Weight distribution of all FC layers");
xlabel("Weight Value");
ylabel("Count");
%}

%% ---------------------------------------------------------------
titles = [
	"Weight distribution of 1st Convolution layer"
	"Weight distribution of 2nd Convolution layer"
	"Weight distribution of 3rd Convolution layer"
	"Weight distribution of 4th Convolution layer"
	"Weight distribution of 5th Convolution layer"
	"Weight distribution of 1st FC layer"
	"Weight distribution of 2nd FC layer"
	"Weight distribution of 3rd FC layer"
	];

figureNames = [
	"weight-distribution-conv1"
	"weight-distribution-conv2"
	"weight-distribution-conv3"
	"weight-distribution-conv4"
	"weight-distribution-conv5"
	"weight-distribution-FC1"
	"weight-distribution-FC2"
	"weight-distribution-FC3"
	];

bitWidth = 8;

for i=1:8
	figure("Name", figureNames(i));
	
	subplot(1, 2, 1);
	h = histogram(p{i * 2 - 1}, nBins);
	set(gca, "YScale", "log");
	lim = 1.1 * max(abs(h.BinEdges));
	xlim([-lim, lim]);
	
	lim = max(h.Values);
	ylim([0.1, lim * 10])
	
% 	title(titles(i));
	title("Single-precision floating-point");
	xlabel("Weight Value");
	ylabel("Count");
	
	
	subplot(1, 2, 2);
	x = QuantizeFixed(p{i * 2 - 1}, bitWidth);
	h = histogram(x, nBins);
	set(gca, "YScale", "log");
	lim = 1.1 * max(abs(h.BinEdges));
	xlim([-lim, lim]);
	
	lim = max(h.Values);
	ylim([0.1, lim * 10])
	
% 	title(titles(i));
	title("8-bit fixed-point");
	xlabel("Weight Value");
	ylabel("Count");
	
	saveas(gca, strcat('Data/figures/', figureNames(i), '.png'))
end
