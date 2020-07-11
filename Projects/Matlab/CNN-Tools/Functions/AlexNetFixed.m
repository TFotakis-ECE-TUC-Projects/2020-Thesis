function [x, xrep, xr, xb] = AlexNetFixed(x, p, rep, xr, xb)
[x, xrep] = QuantizeFixed(x, 8);
x = x ./ 2^double(-xrep);
x = int32(x);
xr(1) = max(xrep, xr(1));
nm = 8;
xb(1) = max(nm, xb(1));

[x, xrep, nm] =  Conv2dFixed(x, xrep, p{1}, rep(1), p{2}, rep(2), 3, 64, 11, 4, 2, 1);
xr(2) = max(xrep, xr(2));
xb(2) = max(nm, xb(2));

x = MaxPool(x, 3, 2);
[x, xrep, nm] =  Conv2dFixed(x, xrep, p{3}, rep(3), p{4}, rep(4), 64, 192, 5, 1, 2, 1);
xr(3) = max(xrep, xr(3));
xb(3) = max(nm, xb(3));

x = MaxPool(x, 3, 2);
[x, xrep, nm] =  Conv2dFixed(x, xrep, p{5}, rep(5), p{6}, rep(6), 192, 384, 3, 1, 1, 1);
xr(4) = max(xrep, xr(4));
xb(4) = max(nm, xb(4));

[x, xrep, nm] =  Conv2dFixed(x, xrep, p{7}, rep(7), p{8}, rep(8), 384, 256, 3, 1, 1, 1);
xr(5) = max(xrep, xr(5));
xb(5) = max(nm, xb(5));

[x, xrep, nm] =  Conv2dFixed(x, xrep, p{9}, rep(9), p{10}, rep(10), 256, 256, 3, 1, 1, 1);
xr(6) = max(xrep, xr(6));
xb(6) = max(nm, xb(6));

x = MaxPool(x, 3, 2);

x = double(x);
x = permute(x, [2 1 3]);
x = reshape(x, [1, numel(x)]);

[x, xrep, nm] =  LinearFixed(x, xrep, p{11}, rep(11), p{12}, rep(12), 1);
xr(7) = max(xrep, xr(7));
xb(7) = max(nm, xb(7));

[x, xrep, nm] =  LinearFixed(x, xrep, p{13}, rep(13), p{14}, rep(14), 1);
xr(8) = max(xrep, xr(8));
xb(8) = max(nm, xb(8));

[x, xrep, nm] =  LinearFixed(x, xrep, p{15}, rep(15), p{16}, rep(16), 0);
xr(9) = max(xrep, xr(9));
xb(9) = max(nm, xb(9));
end

