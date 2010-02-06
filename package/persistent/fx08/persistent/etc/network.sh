#!/bin/sh 

exec_option=$1
/bin/fixrc

if [ -f /persistent/etc/network.conf ]
then
	. /persistent/etc/network.conf
	echo $TIMEZONE > /etc/TZ
	if [ $exec_option == start ]
	then
		echo $HOSTNAME > /etc/HOSTNAME
		hostname -F /etc/HOSTNAME
		echo "127.0.0.1  $DOMAIN  localhost" > /etc/hosts
		echo "127.0.0.1  $HOSTNAME" >> /etc/hosts
		if [ "$DHCPD" == yes ]
		then
			rm -f /var/run/dhcpcd*
			dhcpcd &
		else
		  ifconfig $IF down
			ifconfig $IF hw ether $MAC
			ifconfig $IF $IPADDRESS netmask $NETMASK up
			route add default gw $GATEWAY
			if [ ! -f /etc/resolv.conf ]
			then
				touch /etc/resolv.conf
			fi
			echo "nameserver  $DNS" > /etc/resolv.conf
		fi
		ifconfig eth1 hw ether $LAN_MAC
		ifconfig eth1 $LAN_IP netmask $LAN_MASK up
		sleep 5
	elif [ $exec_option == stop ]
	then
		killall -9 dhcpcd
		rm -f /var/run/dhcpcd*
		ifconfig $IF down
	fi
ZONE=`echo $TIMEZONE | awk -F, '{print $1}'`                                                                               
if [ -f /etc/localtime ]                                                                                                   
        rm /etc/localtime                                                                                                  
fi                                                                                                                         
                                                                                                                           
ln -s /persistent/usr/share/zoneinfo/$ZONE /etc/localtime   
else
	if [ -f /etc/.rootfs_copied ]
	then
		ifconfig eth0 192.168.0.100 up
		route add default gw 192.168.0.254
		echo "nameserver  202.96.69.38" > /etc/resolv.conf
		sleep 5
	fi
fi
