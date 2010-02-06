#!/bin/bash
option=$1
if [ $option == "toauto" ]
then
cat /etc/network.conf | tr \# \; | sed 's/\;\[network\]/\[network\]/g' > /etc/asterisk/rc_auto.conf
elif [ $option == "fromauto" ]
then
cat /etc/asterisk/rc_auto.conf | sed 's/\[network\]/\;\[network\]/g' | tr \; \# | sed '/#!/d' | tr -d ' ' > /etc/network.conf
else
echo "Nothing to do.."
fi
