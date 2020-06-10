function x = QuantizeFixed(x, bitWidth)
x = double(x);
s = zeros(1, bitWidth);
for r=1:bitWidth
	xFixed = num2fixpt(x, fixdt(1, bitWidth, r), 'Nearest');
	xAbs = abs(x - xFixed);
	s(r) = sum(xAbs(:)) / numel(x);
end

[m, rep] = min(s);

x = num2fixpt(x, fixdt(1, bitWidth, rep));
fprintf("Fixed %u.%u, accuracy: %f\n", bitWidth, rep, m);
end
