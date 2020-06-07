function x = QuantizeFixed(x, bitWidth)
s = zeros(1, bitWidth);
for r=1:bitWidth
	s(r) = sum(...
		abs(...
		x - ...
		num2fixpt(x, fixdt(1, bitWidth, r), 'Nearest')...
		), 'all'...
		) / numel(x);
end

[m, rep] = min(s);

x = num2fixpt(x, fixdt(1, bitWidth, rep));
fprintf("Fixed %u.%u, accuracy: %f\n", bitWidth, rep, m);

end

