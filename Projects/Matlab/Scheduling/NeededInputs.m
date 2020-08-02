function inputs = NeededInputs(x, c)
if regexp(c.name, "Linear[0-9]") == 1
    inputs = c.ic; % min([(c.k + (x - 1) * c.s) - c.p, c.h, c.w]);
else
    inputs = min([(c.k + (x - 1) * c.s) - c.p, c.h, c.w]);
end
end

