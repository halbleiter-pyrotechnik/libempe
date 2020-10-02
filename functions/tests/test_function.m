
function test_function (name)

	#
	# Import
	#
	separator = ";";
	csv = dlmread(file=[name ".csv"], sep=separator);
	time = csv(:, 1);
	angle = csv(:, 2);
	data = csv(:, 3);

	#
	# Plot
	#
	myplot = figure(1);
	plotyy(time, data, time, angle);
	hold on;
	grid on;

	title([name " function test"]);
	ylim([-0.1 0.1]);
	xticks(0:10:500);
	yticks(-0.1:0.01:0.1);

	saveas(myplot, [name ".pdf"]);

	clf;

endfunction
