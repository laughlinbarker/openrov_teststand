%Code for OpenROV thrust test stand. Reads in CSVs, averages, plots, and
%saves data to single xls as well as matlab file in single data structure.
% 
% Laughlin Barker - OpenROV - May 2014
%

clear; close all;

%%%%%%%%%%%%%%%
nsamples = 25; %calculate dynamically later
%%%%%%%%%%%%%%%

%move to directory with CSVs
directory = 'C:\Users\cuervo\Documents\OpenROV\test_stand\5-29_30';
cd(directory);

files = dir('*.CSV');
nfiles = size(dir('*.CSV'),1);

filename_out = 'compiledData.xls';

%loop through each file
for f = 1:length(files)

[path,name,ext,ver] = fileparts(files(f).name);
%extract metadata
sprintf('Current file: %s',files(f).name);
[num,alpha,raw] = xlsread(files(f).name);
metadata = alpha(1:9,1); %contains all header info
esc_model = metadata{1};
esc_model = esc_model(6:end);
prop_model = metadata{2};
prop_model = prop_model(7:end);
motor_model = metadata{3};
motor_model = motor_model(8:end);
date_time = metadata{4};
esc_timing = metadata{5};
direction = metadata{6};
direction = direction(end);
gear_ratio = raw{9};
gear_ratio = str2num(gear_ratio(end-4:end));

if direction == 'R'
    direction = 'Reverse';
elseif direction == 'F'
    direction = 'Foward';
end

%find header lines, number of runs
hlines = find(strcmp('time',raw));
nruns = size(hlines,1);
dataStart = hlines + 1;
dataEnd = dataStart + nsamples;

msCol = 2;
rpmCol = 3;
thrustCol = 4;
VCol = 5;
ACol = 6;

%nsamples = hlines(2) - hlines(1) - 3
rpm_mean = [];
thrust_mean = [];
V_mean = [];
A_mean = [];
eff_mean = [];
%generate matricies of run data
for s = 1:nruns
    rpm_mean = [rpm_mean, cell2mat(raw(dataStart(s):dataEnd(s),rpmCol))];
    thrust_mean = [thrust_mean, cell2mat(raw(dataStart(s):dataEnd(s),thrustCol))];
    V_mean = [V_mean, cell2mat(raw(dataStart(s):dataEnd(s),VCol))];
    A_mean = [A_mean, cell2mat(raw(dataStart(s):dataEnd(s),ACol))];
end

%matrix of effeciency points
eff_mean = thrust_mean ./ (A_mean .* V_mean);

ms = cell2mat(raw(dataStart:dataEnd,msCol));
servo_abs = abs(cell2mat(raw(dataStart:dataEnd,msCol))-1500);

%generage average and STD of all run data
rpm  = mean(rpm_mean,2);
rpm_std = std(rpm_mean,0,2);
thrust = mean(thrust_mean,2);
thrust_std = std(thrust_mean,0,2);
V = mean(V_mean,2);
V_std = std(V_mean,0,2);
A = mean(A_mean,2);
A_std = std(A_mean,0,2);
effeciency = mean(eff_mean,2);
eff_std = std(eff_mean,0,2);


%normalize thrust
percent_thrust = servo_abs/5;

%plot
figure;
subplot(2,1,1);
ax = plotyy(percent_thrust, thrust, percent_thrust, A);
hold(ax(1), 'on')
errorbar(ax(1),percent_thrust,thrust,thrust_std,thrust_std);
hold(ax(2), 'on')
errorbar(ax(2),percent_thrust,A,A_std,A_std);
grid on;

%labels, axes, and legends
xlabel(sprintf('Percent thrust in %s',direction))
title(sprintf('%s, %s, %s',esc_model, prop_model,motor_model));
ylabel(ax(1),'Thrust [kg]');
axis(ax(1),[0 140 0 2])
set(ax(1),'YTick',[0:.25:2])
set(ax(1),'box','off')
ylabel(ax(2),'Current [A]');
axis(ax(2),[0 140 0 15])
set(ax(2),'YTick',[0:2:15])
set(ax(2),'box','off')

subplot(2,1,2)
plot(percent_thrust,effeciency);
grid on;
axis([0 140 0 .05])
xlabel(sprintf('Percent thrust in %s',direction))
ylabel('Effeciency [kg/W]');

print('-dpng', name)

%put output data into cell/matrix
dataHeader = {'servo_ms','RPM','thrust_kg','voltage_V','current_A','RPM_STD','thrust_STD','V_STD','A_STD','effeciency','effeciency_STD','percent_thurst'};
dataOut = cell2mat({ms, rpm, thrust, V, A, rpm_std, thrust_std, V_std, A_std, effeciency, eff_std,percent_thrust});

%write all data to single xls file with filenames for sheet names
[success, message] = xlswrite(filename_out,metadata,name,'A1'); %metadata
[success, message] = xlswrite(filename_out,dataHeader,name,'A11'); %data headers
[success, message] = xlswrite(filename_out,dataOut,name,'A12'); %data

for m = 1:length(metadata)
    metadata_parts = regexp(metadata{m},': ','split');
    category = metadata_parts{1};
    value = metadata_parts{2};
    if isempty(value)
        value = 'NA';
    end
    
    %write metadata to structure
    data(f).filename = files(f);
    meta_cmd = sprintf('data(%i).%s = value',f,category);
    eval(meta_cmd);
end

for d = 1:length(dataHeader)
    cmd = sprintf('data(%i).%s = dataOut(:,d)',f,dataHeader{d});
    eval(cmd);
end

end

%clean up & save workspace
clearvars -except data
close all;
save data.mat

