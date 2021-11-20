#!/bin/bash

if [ $# -eq 0 ] || [ $# -gt 3 ] || [ $# -eq 2 ]; then
  echo "Usage: $0 [es|en sys|gui] uuid"
  exit 1
fi

#get args
if [ $# -eq 3 ]; then
  lang=${1}
  type=${2}
  uuid=${3}
elif [ $# -eq 1 ]; then
  lang="en"
  type="sys"
  uuid=$1
fi

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
D=$(cat $fullC | grep crmail | awk '{print $2 ;}')

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

if [ $lang = "en" ] && [ $type = "gui" ]; then
  message="Your email with destination(s): $to with subject: $subject was canceled by the admin user. \n\n This is an automatic message from Hermes System!"
  subject="Email canceled by the admin user"
elif [ $lang = "en" ] && [ $type = "sys" ]; then
  message="Your email with destination(s): $to with subject: $subject was canceled because exceeds system limits. \n\n This is an automatic message from Hermes System!"
  subject="Email canceled by the system"
elif [ $lang = "es" ] && [ $type = "gui" ]; then
  message="Su correo electrónico con destino(s): $to con asunto: $subject fue cancelado por el usuario administrador. \n\n Este es un mensaje del sistema automático de Hermes!"
  subject="Email cancelado por el administrador"
elif [ $lang = "es" ] && [ $type = "sys" ]; then
  message="Su correo electrónico con destino(s): $to con asunto: $subject fue cancelado por exceder los límites del sistema. \n\n Este es un mensaje del sistema automático de Hermes!"
  subject="Email cancelado por el sistema"
fi

#send email
echo $message | mail  -s "email cancelled" $from
