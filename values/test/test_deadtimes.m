
function test_deadtimes()

	#
	# Import
	#
	name = "deadtimes";
	separator = ";";
	csv = dlmread(file=[name ".csv"], sep=separator);
	index = csv(:, 1);
	angle = csv(:, 2);
	ticks1 = csv(:, 3);
	ticks2 = csv(:, 4);
	deadtime1 = csv(:, 5);
	deadtime2 = csv(:, 6);

	#
	# Plot
	#
	myplot = figure(
	'Units','centimeters',
	'PaperUnits','centimeters',
	'PaperType','a5',
	'PaperOrientation','landscape',
	'PaperPosition',[0 0 29.7 21],
	'PaperPositionMode', 'auto'
	);

	subplot(1, 2, 1);
	[ax,h1,h2] = plotyy(angle, deadtime1, angle, deadtime2);
	hold on;
	grid on;
	ylabel(ax(1), "Dead-time between HS turn-off and LS turn-on / ns");
	ylabel(ax(2), "Dead-time between LS turn-off and HS turn-on / ns");

	title([name " test"]);
#	ylim([5 35]);
	set(ax(1), 'Xtick', [0:0.5:6.9]);
#	set(ax(2), 'Xtick', [0:20:365]);
    set([h1,h2], "linewidth", 1.5);
#	set(ax(1), 'Ytick', [0:1:40]);
#	set(ax(2), 'Ytick', [0:1:40]);

	subplot(1, 2, 2);
	[bx,h3,h4] = plotyy(angle, ticks1, angle, ticks2);
	hold on;
	grid on;
	ylabel(bx(1), "Dead-time between HS turn-off and LS turn-on / ticks");
	ylabel(bx(2), "Dead-time between LS turn-off and HS turn-on / ticks");

	title([name " test"]);
#	ylim([5 35]);
	set(bx(1), 'Xtick', [0:0.5:6.9]);
#	set(bx(2), 'Xtick', [0:20:365]);
    set([h3,h4], "linewidth", 1.5);
	set(bx(1), 'Ytick', [0:1:40]);
	set(bx(2), 'Ytick', [0:1:40]);

	saveas(myplot, [name ".pdf"]);

	clf;
	#clf;

endfunction
