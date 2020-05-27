function ret = TConv2DReLU(h, w, ic, oc, k, s, p, parallel, name)
% h: Input height
% w: Input width
% ic: Input channels
% oc: Output channels
% k: Kernel size
% s: Stride
% p: Padding
% ipo: Input per output
% cpi: Cycles per input
% cpo: Cycles per output
% tc: Total cycles needed
% to: Total output

global c;

ipo = ic * k * k;
cpi = (1 - c.useTimings) + c.useTimings * (3 * c.memRead + 1 * c.mul + ...
	1 * c.add + 1 * c.branch + 2 * c.memWrite);

% oh: Output height
oh = (h + 2 * p - k) / s + 1;
oh = fix(oh);

% ow: Output width
ow = (w + 2 * p - k) / s + 1;
ow = fix(ow);

to = oc * oh * ow;

cpo = ipo * cpi / parallel;
tc = to * cpo;


ret.h = h;
ret.w = w;
ret.ic = ic;
ret.oc = oc;
ret.k = k;
ret.s = s;
ret.p = p;
ret.cpi = cpi;
ret.ipo = ipo;
ret.cpo = cpo;
ret.tc = tc;
ret.to = to;
ret.st = 0;
ret.en = 0;
ret.name = name;

ret.totalWeights = oc * ic * k * k;
ret.totalMemory = ret.totalWeights * c.dataTypeSize;
ret.totalOutputMemory = to * c.dataTypeSize;
end