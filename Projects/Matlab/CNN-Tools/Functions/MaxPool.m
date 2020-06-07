function res = MaxPool(x, kernel_size, stride)
cIn = size(x, 3);
hIn = size(x, 1);
wIn = size(x, 2);
cOut = cIn;
hOut = floor((hIn - kernel_size) / stride + 1);
wOut = floor((wIn - kernel_size) / stride + 1);

res = zeros(hOut, wOut, cOut);

% For every output channel
% for out_channel=1:cOut
	% For every output row
	for oh=0:hOut - 1
		hs = 1 + oh * stride;
		% For every output row's pixel
		for ow=0:wOut - 1
			ws = 1 + ow * stride;
			
			xa = x(hs:hs+kernel_size - 1, ws:ws + kernel_size - 1, :);
			
			res(oh + 1, ow + 1, :) = max(max(xa));
		end
	end
% end
