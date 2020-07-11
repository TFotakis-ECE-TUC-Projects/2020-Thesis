close all;
clear all;
clc;

p = LoadParams("Data/alexnetParams");

[p, rep] = ParamsQuantizeFixed(p, 8);
for i=1:numel(p)
	pf{i} = p{i} ./ 2^double(-rep(i));
end

f = fopen("Data/alexnetParamsFixed8", "w");
fwrite(f, numel(pf), 'int8');
for i=1:numel(pf)
	data = pf{i};
	dimsNum = numel(size(data));
	if dimsNum == 4
		data = permute(data, [2, 1, 3, 4]);
	elseif dimsNum == 2 && size(data, 1) == 1
		dimsNum = 1;
	end
% 	pfw{i} = data;

	fwrite(f, dimsNum, 'int8');
	if dimsNum == 1
		fwrite(f, size(data, 2), 'int16');
	else
		dims = size(data);
		fwrite(f, dims(end:-1:1), 'int16');
	end
	fwrite(f, rep(i), 'int8');
	fwrite(f, data, 'int8');
end
fclose(f);
