function t = GetStartingTime(arr, index, inputs)
if index == 1
	t = 0;
	return;
end

[~, layer] = max([arr(1:index-1).tc]);

y = @(x, c) min([(c.k + (x - 1) * c.s) - c.p, c.h, c.w]);
if ~exist('inputs', 'var')
    inputs = 1;
end
for i = index:-1:layer+1
	inputs = y(inputs, arr(i));
%     inputs = NeededInputs(inputs, arr(i));
end

if strfind([arr(layer+1:index).name], 'Conv')
	oc = arr(layer).oc;
else
	oc = 1;
end

t = inputs^2 * oc * arr(layer).cpo + arr(layer).st;
for i = index-1:-1:layer+1
	t = t + arr(i).cpo;
end
end
