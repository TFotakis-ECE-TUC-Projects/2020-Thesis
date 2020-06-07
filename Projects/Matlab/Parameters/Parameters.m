clear;
close all;
clc;

p = ReadParams("../CNN-Tools/Data/alexnetParams");

figureName = 'All histogram';
figure('Name', figureName);
flat = FlattenParams(p);
h = histogram(flat);
set(gca, 'YScale', 'log');
xlim(h.BinLimits);
title(figureName);

figureName = 'Fully connected histogram';
figure('Name', figureName);
flatLinear = FlattenParams(p(11:16));
h = histogram(flatLinear);
set(gca, 'YScale', 'log');
xlim(h.BinLimits);
title(figureName);

threshold = 0.5;
figureName = strcat('All but zeroed with threshold ', threshold, ' histogram');
figure('Name', figureName);
flatZeroed = flat;
flatZeroed(flatZeroed > -0.5 & flatZeroed < 0.5) = [];
h = histogram(flatZeroed);
set(gca, 'YScale', 'log');
xlim(h.BinLimits);
title(figureName);
