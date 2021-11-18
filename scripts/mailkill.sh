#!/bin/bash

if [ $# -eq 0 ]; then
  echo "need uuid arg"
  exit
fi

#get uuid from args
uuid=${BASH_ARGV[*]}

#get uucp host path prefix
host=$(echo $uuid | cut -d "." -f1)

#get uuidwh:  uuid without host
uuidwh=$(echo $uuid | cut -d "." -f2)

fullC=/var/spool/uucp/$host/C./C.$uuidwh
if [ ! -f $fullC ] ;  then
  echo "error - no C file... exiting"
  exit
fi
  
#filter crmail from C and get D
D=$(cat $fullC | grep crmail | awk '{print $6 ;}')

#get destination
to=$(cat $fullC | cut -d ' ' -f 11- )
echo "To: " $to

fullD="/var/spool/uucp/$host/D./$D" 
if [ ! -f $fullD ] ; then
  echo "error - no D file... exiting"
  exit
fi
echo "FullD " = $fullD

from=$(zcat $fullD | head -n1 | awk '{print $2;}')

subject=$(zcat $fullD | head -n20 |grep Subject| awk '{print $2;}')

kill=$(uustat -k $uuid)

#send email
echo "Your email with title: $subject \n to $to was cancelled"| mail  -s "email cancelled" $from

