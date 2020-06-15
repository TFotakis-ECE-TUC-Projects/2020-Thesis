function x = QuantizeFixed(x, bitWidth)
x = double(x);

% s = zeros(1, bitWidth);
% for r=1:bitWidth
% % for r=1:32
% 	xFixed = num2fixpt(x, fixdt(1, bitWidth, r), 'Nearest');
% 	xAbs = abs(x - xFixed) .^ 1;
% 	s(r) = sum(xAbs(:)) / numel(x);
% end
% [m, rep] = min(s);

rep = -fixptbestexp(x(:), bitWidth, 1);
rep = int32(rep);
m=0;

x = num2fixpt(x, fixdt(1, bitWidth, rep));
fprintf("Fixed %u.%u, Error rate: %f\n", bitWidth, rep, m);
end
