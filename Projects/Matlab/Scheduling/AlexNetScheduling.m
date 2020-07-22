clear;
close all;
clc;

%% Definitions
% c: cycles per operation
global c p chip_frequency logScale printTimings USE_SUBPLOTS USE_TITLES SAVE_PLOTS
c.memRead = 1;
c.mul = 1;
c.add = 1;
c.branch = 1;
c.memWrite = 1;
c.useTimings = 0;
c.dataTypeSize = 4;
chip_frequency = 100e6;
logScale = false;
printTimings = false;
USE_SUBPLOTS = false;
USE_TITLES = false;
SAVE_PLOTS = true;
fprintf("Chip Frequency: %d MHz\n\n", chip_frequency / 1e6);


%% Network
p.conv1 = 1;
p.conv2 = 1;
p.conv3 = 1;
p.conv4 = 1;
p.conv5 = 1;
p.maxpool1 = 1;
p.maxpool2 = 1;
p.maxpool3 = 1;
p.linear1 = 1;
p.linear2 = 1;
p.linear3 = 1;

figureName = 'Serial';
if USE_SUBPLOTS
	subplot(3,2,1);
else
	figure('Name', figureName);
end
n1 = CreateNetwork();
n1 = ScheduleSerial(n1);
PrintSchedule(n1.arr, figureName, USE_TITLES);
LayerTypeTimings(n1);

figureName = 'Pipelined 1x';
if USE_SUBPLOTS
	subplot(3,2,2);
else
	figure('Name', figureName);
end
n2 = CreateNetwork();
n2 = SchedulePipelined(n2);
PrintSchedule(n2.arr, figureName, USE_TITLES);

p.conv1 = 11*11;
p.conv2 = 5*5;
p.conv3 = 3*3;
p.conv4 = 3*3;
p.conv5 = 3*3;
p.maxpool1 = 3*3;
p.maxpool2 = 3*3;
p.maxpool3 = 3*3;
p.linear1 = 1024;
p.linear2 = 1024;
p.linear3 = 1024;

figureName = 'Serial Kernels Parallelism';
if USE_SUBPLOTS
	subplot(3,2,3);
else
	figure('Name', figureName);
end
n3 = CreateNetwork();
n3 = ScheduleSerial(n3);
PrintSchedule(n3.arr, figureName, USE_TITLES);

figureName = 'Pipelined Kernels Parallelism';
if USE_SUBPLOTS
	subplot(3,2,4);
else
	figure('Name', figureName);
end
n4 = CreateNetwork();
n4 = SchedulePipelined(n4);
PrintSchedule(n4.arr, figureName, USE_TITLES);

p.conv1 = 11*11*3;
p.conv2 = 5*5*64;
p.conv3 = 3*3*192;
p.conv4 = 3*3*384;
p.conv5 = 3*3*256;
p.maxpool1 = 3*3;
p.maxpool2 = 3*3;
p.maxpool3 = 3*3;
p.linear1 = 1024;
p.linear2 = 1024;
p.linear3 = 1024;

figureName = 'Serial K x OC Parallelism';
if USE_SUBPLOTS
	subplot(3,2,5);
else
	figure('Name', figureName);
end
n5 = CreateNetwork();
n5 = ScheduleSerial(n5);
PrintSchedule(n5.arr, figureName, USE_TITLES);

figureName = 'Pipelined K x OC Parallelism';
if USE_SUBPLOTS
	subplot(3,2,6);
else
	figure('Name', figureName);
end
n6 = CreateNetwork();
n6 = SchedulePipelined(n6);
PrintSchedule(n6.arr, figureName, USE_TITLES);

if USE_SUBPLOTS && SAVE_PLOTS
	saveas(gca, 'output/withSubplots/Scheduling.png');
end