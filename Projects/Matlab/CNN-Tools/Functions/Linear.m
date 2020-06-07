function res = Linear(x, weights, bias, doReLU)
global activationsConvertFunc;

res = x * weights + bias;

if doReLU
	res = double(res > 0) .* res;

res = activationsConvertFunc(res);
end
end

