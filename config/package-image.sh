#!/bin/sh

# Places a 32 byte MD5 ASCII checksum on the end of the input file

if [ "$2" = "" ]; then
	echo "Usage: package-image <source file> <destination file>"
	exit 1
fi

SRC=$1
DST=$2
PID=$$
TMP="temp-${PID}.MD5"

echo "Building MD5 embedded distributable image"
# Insure base file size is padded out to a multiple of 32 
dd bs=32 if=${SRC} of=${DST} conv=sync
FILESIZE=$(stat -c%s ${DST})
NUMBLOCKS=`expr $FILESIZE / 32 `
md5sum ${DST} >${TMP}
dd bs=32 seek=$NUMBLOCKS count=1 if=${TMP} of=${DST} oflag=append
rm ${TMP}
