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


