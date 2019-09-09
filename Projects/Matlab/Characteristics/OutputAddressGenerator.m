function res = OutputAddressGenerator(d, oc, order)
	% Order: 'ImageFirst' or 'FilterFirst'
	res = zeros(d * d * oc, 3);

	if strcmp(order, 'ImageFirst')
		index = 1;
		for k=0:oc-1
			for i=0:d-1
				for j=0:i
					res(index, :) = [k, i, j];
					index = index + 1;
				end
				for j=j - 1:-1:0
					res(index, :) = [k, j, i];
					index = index + 1;
				end
			end
		end
	elseif strcmp(order, 'FilterFirst')
		index = 1;
		for i=0:d-1
			for j=0:i
				for k=0:oc-1
					res(index, :) = [k, i, j];
					index = index + 1;
				end
			end
			for j=j - 1:-1:0
				for k=0:oc-1
					res(index, :) = [k, j, i];
					index = index + 1;
				end
			end
		end
	end
end
