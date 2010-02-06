#!/bin/sh
IP_ADDR=`/sbin/ifconfig  | grep 'inet addr:'| grep -v '127.0.0.1' | cut -d: -f2 | awk '{ print $1}'`;
echo "SET VARIABLE IP_ADD $IP_ADDR"
exit 0;
