#!/bin/sh
#This script send a file as e-mailattachment using ssmtp mail transfer agent
#Syntax:
#	 mailfile file_name somboady@domain.com filetype(tiff/pdf)

random_number=`date '+%s'`
tmpfile='/tmp/email'$random_number
configfile='/etc/asterisk/fax2email.conf'

subject=`sed '/^\#/d' $configfile | grep 'emailsubject' | tail -n 1 | sed 's/^.*=//;s/^[[:space:]]*//;s/[[:space:]]*$//'`
sender=`sed '/^\#/d' $configfile | grep 'serveremail' | tail -n 1 | sed 's/^.*=//;s/^[[:space:]]*//;s/[[:space:]]*$//'`
body=`sed '/^\#/d' $configfile | grep 'emailbody' | tail -n 1 | sed 's/^.*=//;s/^[[:space:]]*//;s/[[:space:]]*$//'`

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

#tiff file
echo '--------------060502030501040302050009' >> $tmpfile
if [ -f "/bin/tiff2pdf" ]; then
  echo 'Content-Type: image/'$3';' >> $tmpfile
  echo ' name="'$1'.'$3'"' >> $tmpfile
  echo 'Content-Transfer-Encoding: base64' >> $tmpfile
  echo 'Content-Disposition: attachment;' >> $tmpfile
  echo ' filename="'$1'.'$3'"' >> $tmpfile
else
  echo 'Content-Type: image/tiff;' >> $tmpfile
  echo ' name="'$1'.tiff"' >> $tmpfile
  echo 'Content-Transfer-Encoding: base64' >> $tmpfile
  echo 'Content-Disposition: attachment;' >> $tmpfile
  echo ' filename="'$1'.tiff"' >> $tmpfile
fi

#encode the file
if [ $3 = "pdf" ] && [ -f "/bin/tiff2pdf" ]; then
  tiff2pdf $1 -c "SwitchFin PBX" -o $1
fi
uuencode -m $1 $1 | sed '$d' >> $tmpfile

#email footer
echo '' >> $tmpfile
echo '--------------060502030501040302050009--' >> $tmpfile

#Finally send the e-mail using ssmtp
cat $tmpfile | ssmtp $2

#clean the tmp files
rm $tmpfile

