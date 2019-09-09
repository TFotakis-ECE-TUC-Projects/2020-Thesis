function PrintMemoryLocality(arr, din, k, s, p, ic)
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
	% colorbar('Ticks', unique(im));
	colorbar;
	title("Input image's pixels usage frequency")

	subplot(2, 2, 2)
	% histogram(im)
	hist(im)
	set(gca, 'xtick', unique(im));
	title("Input image's pixels usage histogram")
end
