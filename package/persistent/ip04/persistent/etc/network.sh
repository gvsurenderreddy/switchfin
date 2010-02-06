#!/bin/hush 

exec_option=$1

/bin/fixrc

if [ -f /persistent/etc/network.conf ]
then
	. /persistent/etc/network.conf
	echo "$TIMEZONE" > /etc/TZ
	if [ $exec_option == start ]
	then
		hostname -F /etc/HOSTNAME
		if [[ "$DHCPD" == yes ]]
		then
			rm -f /var/run/dhcpcd*
			dhcpcd &
		elif [[ "$DHCPD" == auto ]]
                then    
			ifconfig $IF $IPADDRESS netmask $NETMASK up
                        route add default gw $GATEWAY
                        if [ ! -f /etc/resolv.conf ]
                        then    
                               	touch /etc/resolv.conf
                        fi
                        echo "nameserver  $DNS" > /etc/resolv.conf
 			if test `ping -c 1 $GATEWAY |grep "ttl" |wc -l` -gt 0
			then
				echo "It seems that our statically assing IP address if working..."
			else
				echo "We can not ping our gateway, attempting to start dhcp client..."
                        	rm -f /etc/resolv.conf
                        	rm -f /var/run/dhcpcd*
                        	dhcpcd &
			fi

		else
			ifconfig $IF $IPADDRESS netmask $NETMASK up
			route add default gw $GATEWAY
			if [ ! -f /etc/resolv.conf ]
			then
				touch /etc/resolv.conf
			fi
			echo "nameserver  $DNS" > /etc/resolv.conf
		fi
	elif [ $exec_option == stop ]
	then
		killall -9 dhcpcd
		rm -f /var/run/dhcpcd*
		ifconfig $IF down
	fi
	
ZONE=`echo $TIMEZONE | awk -F, '{print $1}'`                                                                               
ln -sf /persistent/usr/share/zoneinfo/$ZONE /etc/localtime   

else
		ifconfig eth0 192.168.1.100 netmask 255.255.255.0 up
		route add default gw 192.168.1.1
		echo "nameserver  192.168.1.1" > /etc/resolv.conf
fi
