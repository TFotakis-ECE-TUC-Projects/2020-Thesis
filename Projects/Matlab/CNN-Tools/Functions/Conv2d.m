function res = Conv2d(x, weights, bias, in_channels, out_channels, ...
	kernel_size, stride, padding, doReLU)
global activationsConvertFunc;

hIn = size(x, 1);
wIn = size(x, 2);
hOut = floor((hIn + 2 * padding - kernel_size) / stride + 1);
wOut = floor((wIn + 2 * padding - kernel_size) / stride + 1);

res = zeros(hOut, wOut, out_channels);
res = activationsConvertFunc(res);

arr = zeros(hIn + 2 * padding, wIn + 2 * padding, in_channels);
arr(padding + 1: padding + hIn, padding + 1: padding + wIn, :) = x;
arr = activationsConvertFunc(arr);

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
			pixel = activationsConvertFunc(sum(p(:))) + bias(out_channel);
			
			if doReLU && pixel > activationsConvertFunc(0) || ~doReLU
				res(oh + 1, ow + 1, out_channel) = pixel;
			end
		end
	end
end

% res = activationsConvertFunc(res);

end
