function PrintAddressNew(arr, layerName)
global SAVE_PLOTS USE_SUBPLOTS

if USE_SUBPLOTS
	subplot(1, 3, 3)
else
	figureName = strcat(layerName, '-output-creation-time');
	figure('Name', figureName)
end

d = max(arr(:, 2));

arr = arr(arr(:, 1)==0, 2:3) + 1;

im = zeros(d);
index = 0;
for i=1:length(arr)
	im(arr(i, 1), arr(i, 2)) = index;
	index = index + 1;
end
image(im, 'CDataMapping','scaled')
c = colorbar('Ticks', [0, index-1], 'TickLabels', {'Start', 'End'}, 'Direction','reverse');
ylabel(c, 'Time');
title('Output pixel creation time')
axis image;
colormap('jet');
xlabel('Columns');
ylabel('Rows');

if ~USE_SUBPLOTS && SAVE_PLOTS
	saveas(gcf, strcat('output/noSubplots/', figureName, '.png'));
end
end
