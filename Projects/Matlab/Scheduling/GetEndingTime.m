function t = GetEndingTime(arr, index)
if index == 1
	t = arr(index).tc;
elseif arr(index - 1).en - arr(index - 1).st > arr(index).tc
	t = arr(index - 1).en + arr(index).cpo;
else
	t = arr(index).st + arr(index).tc;
end
end