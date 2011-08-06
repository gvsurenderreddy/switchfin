#!/bin/hush
case "$1" in

get_device)
  state=`home-auto /dev/wltc get_device $2`
  echo "SET VARIABLE WLRR_STATE $state"
  exit 0;
;;

set_device)
  state=`home-auto /dev/wltc set_device $2 $3`
  echo "SET VARIABLE WLRR_STATE $state"
  exit 0;
esac
