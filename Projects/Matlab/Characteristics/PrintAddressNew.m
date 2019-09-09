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
	% colorbar('Ticks', [0, index-1], 'TickLabels', {'Start', 'End'}, 'Direction','reverse')
	colorbar
	title('Time when outputs are created')
end
