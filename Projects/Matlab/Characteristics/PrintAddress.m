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
