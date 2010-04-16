#!/bin/hush
#
#The current BR4 Appliance is using about 21MB rootfs there are about 4MB 
#other stuff as you can check in the memory map in the boot log. So we 
#have around 40MB in total for RAM. In case the used memory become more 
#than 35MB the OS is in a shortage of free RAM, so we restart asterisk.    

#get the used memory
sync; echo 3 > /proc/sys/vm/drop_caches
usedmem=`top -n 1| head -n 1| sed  's/.*Mem: \([0-9]*\).*/\1/'`

#restart asterisk if free mem is too low
if [ $usedmem -gt 35000 ]; then
        datestr=`date`
	echo "Restart Asterisk: $datestr : usedmem=$usedmem" >> /tmp/mem.sh.log
	/etc/init.d/asterisk restart
fi

