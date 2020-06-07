function x = AlexNet(x, p)
x = Conv2d(x, p{1}, p{2}, 3, 64, 11, 4, 2, 1);
x = MaxPool(x, 3, 2);
x = Conv2d(x, p{3}, p{4}, 64, 192, 5, 1, 2, 1);
x = MaxPool(x, 3, 2);
x = Conv2d(x, p{5}, p{6}, 192, 384, 3, 1, 1, 1);
x = Conv2d(x, p{7}, p{8}, 384, 256, 3, 1, 1, 1);
x = Conv2d(x, p{9}, p{10}, 256, 256, 3, 1, 1, 1);
x = MaxPool(x, 3, 2);
x = permute(x, [2 1 3]);
x = reshape(x, [1, numel(x)]);
x = Linear(x, p{11}, p{12}, 1);
x = Linear(x, p{13}, p{14}, 1);
x = Linear(x, p{15}, p{16}, 0);
end

