function p = PruneParams(p)
% factors = [0.2, 0, 0.1, 0, 0.1, 0, 0.05, 0, 0.05, 0, 0.018, 0, 0.02, 0, 0.03, 0]; % 93.11%
% factors = [0.2, 0, 0.1, 0, 0.1, 0, 0.05, 0, 0.05, 0, 0.005, 0, 0.005, 0, 0.005, 0]; % 41.22%
% factors = [0.01, 0, 0.01, 0, 0.01, 0, 0.01, 0, 0.01, 0, 0.005, 0, 0.005, 0, 0.005, 0]; % 38.53%
factors = [0.005, 0, 0.005, 0, 0.005, 0, 0.005, 0, 0.005, 0, 0.005, 0, 0.005, 0, 0.005, 0]; % 37.96%
% factors = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.005, 0, 0.005, 0, 0.005, 0]; % 37.37%
% factors = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.01, 0, 0.01, 0, 0.01, 0]; % 64.78%
% factors = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.02, 0, 0.02, 0, 0.02, 0]; % 89.63%

totalParams = 0;
totalPruned = 0;

% for i=1:numel(p)
for i=1:2:numel(p)
	xa = -factors(i) <= p{i} & p{i} <= factors(i);
	perc = sum(xa(:))/numel(xa) * 100;
	p{i}(xa) = 0;
	
	totalParams = totalParams + numel(p{i});
	totalPruned = totalPruned + sum(xa(:));	
	
% 	fprintf("Param %u: %0.2f%%\n", i, perc);
	fprintf("%0.2f\n", perc);
end
perc = round(totalPruned/totalParams*100, 2);
% fprintf("Params pruned: %0.2f%%\n", perc);

fprintf("%0.2f\n", perc);
end
