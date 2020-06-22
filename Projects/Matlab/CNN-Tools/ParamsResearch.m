clear;
close all;
clc;

addpath("Functions");

p = LoadParams("Data/alexnetParams");
nBins = 100;

titles = [
	"1st Convolution layer"
	"2nd Convolution layer"
	"3rd Convolution layer"
	"4th Convolution layer"
	"5th Convolution layer"
	"1st FC layer"
	"2nd FC layer"
	"3rd FC layer"
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

%% ---------------------------------------------------------------
figure("Name", "All layer distributions");
for i=1:8
	subplot(4, 2, i);
	h = histogram(p{i * 2 - 1}, nBins);
	set(gca, "YScale", "log");
	lim = 1.1 * max(abs(h.BinEdges));
	xlim([-lim, lim]);
	
	lim = max(h.Values);
	ylim([0.1, lim * 10])
	
	title(titles(i));
	xlabel("Weight Value");
	ylabel("Count");
end

%% ---------------------------------------------------------------
bitWidth = 8;

% paramsConvertFunc = @(p) ParamsQuantizeFixed(p, bitWidth);
paramsConvertFunc = @(p) p;
% pTransformed = p;
pTransformed = PruneParams(p);
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
% 	title("Single-precision floating-point");
	title("Original");
	xlabel("Weight Value");
	ylabel("Count");
	
	
	subplot(1, 2, 2);
	x = paramsConvertFunc(pTransformed{i * 2 - 1});
	h = histogram(x, nBins);
	set(gca, "YScale", "log");
	lim = 1.1 * max(abs(h.BinEdges));
	xlim([-lim, lim]);
	
	lim = max(h.Values);
	ylim([0.1, lim * 10])
	
% 	title(titles(i));
% 	title("8-bit fixed-point");
	title("Pruned");
	xlabel("Weight Value");
	ylabel("Count");
	
	saveas(gca, strcat('Data/figures/weights-distributions/original-vs-pruned/', figureNames(i), '.png'))
end

%% ---------------------------------------------------------------
%{
subplot(2, 2, 1);
flat = FlattenParams(p(1:2:end));
h = histogram(flat, nBins);
set(gca, "YScale", "log");
lim = max(abs(h.BinEdges));
xlim([-lim, lim]);
title("all layers");
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
title(strcat("all layers, pruned with threshold: ", num2str(threshold)));
xlabel("Weight Value");
ylabel("Count");

subplot(2, 2, 3);
flatLinear = FlattenParams(p(11:2:end));
h = histogram(flatLinear, nBins);
set(gca, "YScale", "log");
lim = max(abs(h.BinEdges));
xlim([-lim, lim]);
title("all FC layers");
xlabel("Weight Value");
ylabel("Count");
%}
