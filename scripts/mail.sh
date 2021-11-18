#!/bin/bash

file=${BASH_ARGV[*]}
#sample
#file="/var/spool/uucp/B0DE/C./C.NRIPLjRAA5vU"
echo "file: "  $file

#filter crmail from C  and get D
D=$(cat $file | grep crmail | awk '{print $6 ;}' )
echo "D: " $D

#get dest 
to=$(cat $file | cut -d ' ' -f 11- )
echo "To: " $to

#get uucp host path prefix
#file_dir=$(echo $file | awk -F '/' '{print $1"/";}')

host_dir=$(echo $file | awk -F '/' '{print $5;}')
echo "host_dir " $host_dir

from=$(zcat /var/spool/uucp/$host_dir/D./$D | head -n1 | awk '{print $2;}')
echo "from " $from

subject=$(zcat /var/spool/uucp/$host_dir/D./$D | head -n20 |grep Subject| awk '{print $2;}')
echo "subject " $subject

#send mail
echo "Your email with title: $subject \n send to $to was canceled"| mail $from -s "Email cancelled" 
