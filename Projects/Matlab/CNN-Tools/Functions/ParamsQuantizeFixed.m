function params = ParamsQuantizeFixed(params, bitWidth)
for p=1:numel(params)
	fprintf("Params %u: ", p);
	params{p} = QuantizeFixed(params{p}, bitWidth);
end
end
