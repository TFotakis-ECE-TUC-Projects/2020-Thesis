function flat = FlattenParams(p)
flat = [];
for i=1:length(p)
	flat = [flat p(i).w(:)'];
end
end

