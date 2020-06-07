close all;
clear all;
clc;

global imgConvertFunc paramsConvertFunc activationsConvertFunc

compDTypeFile = "Data/fixed8.txt";

imgConvertFunc = @(x) x;
% imgConvertFunc = double;
% imgConvertFunc = single;
% imgConvertFunc = half;
% imgConvertFunc = @(x) num2fixpt(im2doubble(x), fixdt(0, 8, 6), 'Nearest');

paramsConvertFunc = @(p) p;
% paramsConvertFunc = @(p) cellfun(@(x) single(x), p, 'UniformOutput', false);
% paramsConvertFunc = @(p) cellfun(@(x) half(x), p, 'UniformOutput', false);
% paramsConvertFunc = @(p) ParamsQuantizeFixed(p, 8);

activationsConvertFunc = @(x) x;
% activationsConvertFunc = double;
% activationsConvertFunc = single;
% activationsConvertFunc = half;
% activationsConvertFunc = @(x) QuantizeFixed(x, 8);

%% ---------------------------------------
p = LoadParams("Data/alexnetParams");
p = paramsConvertFunc(p);

labels = LoadLabels("Data/alexnet.labels");
baseline = LoadResults("Data/baseline.txt");
compDType = LoadResults(compDTypeFile);


allImgs = dir("Data/images/cat/*.ppm");
allImgs = [allImgs; dir("Data/images/dog/*.ppm")];

imgs = [];
for i=1:numel(allImgs)
	if ~compDType.isKey(allImgs(i).name)
		imgs = [imgs allImgs(i)];
	end
end

count = 0;
correct = 0;
t = 0;
f = fopen(compDTypeFile, "at");
for i=1:numel(imgs)
	fprintf("%u/%u %s: ", i, numel(imgs), imgs(i).name);
	x = imread(strcat(imgs(i).folder, "\", imgs(i).name));
	x = im2double(x);
	x = imgConvertFunc(x);
	
	tic
	x = AlexNet(x, p);
	t = t + toc;
	
	[~, x] = max(x);
	x = x - 1;
	eval = baseline(imgs(i).name) == x;
	correct = correct + eval * 1;
	if eval, fprintf("+ "), else, fprintf("x "), end
	fprintf("Class %u, Error Rate %f, Avg Time (sec) %f\n", x, ErrorRate(correct, i), t / i);
	fprintf(f, "%s: %u\n", imgs(i).name, x - 1);
end
fclose(f);
