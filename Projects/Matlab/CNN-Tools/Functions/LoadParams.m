function params = LoadParams(filepath)

f = fopen(filepath, 'r');

% Get the number of parameter sets existing in this file
paramsLen = fread(f, 1, 'int');

% For every parameters set */
for p=1:paramsLen
	% Get the number of dimensions this parameters set consists of */
	dimsNum = fread(f, 1, 'int');
	
	dims = fread(f, dimsNum, 'int');
	
	data = fread(f, prod(dims), 'single');
	
	if dimsNum == 4
		data = reshape(data, dims(end:-1:1)');
		data = permute(data, [2, 1, 3, 4]);
	elseif dimsNum == 1
		data = data';
	elseif dimsNum == 2
		data = reshape(data, dims(end:-1:1)');
	end
	
	data = single(data);
	params{p} = data;
end

fclose(f);
end
