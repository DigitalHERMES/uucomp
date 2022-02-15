#!/bin/bash

# maximum email size
MAX_EMAIL_SIZE=${MAX_EMAIL_SIZE:=20000}

# maximum queue size
MAX_UUCP_QUEUE=${MAX_UUCP_QUEUE:=80000}


# input paramenters: $HOST
get_uucp_queue_size_for_host ()
{
  total_size=0
  if [ -d /var/spool/uucp/${HOST}/C. ]
  then
    echo "Calculating size of email uucp queue for host: ${HOST}."
    cd /var/spool/uucp/${HOST}/C.
    for k in $(ls -1)
    do
      cmd=$(cat ${k} | cut -d ' '  -f 10 )
      if [ "${cmd}" == "crmail" ]
      then
         D=$(cat ${k} | grep crmail | awk '{print $2 ;}')
         D_size=$(stat -c %s ../D./${D})
         if [[ $D_size =~ ^[+-]?[0-9]+$ ]]
         then
           let total_size=total_size+D_size
         fi
      fi
    done
  fi
}

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
          mailkill.sh size_limit ${uuid}
        else
          echo "UUID = ${uuid} SIZE ${uuid} IS GOOD"
        fi
      fi

    done
  fi
done


# now check for the size of the emails in the uucp queue for each host... 
# and delete the newest emails.. but not rmail, as there might
# be other emails being processed together
total_size=0

for i in $(ls -1 /var/spool/uucp/)
do
  if [ -d /var/spool/uucp/${i}/C. ]
  then
    HOST=${i}
    get_uucp_queue_size_for_host
    echo "UUCP email queue for ${HOST} is ${total_size} bytes."

    # total_size=$(uustat -a | grep -v ' rmail ' |  awk -F ' ' '{sum+=$(NF - 1);}END{print sum;}')

    cd /var/spool/uucp/${HOST}/C.

    while [ "${total_size}" -gt "${MAX_UUCP_QUEUE}" ]
    do
      newest_timestamp="0"
      echo "Total size of the UUCP (${total_size}) exceed the maximum of ${MAX_UUCP_QUEUE}. Deleting last email."

      for j in $(ls -1)
      do
            # check if it is mail
        cmd=$(cat ${j} | cut -d ' '  -f 10 )
        if [ "${cmd}" == "crmail" ]
        then
          # get the stat in unix seconds timestamp pra ficar facil de achar quem eh o mais novo...
          time_stamp="$( stat -c '%W' ${j} )"
          if [ "${time_stamp}" -gt "${newest_timestamp}" ]
          then
            newest_timestamp="${time_stamp}"
            id=$(echo ${j} | cut -d . -f 2)
            uuid="${i}.${id}"
          fi
        fi
      done

      echo "Deleting uuid: ${uuid} ${newest_timestamp}"
      # delete the newest email...
      mailkill.sh queue_full ${uuid}

      get_uucp_queue_size_for_host
    done
    echo "Total UUCP email queue size ${total_size} to ${HOST} is good"
  fi

done
