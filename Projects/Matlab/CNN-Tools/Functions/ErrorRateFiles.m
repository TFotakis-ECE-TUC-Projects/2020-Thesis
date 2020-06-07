function errorRate = ErrorRateFiles(baselinePath, compareToPath)
baseline = LoadResults(baselinePath);
compToDType = LoadResults(compareToPath);

correct = 0;
count = 0;
for k=baseline.keys()
	if compToDType.isKey(k)
		correct = correct + (baseline(k{1}) == compToDType(k{1}));
		count = count + 1;
	end
end
errorRate = ErrorRate(correct, count);
end
