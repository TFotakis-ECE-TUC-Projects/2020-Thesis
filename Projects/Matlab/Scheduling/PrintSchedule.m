function PrintSchedule(arr, figureName, useTitles)
global chip_frequency logScale printTimings USE_SUBPLOTS SAVE_PLOTS
%cla;
lineWidth = 15;

a = [[arr(:).st]; [arr(:).en]];
b = [length(arr):-1:1; length(arr):-1:1];
s = [arr(:).st];
plotFunc = @plot;
if logScale
	a = a + (a == 0);
	plotFunc = @semilogx;
	s = s + (s == 0);
end
plotFunc(a, b, 'LineWidth', lineWidth);
text(s, length(arr):-1:1, {arr(:).name})

if useTitles
	title(figureName);
end
xlabel('Clock cycles');
set(gca, 'ytick', []);
ylim([0 12]);

if logScale
	set(gca,'XTick', 10.^(0:9));
	xlim([0 1e9]);
end

if printTimings
	fprintf("%s: %.2f ms\n", figureName, (arr(end).en / chip_frequency) * 1e3)
end

if ~USE_SUBPLOTS && SAVE_PLOTS
	figurePath = strcat('output/noSubplots/', regexprep(figureName, ' +', '-'), '.png');
	saveas(gca, figurePath);
end
end
