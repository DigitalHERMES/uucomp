#!/bin/bash

# maximum email size
MAX_EMAIL_SIZE=${MAX_EMAIL_SIZE:=20000}

# maximum queue size
MAX_UUCP_QUEUE=${MAX_UUCP_QUEUE:=80000}

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
        id=$(echo ${j} | cut -d . -f 2)
        uuid="${i}.${id}"
        if [ ${D_size} -gt ${MAX_EMAIL_SIZE} ]
        then
          echo "UUID = ${uuid} SIZE ${D_size} WILL ME MAILKILLED!"
          mailkill.sh ${uuid}
        else
          echo "UUID = ${uuid} SIZE ${uuid} IS GOOD"
        fi
      fi

    done
  fi
done

# now check for the size of the full uucp queue... and delete the newest emails..
total_size=$(uustat -a | awk -F ' ' '{sum+=$(NF - 1);}END{print sum;}')

if [ ${total_size} -gt ${MAX_EMAIL_SIZE} ]
then
  echo "Total size of the UUCP (${total_size}) exceed the maximum of ${MAX_EMAIL_SIZE}"
  # iterate in all emails...
  # use the SEQF? # use  the time of creation of the C. to identify the latest created email?
else
  echo "Total UUCP queue size ${total_size} is good"
fi
