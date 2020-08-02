function LayerTypeTimings(n)

totalClockCycles = 0;
totalConv = 0;
totalMaxPool = 0;
totalLinear = 0;
f = fieldnames(n);
for i=1:numel(f)
    fName = string(f{i});
    if regexp(fName, "c[0-9]") == 1
        totalConv = totalConv + n.(fName).tc;
        totalClockCycles = totalClockCycles + n.(fName).tc;
    elseif regexp(fName, "m[0-9]") == 1
        totalMaxPool = totalMaxPool+ n.(fName).tc;
        totalClockCycles = totalClockCycles + n.(fName).tc;
    elseif regexp(fName, "l[0-9]") == 1
        totalLinear = totalLinear + n.(fName).tc;
        totalClockCycles = totalClockCycles + n.(fName).tc;
    end 
end
fprintf("Total: %d, conv: %d, maxpool: %d, linear: %d\n", ...
    totalClockCycles, totalConv, totalMaxPool, totalLinear);
end

