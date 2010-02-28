#!/bin/sh
#This script send a file as e-mailattachment using ssmtp mail transfer agent
#Syntax:
#	 mailfile file_name somboady@domain.com 


#Template of the e-mail using MIME protocol
cp /bin/mailfile_header_template /tmp/email

#Include the file name in the template
sed -i "s/name_of_the_file/$1/g" /tmp/email 

#Encode the file
uuencode -m $1 $1 | sed '1d$d' >> /tmp/email

#Add footer
cat /bin/mailfile_footer_template >> /tmp/email

#Finally send the e-mail using ssmtp
cat /tmp/email | ssmtp $2

#clean the tmp files
rm /tmp/email

