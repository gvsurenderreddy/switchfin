#!/bin/sh
#This script send a file as e-mailattachment using ssmtp mail transfer agent
#Note that the input tiff file will be deleted! 
#
#Syntax:
#	mailfile file_name somboady@domain.com [filetype]
#
#Arguments:	
#	file_name 		- Input tiff file name
#	somboady@domain.com 	- E-mail address
#	filetype		- Optional argument can be tiff or pdf
#				  If ommited tiff is assumed. 
#				  pdf type is used only if tiff2pdf tool is installed
#	
# 	  			

random_number=`date '+%s'`
tmpfile='/tmp/email'$random_number
configfile='/etc/asterisk/fax2email.conf'

subject=`sed '/^\#/d' $configfile | grep 'emailsubject' | tail -n 1 | sed 's/^.*=//;s/^[[:space:]]*//;s/[[:space:]]*$//'`
sender=`sed '/^\#/d' $configfile | grep 'serveremail' | tail -n 1 | sed 's/^.*=//;s/^[[:space:]]*//;s/[[:space:]]*$//'`
body=`sed '/^\#/d' $configfile | grep 'emailbody' | tail -n 1 | sed 's/^.*=//;s/^[[:space:]]*//;s/[[:space:]]*$//'`
if [ `ls | grep $1 | grep g3 | wc -l` != 0 ]; then
  #email header
  echo 'Subject: '$subject > $tmpfile
  echo 'From: '$sender >> $tmpfile
  echo 'Content-Type: multipart/mixed;' >> $tmpfile
  echo ' boundary="------------060502030501040302050009"' >> $tmpfile
  echo ' ' >> $tmpfile
  echo 'This is a multi-part message in MIME format.' >> $tmpfile
  echo '--------------060502030501040302050009' >> $tmpfile
  echo 'Content-Type: text/plain; charset=ISO-8859-1; format=flowed' >> $tmpfile
  echo 'Content-Transfer-Encoding: 7bit' >> $tmpfile

  #email body and replace /n with new line
  echo '' >> $tmpfile
  echo $body | sed 's/\/n/\n/g' >> $tmpfile
  echo '' >> $tmpfile

  #find fax encoding
  if echo $1*g3|grep 2D >/dev/null; then
    sz=2
  else
    sz=1
  fi

  #convert to tiff files
  for i in $1*g3; do 
    fax2tiff -s -3 -$sz -o $i.tiff $i
    rm -f $i
  done

  #combine tiff files
  tiffcp $1*.tiff /tmp/$1.tiff
  rm -f $1*tiff

  #tiff file
  echo '--------------060502030501040302050009' >> $tmpfile
  if [ "$3" == "pdf" ] && [ -f "/bin/tiff2pdf" ]; then
    echo 'Content-Type: application/'$3';' >> $tmpfile
    echo ' name="'$1'.'pdf'"' >> $tmpfile
    echo 'Content-Transfer-Encoding: base64' >> $tmpfile
    echo 'Content-Disposition: attachment;' >> $tmpfile
    echo ' filename="'$1.pdf'"' >> $tmpfile
    tiff2pdf /tmp/$1.tiff -c "SwitchFin PBX" -o /tmp/$1.pdf
    echo '' >> $tmpfile
    uuencode -m /tmp/$1.pdf /tmp/$1.pdf | sed '1d$d' >> $tmpfile
    rm /tmp/$1.pdf
  else
    echo 'Content-Type: image/tiff;' >> $tmpfile
    echo ' name="'$1'"' >> $tmpfile
    echo 'Content-Transfer-Encoding: base64' >> $tmpfile
    echo 'Content-Disposition: attachment;' >> $tmpfile
    echo ' filename="'$1.tiff'"' >> $tmpfile
    echo '' >> $tmpfile
    uuencode -m /tmp/$1.tiff /tmp/$1.tiff | sed '1d$d' >> $tmpfile 
  fi

  #email footer
  echo '' >> $tmpfile
  echo '--------------060502030501040302050009--' >> $tmpfile

  #Finally send the e-mail using ssmtp
  cat $tmpfile | ssmtp $2

  #clean the tiff and tmp file
  rm $tmpfile
  rm /tmp/$1.tiff
fi
