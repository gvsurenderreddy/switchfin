####################################################################
####    Copyright Brandon Kruse <bkruse@digium.com> && Digium   ####
####################################################################

# Quick script for applying zaptel settings from the GUI.

ZAPCONF="/etc/zaptel.conf"
FILENAME="/etc/asterisk/applyzap.conf"

if grep -l hdb3 ${FILENAME} || grep -l ccs {FILENAME}
then
#E1
        grep -v '\;' ${FILENAME} | sed 's/\[general\]//g'|sed -r 's/fxsks[ =1-8,]*//g'|sed -r 's/fxoks[ =1-8,]*//g'|sed 's/bchan.*=.*/bchan=1-15,17-31/'|sed 's/dchan.*=.*/dchan=16/'|sed 's/crc4\/hdb3/hdb3,crc4/' > ${ZAPCONF}
else
#T1
        grep -v '\;' ${FILENAME} | sed 's/\[general\]//g'|sed -r 's/fxsks[ =1-8,]*//g'|sed -r 's/fxoks[ =1-8,]*//g'|sed 's/bchan.*=.*/bchan=1-23/'|sed 's/dchan.*=.*/dchan=24/' > ${ZAPCONF}
fi
