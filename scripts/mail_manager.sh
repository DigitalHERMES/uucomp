#!/bin/bash

# maximum
MAX_EMAIL_SIZE=${MAX_EMAIL_SIZE:=20000}

for i in $(ls -1 /var/spool/uucp/)
do
  if [ -d /var/spool/uucp/${i}/C. ]
  then
    echo "Looking at system ${i}"
    cd /var/spool/uucp/${i}/C.
    for j in $(ls -1)
    do
      uucomp ${j}

      # check if it is mail
      cmd=$(cat ${j} | cut -d ' '  -f 10 )
      if [ "${cmd}" == "crmail" ]
      then
        D=$(cat ${j} | grep crmail | awk '{print $2 ;}')
        D_size=$(stat -c %s ../D./${D})
        if [ ${D_size} -gt ${MAX_EMAIL_SIZE} ]
        then
          id=$(echo ${j} | cut -d . -f 2)
          uuid="${i}.${id}"
          echo "UUID = ${uuid} SIZE ${D_size} WILL ME MAILKILLED!"
          mailkill.sh ${uuid}
        else
          echo "UUID = ${uuid} SIZE ${uuid} IS GOOD"
        fi
      fi

    done
  fi
done
