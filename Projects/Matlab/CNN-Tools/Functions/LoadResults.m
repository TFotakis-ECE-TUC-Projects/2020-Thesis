function results = LoadResults(path)
f = fopen(path, 'r');

results = containers.Map;
if f ~= -1
	while 1
		t = fgetl(f);
		if ~ischar(t) || isempty(t), break, end
		t = split(t, ': ');
		k = t{1};
		v = str2num(t{2});
		results(k) = v;
	end
	
	fclose(f);
	
	if isempty(t)
		f = fopen(path, 'w');
		fprintf(f, "");
		fclose(f);
	end
end
end

