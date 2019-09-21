function PrintMemoryLocality(arr, din, k, s, p, ic, layerName)
global SAVE_PLOTS USE_SUBPLOTS
arr = arr(arr(:, 1)==0, 2:3);

im = zeros(din);
for i=1:length(arr)
	inputs = InputAddressGenerator(arr(i, :), din, k, s, p, ic);
	inputs = inputs(inputs(:, 1)==0, 2:3) + 1;
	
	for j=1:length(inputs)
		im(inputs(j, 1), inputs(j, 2)) = im(inputs(j, 1), inputs(j, 2)) + 1;
	end
end

if USE_SUBPLOTS
	subplot(2, 2, 1)
else
	figureName = strcat(layerName, '-pixel-frequency');
	figure('Name', figureName)
end
image(im, 'CDataMapping','scaled')
c = colorbar('Ticks', unique(im));
ylabel(c, 'Times used');
title("Pixel usage frequency")
axis image;
colormap('jet');
xlabel('Columns');
ylabel('Rows');
if ~USE_SUBPLOTS && SAVE_PLOTS
	saveas(gcf, strcat('output/noSubplots/AlexNet-Characteristics-', figureName, '.png'));
end

if USE_SUBPLOTS
	subplot(2, 2, 2)
else
	figureName = strcat(layerName, '-pixel-histogram');
	figure('Name', figureName)
end
histogram(im)
set(gca, 'xtick', unique(im));
title("Pixel usage histogram")
xlabel('Times used');
ylabel('Pixels Count');
if ~USE_SUBPLOTS && SAVE_PLOTS
	saveas(gcf, strcat('output/noSubplots/AlexNet-Characteristics-', figureName, '.png'));
end
end
