#!/bin/hush 

exec_option=$1

if [ -f /persistent/etc/network.conf ]
then
	. /persistent/etc/network.conf
	if [ $exec_option == start ]
	then
		if [[ "$VLAN" == yes ]]
		then
			vconfig add $IF $VLANNUMBER
			ifconfig $IF.$VLANNUMBER $VLANIPADDRESS netmask $VLANSUBNET up
			route add default gw $VLANGATEWAY
		else
			echo "VLAN not configured!"
		fi
	elif [ $exec_option == stop ]
	then
		ifconfig $IF.$VLANNUMBER down
		vconfig rem $IF.$VLANNUMBER
	fi
else
	vconfig add eth0 100
	ifconfig eth0.100 192.168.100.100 netmask 255.255.255.0 up
	route add default gw 192.168.100.1
fi
