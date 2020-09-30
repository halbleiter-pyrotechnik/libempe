#!/bin/bash
#
# This script adds a configurable delta to the modification time of a file
# and adds this time as a prefix to the filename.
# This is useful whenever data from some instrument is saved,
# but the instrument is set to an incorrect date/time.
# Negative values are permitted.
#

delta_hours=-2
delta_minutes=7

files=$@
if [ "$files" == "" ]; then
	echo "Usage: $0 <filenames>"
	exit 0
fi

script='
year = $6;
month = $7;
day = $8;
hour = $9;
minute = $10;
filename = $11;

hour = hour + delta_hours;
minute = minute + delta_minutes;
while (minute < 0)
{
	minute = minute + 60;
	hour = hour - 1;
}
while (minute >= 60)
{
	minute = minute - 60;
	hour = hour + 1;
};
while (hour < 0)
{
	hour = hour + 24;
	day = day - 1;
}
while (hour >= 24)
{
	hour = hour - 24;
	day = day + 1;
}

if (day < 10)
	day = "0" day;
if ((hour >= 0) && (hour < 10))
	hour = "0" hour;
if (minute < 10)
	minute = "0" minute;

print year "-" month "-" day "_" hour "-" minute "_Uhr_" filename;
'

for filename in $files; do
	if [ -d "$filename" ]; then
		echo "\"$filename\" is a directory. Skipping."
		continue
	fi
	if [ ! -f "$filename" ]; then
		echo "File not found: \"$filename\""
		continue
	fi

	time_style="+%d.%m.%Y %H %M Uhr"
	ls=$(ls -l --time=ctime --time-style="$time_style" "$filename")
	cdate=$(echo -n $ls | awk {'print $6'})
	ctime=$(echo -n $ls | awk {'print $7 ":" $8 " Uhr"'})

	time_style="+%Y %m %d %H %M"
	ls=$(ls -l --time=ctime --time-style="$time_style" "$filename")
	newname=$(echo -n $ls | awk -v delta_hours=$delta_hours -v delta_minutes=$delta_minutes {"$script"})

	echo $filename - $cdate - $ctime " -> " $newname
	mv -v "$filename" "$newname"
done

