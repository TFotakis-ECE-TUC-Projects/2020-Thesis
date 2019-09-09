clear;
% close all;
clc;

%% Definitions
% c: cycles per operation
global c p chip_frequency logScale printTimings
c.memRead = 1;
c.mul = 1;
c.add = 1;
c.branch = 1;
c.memWrite = 1;
c.useTimings = 0;
chip_frequency = 100e6;
logScale = 0;
printTimings = 1;
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

subplot(3,2,1)
n1 = CreateNetwork();
n1 = ScheduleSerial(n1);
PrintSchedule(n1.arr, 'Serial');

subplot(3,2,2)
n2 = CreateNetwork();
n2 = SchedulePipelined(n2);
PrintSchedule(n2.arr, 'Pipelined 1x');

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

subplot(3,2,3)
n3 = CreateNetwork();
n3 = ScheduleSerial(n3);
PrintSchedule(n3.arr, 'Serial Kernels Parallelism');

subplot(3,2,4)
n4 = CreateNetwork();
n4 = SchedulePipelined(n4);
PrintSchedule(n4.arr, 'Pipelined Kernels Parallelism');

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

subplot(3,2,5)
n5 = CreateNetwork();
n5 = ScheduleSerial(n5);
PrintSchedule(n5.arr, 'Serial K*OC Parallelism');

subplot(3,2,6)
n6 = CreateNetwork();
n6 = SchedulePipelined(n6);
PrintSchedule(n6.arr, 'Pipelined K*OC Parallelism');