function ret = TLinear(ic, oc, useReLU, parallel, name)
% ic: Input channels
% oc: Output channels
% ipo: Input per output
% cpi: Cycles per input
% cpo: Cycles per output
% tc: Total cycles needed

global c;

cpi = (1 - c.useTimings) + c.useTimings * (3 * c.memRead + 1 * c.mul + ...
	1 * c.add + 1 * useReLU * c.branch + 1 * c.memWrite);
ipo = ic;
cpo = ipo * cpi / parallel;
tc = oc * cpo;

ret.ic = ic;
ret.oc = oc;
ret.ipo = ipo;
ret.cpi = cpi;
ret.cpo = cpo;
ret.tc = tc;
ret.st = 0;
ret.en = 0;

ret.h = 0;
ret.w = 0;
ret.k = 0;
ret.s = 0;
ret.p = 0;
ret.to = oc;
ret.name = name;
end
