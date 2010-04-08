#!/bin/hush
#
#/etc/ast
#
## Check the used memory and restart Asterisk if too much memory is used

#Clear the cache
sync;echo 3 > /proc/sys/vm/drop_caches

#get the free memory
freemem=`top -n 1| head -n 1| sed  's/.*used, \([0-9]*\).*/\1/'`

#restart asterisk if mem is too low
if [ $freemem -lt 2000 ]; then
        datestr=`date`
	echo "Restart Asterisk: $datestr : freemem=$freemem" >> /tmp/mem.sh.log
	/etc/init.d/asterisk restart
fi

