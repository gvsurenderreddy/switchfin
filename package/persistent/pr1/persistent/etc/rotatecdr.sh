#!/bin/hush 
export MMC=`cat /etc/mmc`
if [ $MMC == 1 ]
then
  logdir=/mnt/sd/asterisk/cdr-csv
else
  logdir=/persistent/var/log/asterisk/cdr-csv
fi
#logdir=/var/log/asterisk/cdr-csv
dirsize=`du -md 0 $logdir | cut -f1`
logfile=Master.csv
logsize=`du -k $logfile | cut -f1`

#archive logfile if it's bigger than 500kb
if [ -f $logdir/$logfile ] && [ $logsize -gt 500 ]
then
	smallestval=999999999
	largestval=0
	grep -o '[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]' $logdir/$logfile | tr -d '-' > /tmp/tmp_dates
	for i in `cat /tmp/tmp_dates`
	do
		if [ $i -lt $smallestval ]
		then
			smallestval=$i
		elif [ $i -gt $largestval ]
		then
			largestval=$i
		fi
	done
	sh /etc/archive.sh $logdir/$logfile $logdir/$smallestval-$largestval.csv
fi

#remove oldest file/directory in logdir if size of logdir is bigger than 10mb
if [ -d $logdir ] && [ $dirsize -gt 10 ]
then
	rm -r $logdir/`ls -t $logdir | tail -n 1`
fi

rm /tmp/tmp_dates
