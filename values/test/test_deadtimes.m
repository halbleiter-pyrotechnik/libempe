
function test_deadtimes()

	#
	# Import
	#
	name = "deadtimes";
	separator = ";";
	csv = dlmread(file=[name ".csv"], sep=separator);
	index = csv(:, 1);
	angle = csv(:, 2);
	deadtime1 = csv(:, 3);
	deadtime2 = csv(:, 4);

	#
	# Plot
	#
	myplot = figure(1);
	[ax,h1,h2] = plotyy(angle, deadtime1, angle, deadtime2);
#    plot(angle, deadtime1);
#	hold on;
	grid on;

	title([name " test"]);
#	ylim([5 35]);
	set(ax(1), 'Xtick', [0:0.5:6.9]);
#	set(ax(2), 'Xtick', [0:20:365]);
    set([h1,h2], "linewidth", "1.5px");
	set(ax(1), 'Ytick', [0:1:40]);
	set(ax(2), 'Ytick', [0:1:40]);

	saveas(myplot, [name ".pdf"]);

	#clf;

endfunction
