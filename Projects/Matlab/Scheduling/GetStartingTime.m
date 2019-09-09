function t = GetStartingTime(arr, index)
if index == 1
	t = 0;
	return;
end

[~, layer] = max([arr(1:index-1).tc]);

y = @(x, c) min([(c.k + (x - 1) * c.s) - c.p, c.h, c.w]);
inputs = 1;
for i = index:-1:layer+1
	inputs = y(inputs, arr(i));
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
