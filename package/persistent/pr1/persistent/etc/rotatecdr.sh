#!/bin/hush 
#Original script with some improvements done by Atcom
export MMC=`cat /etc/mmc`
if [ $MMC == 1 ]
then
  logdir=/mnt/sd/asterisk/cdr-csv
else
  logdir=/persistent/var/log/asterisk/cdr-csv
fi
#logdir=/var/log/asterisk/cdr-csv
dirsize=`du -k $logdir | cut -f1 | tail -n 1`
dirsize=`expr $dirsize / 1024`
logfile=$logdir/Master.csv
logsize=`du -k $logfile | cut -f1`

#archive logfile if it's bigger than 500kb
if [ -f $logfile ] && [ $logsize -gt 500 ]
then
	grep -o '[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]' $logfile | tr -d '-'  | sort > /tmp/tmp_dates
	sh /etc/archive.sh $logfile $logdir/`head -n 1 /tmp/tmp_dates`-`tail -n 1 /tmp/tmp_dates`.csv
	rm /tmp/tmp_dates
fi

#remove oldest file/directory in logdir if size of logdir is bigger than 10mb
if [ -d $logdir ] && [ $dirsize -gt 10 ]
then
	rm -r $logdir/`ls -t $logdir | tail -n 1`
fi

