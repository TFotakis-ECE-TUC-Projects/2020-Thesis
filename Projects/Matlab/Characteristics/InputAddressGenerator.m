function res = InputAddressGenerator(outAddr, d, k, s, p, ic)
	rs = outAddr(1) * s - p;
	rf = rs + k;
	rf = rf * (rf <= d) + d * (rf > d);
	rs = rs * (rs >= 0);

	cs = outAddr(2) * s - p;
	cf = cs + k;
	cf = cf * (cf <= d) + d * (cf > d);
	cs = cs * (cs >= 0);

	res = zeros((rf - rs) * (cf - cs) * ic, 3);
	index = 1;
	for k=0:ic - 1
		for i=rs:rf - 1
			for j=cs:cf - 1
				res(index, :) = [k, i, j];
				index = index + 1;
			end
		end
	end
end
