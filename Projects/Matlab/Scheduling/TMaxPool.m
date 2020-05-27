function ret = TMaxPool(ic, h, w, k, s, parallel, name)
% ic: Input channels
% h: Input height
% w: Input width
% k: Kernel size
% s: Stride
% ipo: Input per output
% cpi: Cycles per input
% cpo: Cycles per output
% tc: Total cycles needed
% to: Total output
global c;

ipo = k * k;

% oh: Output height
oh = (h - k) / s + 1;
oh = ceil(oh);

% ow: Output width
ow = (w - k) / s + 1;
ow = ceil(ow);

cpi = (1 - c.useTimings) + c.useTimings * (2 * c.memRead + ...
	1 * c.branch * 1 * c.memWrite);

cpo = ipo * cpi / parallel;
to = ic * oh * ow;
tc = to * cpo;

ret.ic = ic;
ret.h = h;
ret.w = w;
ret.k = k;
ret.s = s;
ret.p = 0;
ret.oc = ic;
ret.ipo = ipo;
ret.cpi = cpi;
ret.cpo = cpo;
ret.tc = tc;
ret.to = to;
ret.st = 0;
ret.en = 0;
ret.name = name;

ret.totalWeights = 0;
ret.totalMemory = ret.totalWeights * c.dataTypeSize;
ret.totalOutputMemory = to * c.dataTypeSize;
end