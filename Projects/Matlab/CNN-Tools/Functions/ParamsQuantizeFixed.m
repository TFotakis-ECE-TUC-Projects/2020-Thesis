function [params, rep] = ParamsQuantizeFixed(params, bitWidth)
rep = zeros(1, numel(params));
for p=1:numel(params)
% 	fprintf("Params %u: ", p);
	[params{p}, rep(p)] = QuantizeFixed(params{p}, bitWidth);
end
end
