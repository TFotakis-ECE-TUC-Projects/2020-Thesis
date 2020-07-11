function [params, rep] = LoadParamsFixed(filepath)

f = fopen(filepath, 'r');

% Get the number of parameter sets existing in this file
paramsLen = fread(f, 1, 'int8');
rep = zeros(paramsLen, 1);
% For every parameters set */
for i=1:paramsLen
	% Get the number of dimensions this parameters set consists of */
	dimsNum = fread(f, 1, 'int8');
	
	dims = fread(f, dimsNum, 'int16');
	
	rep(i) = fread(f, 1, 'int8');
	
	data = fread(f, prod(dims), 'int8');
	
	if dimsNum == 4
		data = reshape(data, dims(end:-1:1)');
		data = permute(data, [2, 1, 3, 4]);
	elseif dimsNum == 1
		data = data';
	elseif dimsNum == 2
		data = reshape(data, dims(end:-1:1)');
	end
	
	data = int8(data);
	params{i} = data;
end

fclose(f);
end
