function n = SchedulePipelined(n)
for i=1:8
	n.arr(i).st = GetStartingTime(n.arr, i);
	n.arr(i).en = GetEndingTime(n.arr, i);
end

n.arr(9).st = GetStartingTime(n.arr, 8, n.arr(8).k);
n.arr(9).en = n.arr(8).en + n.arr(9).oc;

n.arr(10).st = n.arr(8).en + n.arr(9).cpi;
n.arr(10).en = n.arr(10).st + n.arr(10).tc;

n.arr(11).st = n.arr(10).en - ((n.arr(10).oc - 1) * n.arr(10).cpo);
n.arr(11).en = n.arr(10).en + n.arr(11).cpo;

% n.arr(9).st = n.arr(8).en;
% n.arr(9).en = n.arr(9).st + n.arr(9).tc;
% 
% n.arr(10).st = n.arr(9).en;
% n.arr(10).en = n.arr(10).st + n.arr(10).tc;
% 
% n.arr(11).st = n.arr(10).en;
% n.arr(11).en = n.arr(11).st + n.arr(11).tc;

end
