#!/bin/hush

fxs=`dahdi_scan | grep FXS | sed 's/port=\([1-8]\),FXS/\1 /' | sed 's/\([1-8]\)/\1,/g' | tr -d "\n" | sed 's/,.$//'`
fxo=`dahdi_scan | grep FXO | sed 's/port=\([1-8]\),FXO/\1 /' | sed 's/\([1-8]\)/\1,/g' | tr -d "\n" | sed 's/,.$//'`

echo loadzone = us > /etc/dahdi/system.conf
echo defaultzone = us >> /etc/dahdi/system.conf

if [ ! -z "$fxs" ]; then
	echo fxoks=$fxs >> /etc/dahdi/system.conf
	channels="$fxs"
fi

if [ ! -z "$fxo" ]; then
	echo fxsks=$fxo >> /etc/dahdi/system.conf
	if [ ! -z "$channels" ]; then
		channels="$fxs, $fxo"
	else
		channels="$fxo"
		
	fi
fi

if [ ! -z "$channels" ]; then
	echo echocanceller=oslec,$channels >> /etc/dahdi/system.conf
fi
