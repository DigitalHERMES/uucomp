#!/bin/bash
# usage:
# decompress_audio file.{lpcnet,lyra} file.aac

LPCNET_DEC=${LPCNET_DEC:=/opt/lpcnet/lpcnet_demo}
LYRA_DEC=${LYRA_DEC:=/opt/lyra/decoder_main}

if [ $# -lt 2 ]; then
  echo "Usage: $0 audio_filename.{lpcnet,lyra} output.aac"
  exit 1
fi

input_file=${1}
output_file=${2}

AUDIO_FORMAT="${input_file##*.}"

TEMPFILE=/tmp/temp-$$.pcm

if [ ${AUDIO_FORMAT} = "lpcnet" ]; then
  ${LPCNET_DEC} -decode ${input_file} ${TEMPFILE}

  ffmpeg -y -f s16le -ar 16000 -ac 1 -c:a pcm_s16le -i ${TEMPFILE} ${output_file}

  rm -f ${TEMPFILE}

else
  echo "Unsupported extension: ${AUDIO_FORMAT}."
  exit
fi

echo "Final file size: $(stat -c%s "${output_file}")"
