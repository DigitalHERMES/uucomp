#!/bin/bash
# usage:
# compress_audio.sh audio_filename.{wav,mp3,aac,...} [output.{lpcnet,lyra}]

## env vars:
# LPCNET_ENC: lpcnet enc binary
# LYRA_ENC: lyra enc binary
# input_file
# output_file

LPCNET_ENC=${LPCNET_ENC:=/opt/lpcnet/lpcnet_demo}
LYRA_ENC=${LYRA_ENC:=/opt/lyra/encoder_main}

if [ $# -lt 2 ]; then
  echo "Usage: $0 audio_filename.{wav,mp3,aac,...} [output.{lpcnet,lyra}]"
  exit 1
fi

input_file=${input_file:=${1}}
output_file=${output_file:=${2}}

AUDIO_FORMAT="${output_file##*.}"

TEMPFILE=/tmp/temp-$$.${AUDIO_FORMAT}

if [ ${AUDIO_FORMAT} = "lpcnet" ]; then
  ffmpeg -y -i "${input_file}"  -c:a pcm_s16le -f s16le -ac 1 -ar 16000 ${TEMPFILE}
  ${LPCNET_ENC} -encode ${TEMPFILE} "${output_file}"

  rm -f ${TEMPFILE}

else
  echo "Unsupported extension: ${AUDIO_FORMAT}."
  exit
fi

echo "Final file size: $(stat -c%s "${output_file}")"
