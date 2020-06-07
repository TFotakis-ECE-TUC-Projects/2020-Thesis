function p = ReadParams(path)
f = fopen(path, 'r');

paramsNum = fscanf(f, "%d", 1);

p = struct('d', cell(1, paramsNum), 'w', cell(1, paramsNum)); 
for i=1:paramsNum
	dimsNum = fscanf(f, "%d", 1);
	p(i).d = fscanf(f, "%d", dimsNum);
	rows = p(i).d(1);
	cols = prod(p(i).d(2:end));
	p(i).w = fscanf(f, "%f", [rows, cols]);
end

fclose(f);
end

