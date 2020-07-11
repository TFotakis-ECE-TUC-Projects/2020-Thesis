function [res, resRep, nm] = Linear(x, xrep, weights, wrep, bias, brep, doReLU)

x = double(x);

weights = weights ./ 2^double(-wrep);
% weights = int32(weights);

bias = bias ./ 2^double(-brep);
bias = int32(bias);
bshift = xrep + wrep - brep;
bias = bitshift(abs(bias), bshift) .* int32(sign(bias));
bias = double(bias);

res = x * weights + bias;
res = int32(res);

if doReLU
	res = int32(res > 0) .* res;
end

m = max(abs(res(:)));
nm = numel(dec2bin(m));
resShift = nm - 7;
resRep = xrep + wrep - resShift;
res = bitshift(abs(int32(res)), -resShift) .* int32(sign(res)); 

end

