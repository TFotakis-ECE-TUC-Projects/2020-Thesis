function labels = LoadLabels(path)
f = fopen(path);

n = fgetl(f);
n = str2num(n);
labels(n) = "";
for i=1:n
	labels(i) = fgetl(f);
end

fclose(f);
end

