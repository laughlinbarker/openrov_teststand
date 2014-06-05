%Quick 'n dirty (but pretty) plots. Shows how to plot when using MATLAB structured  vars. Modify at will.
%
%Laughlin Barker - OpenROV - May 2014
%

clear; close all;

directory = 'C:\Users\cuervo\Documents\OpenROV\test_stand\5-29_30\';
cd(directory);

datafile = 'data.mat';
load(datafile)

numtests = size(data,2);

P = cellfun(@char,{data.Prop},'unif',0);
props = unique(P);
uniqueProps = size(props,2);

fwdMask = strcmp('F',{data.Direction});
fwdRuns = find(fwdMask);
revMask = not(fwdMask);
revRuns = find(revMask);

plot([data(fwdRuns).percent_thurst],[data(fwdRuns).thrust_kg],'LineWidth',2)
legend(data(fwdRuns).Prop,'Location','SouthEast');

figure;
plot([data(fwdRuns).percent_thurst],[data(fwdRuns).effeciency],'LineWidth',2)
legend(data(fwdRuns).Prop,'Location','SouthEast');

graph1 = [15 16 2 7];
graph2 = [17 18 5];

figure;
hold on;
%forward, on stinger
plot([data(2).percent_thurst],[data(2).thrust_kg],'g-','LineWidth',2);
%forward, in ROV
plot([data(11).percent_thurst],[data(11).thrust_kg],'g--','LineWidth',2);
%reverse, on stinger
plot([data(7).percent_thurst],[data(7).thrust_kg],'r-','LineWidth',2);
%reverse, in ROV
plot([data(12).percent_thurst],[data(12).thrust_kg],'r--','LineWidth',2);

legend('FWD on stinger','FWD in ROV','REV on stinger','REV in ROV','Location','NorthWest');
xlabel('Percent of Max Thrust');
ylabel('Thrust [kg]')
title('2308.65 prop, 2213 motor, EZRUN 18A ESC, V_{supp} = 9V')

figure;
hold on
%forward on stinger
plot([data(5).percent_thurst],[data(5).thrust_kg],'g-','LineWidth',2);
%forward on ROV
plot([data(17).percent_thurst],[data(17).thrust_kg],'g--','LineWidth',2);
%reverse on ROV
plot([data(18).percent_thurst],[data(18).thrust_kg],'r--','LineWidth',2);

legend('FWD on stinger','FWD in ROV','REV in ROV','Location','NorthWest');
xlabel('Percent of Max Thrust');
ylabel('Thrust [kg]')
title('2303.57 prop, 2213 motor, EZRUN 18A ESC, V_{supp} = 9V')

figure; hold on;
%2303.57 forward on ROV
plot([data(17).percent_thurst],[data(17).thrust_kg],'g--','LineWidth',2);
%2308.60 forward, in ROV
plot([data(11).percent_thurst],[data(11).thrust_kg],'g-','LineWidth',2);
%2303.57 reverse on ROV
plot([data(18).percent_thurst],[data(18).thrust_kg],'r--','LineWidth',2);
%2308.60 reverse, in ROV
plot([data(12).percent_thurst],[data(12).thrust_kg],'r-','LineWidth',2);

legend('2303.57 FWD','2308.60 FWD','2303.57 REV','2308.60 REV','Location','NorthWest');
xlabel('Percent of Max Thrust');
ylabel('Thrust [kg]')
title('In-situ thrust comparison, 2213 motor, EZRUN 18A ESC, V_{supp} = 9V')

figure; hold on;

%2303.57 forward on ROV
plot([data(17).percent_thurst],[data(17).RPM],'g--','LineWidth',2);
%2308.60 forward, in ROV
plot([data(11).percent_thurst],[data(11).RPM],'g-','LineWidth',2);
%2303.57 reverse on ROV
plot([data(18).percent_thurst],[data(18).RPM],'r--','LineWidth',2);
%2308.60 reverse, in ROV
plot([data(12).percent_thurst],[data(12).RPM],'r-','LineWidth',2);

legend('2303.57 FWD','2308.60 FWD','2303.57 REV','2308.60 REV','Location','NorthWest');
xlabel('Percent of Max Thrust');
ylabel('RPM')
title('In-situ RPM comparison, 2213 motor, EZRUN 18A ESC, V_{supp} = 9V')

figure; hold on;

%2303.57 forward on ROV
plot([data(17).percent_thurst],[data(17).effeciency],'g--','LineWidth',2);
%2308.60 forward, in ROV
plot([data(11).percent_thurst],[data(11).effeciency],'g-','LineWidth',2);
%2303.57 reverse on ROV
plot([data(18).percent_thurst],[data(18).effeciency],'r--','LineWidth',2);
%2308.60 reverse, in ROV
plot([data(12).percent_thurst],[data(12).effeciency],'r-','LineWidth',2);

legend('2303.57 FWD','2308.60 FWD','2303.57 REV','2308.60 REV','Location','NorthWest');
xlabel('Percent of Max Thrust');
ylabel('Efficiency [kg/W]')
title('In-situ efficiency comparison, 2213 motor, EZRUN 18A ESC, V_{supp} = 9V')