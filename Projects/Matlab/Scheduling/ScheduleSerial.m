function n = ScheduleSerial(n)
n.arr(1).st = 0;
n.arr(1).en = n.c1.tc;
for i = 2:11
	n.arr(i).st = n.arr(i-1).en;
	n.arr(i).en = n.arr(i).st + n.arr(i).tc;
end
end
