function LayerTypeTimings(n)

totalClockCycles = 0;

totalConv = 0;
f = fieldNames(n);
for i=1:numel(f)
	fName = string(f(i));
	s = regexp(fName, "c[0-9]");
	if regexp(fName, "c[0-9]") == 1
		totalConv = totalConv + n.(fName).tc;
		totalClockCycles = totalClockCycles + n.(fName).tc;
	else if regexp(fName, "c[0-9]") == 1
		totalConv = totalConv + n.(fName).tc;
		totalClockCycles = totalClockCycles + n.(fName).tc;
			
	else if regexp(fName, "c[0-9]") == 1
		totalConv = totalConv + n.(fName).tc;
		totalClockCycles = totalClockCycles + n.(fName).tc;
				
	end
end

end

