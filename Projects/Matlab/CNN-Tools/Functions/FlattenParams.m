function flat = FlattenParams(p)
flat = [];
for i=1:numel(p)
	flat = [flat, reshape(p{i}, [1, numel(p{i})])];
end
end

