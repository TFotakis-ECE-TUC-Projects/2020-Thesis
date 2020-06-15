close all;
clear all;
clc;

global imgConvertFunc paramsConvertFunc activationsConvertFunc

compDTypeFile = "Data/results/fixed16MQE.txt";

imgConvertFunc = @(x) x;
% imgConvertFunc = @double;
% imgConvertFunc = @single;
% imgConvertFunc = @half;
% imgConvertFunc = @(x) num2fixpt(x, fixdt(0, 8, 6), 'Nearest');

% paramsConvertFunc = @(p) p;
% paramsConvertFunc = @(p) cellfun(@(x) double(x), p, 'UniformOutput', false);
% paramsConvertFunc = @(p) cellfun(@(x) single(x), p, 'UniformOutput', false);
% paramsConvertFunc = @(p) cellfun(@(x) half(x), p, 'UniformOutput', false);
paramsConvertFunc = @(p) ParamsQuantizeFixed(p, 16);
% paramsConvertFunc = @(p) PruneParams(p);

activationsConvertFunc = @(x) x;
% activationsConvertFunc = @double;
% activationsConvertFunc = @single;
% activationsConvertFunc = @half;
% activationsConvertFunc = @(x) QuantizeFixed(x, 8);

addpath("Functions");

%% ---------------------------------------
hWaitbar = waitbar(0, 'Loading Params', 'Name', ...
	'Compare Data types for network inferencing', 'CreateCancelBtn', ...
	'delete(gcbf)');
p = LoadParams("Data/alexnetParams");
p = paramsConvertFunc(p);

waitbar(1/5, hWaitbar, 'Loading Labels');
labels = LoadLabels("Data/alexnet.labels");

waitbar(2/5, hWaitbar, 'Loading baseline');
baseline = LoadResults("Data/results/baseline.txt");

waitbar(3/5, hWaitbar, 'Loading compare results');
compDType = LoadResults(compDTypeFile);

waitbar(4/5, hWaitbar, 'Loading images');
allImgs = dir("Data/images/cat/*.ppm");
allImgs = [allImgs; dir("Data/images/dog/*.ppm")];

imgs = [];
for i=1:numel(allImgs)
	if ~compDType.isKey(allImgs(i).name)
		imgs = [imgs allImgs(i)];
	end
end
waitbar(5/5, hWaitbar, 'Success!');

count = 0;
correct = 0;
t = 0;
f = fopen(compDTypeFile, "at");
for i=1:numel(imgs)
	waitbar(i/numel(imgs), hWaitbar, ['Image ' num2str(i) '/' num2str(numel(imgs))]);

	fprintf("%u/%u %s: ", i, numel(imgs), imgs(i).name);
	x = imread(char(strcat(imgs(i).folder, "/", imgs(i).name)));
	x = im2double(x);
	x = imgConvertFunc(x);

	tic
	x = AlexNet(x, p);
	t = t + toc;

	[~, x] = max(double(x));
	x = x - 1;
	eval = baseline(imgs(i).name) == x;
	correct = correct + eval * 1;
	if eval, fprintf("+ "), else, fprintf("x "), end
	fprintf("Class %u, Error Rate %f, Avg Time (sec) %f\n", x, ErrorRate(correct, i), t / i);
	fprintf(f, "%s: %u\n", imgs(i).name, x);

	drawnow;
	if ~ishandle(hWaitbar), break, end
end
fclose(f);
