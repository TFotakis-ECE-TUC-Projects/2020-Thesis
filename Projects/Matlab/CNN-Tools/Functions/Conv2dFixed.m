function [res, resRep, nm] = Conv2dFixed(x, xrep, weights, wrep, bias, brep, in_channels, out_channels, ...
	kernel_size, stride, padding, doReLU)

weights = weights ./ 2^double(-wrep);
weights = int32(weights);

bias = bias ./ 2^double(-brep);
bias = int32(bias);
bshift = xrep + wrep - brep;
bias = bitshift(abs(bias), bshift) .* int32(sign(bias));


hIn = size(x, 1);
wIn = size(x, 2);
hOut = floor((hIn + 2 * padding - kernel_size) / stride + 1);
wOut = floor((wIn + 2 * padding - kernel_size) / stride + 1);

res = zeros(hOut, wOut, out_channels);

arr = zeros(hIn + 2 * padding, wIn + 2 * padding, in_channels);
arr(padding + 1: padding + hIn, padding + 1: padding + wIn, :) = x;

% For every output channel
for out_channel=1:out_channels
	% For every output row
	for oh=0:hOut - 1
		imgStartH = oh * stride + 1;
		
		% For every output row's pixel
		for ow=0:wOut - 1
			% Calculate starting coordinates on the padded matrix
			imgStartW = ow * stride + 1;
			
			xa = arr(imgStartH: imgStartH + kernel_size - 1, ...
				imgStartW: imgStartW + kernel_size - 1, :);
			xa = double(xa);
			
			wa = weights(:, :, :, out_channel);
			wa = double(wa);
			
			p = xa .* wa;
			pixel = sum(p(:)) + bias(out_channel);
			
			if doReLU && pixel > 0 || ~doReLU
				res(oh + 1, ow + 1, out_channel) = pixel;
			end
		end
	end
end

% res = activationsConvertFunc(res);
m = max(abs(res(:)));
nm = numel(dec2bin(m));
resShift = nm - 7;
resRep = xrep + wrep - resShift;
res = bitshift(abs(int32(res)), -resShift) .* int32(sign(res)); 

end
