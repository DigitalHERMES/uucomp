#!/bin/bash
# usage:
# compress_image.sh image_filename.{png,gif,...} [output.{jpg,avif,heic,vvc}]

## env vars:
# VVC_ENC: vvc enc binary
# TARGET SIZE: target size in bits
# input_file
# output_file

# initial VVC QP... it will only get bigger...
VVC_QP=39

MAX_DIMENSION_SIZE=840

QUALITY=75 # initial start quality to try for jpeg

VVC_ENC=${VVC_ENC:=/opt/vvc/vvencapp}
EVC_ENC=${EVC_ENC:=/root/xeve/build/bin/xeve_app}
AV1_ENC=${AV1_ENC:=/root/aom/build2/aomenc}

# maximum image size setting
MAX_SIZE=${MAX_SIZE:=10000} # 10kB max size limit

# converting to bits...
TARGET_SIZE=${TARGET_SIZE:=$((${MAX_SIZE} * 8))}


# vvc and evc  are the state of the art, no integration to userlad
# avif and heic are already implemented and integrated to userland
# jpg is the legacy format
# IMAGE_FORMAT=${IMAGE_FORMAT:=heic}

if [ $# -lt 2 ]; then
  echo "Usage: $0 image_filename.{png,gif,...} [output.{jpg,avif,heic,vvc}]"
  exit 1
fi

input_file=${input_file:=${1}}
output_file=${output_file:=${2}}

IMAGE_FORMAT="${output_file##*.}"

TEMPFILE=/tmp/temp-$$.${IMAGE_FORMAT}
TEMPFILEYUV=/tmp/temp-$$.yuv
RCFILE=/tmp/temp-$$.rc


# this was for JPEG support...
## cp -f "${input_file}" ${TEMPFILE}

resolution=$(ffprobe -v error -select_streams v:0 -show_entries stream=width,height -of csv=p=0 "${input_file}" | sed 's/,/x/g')
width=$(echo -n ${resolution} | cut -f 1 -d x)
height=$(echo -n ${resolution} | cut -f 2 -d x)

changed_resolution=0

echo "Original File size = $(stat -c%s "${input_file}")"
echo "Original Resolution = ${resolution}"

if [ "${width}" -gt "${MAX_DIMENSION_SIZE}" ] || [ "${height}" -gt "${MAX_DIMENSION_SIZE}" ]; then
  if [ "${width}" -gt "${height}" ]; then
    new_width=${MAX_DIMENSION_SIZE}
    ratio=$(awk "BEGIN {x=${width}/${new_width}; printf \"%.4f\n\",x}")
    new_height=$(awk "BEGIN {x=${height}/${ratio}; printf \"%d\",x}")
  else
    new_height=${MAX_DIMENSION_SIZE}
    ratio=$(awk "BEGIN {x=${height}/${new_height}; printf \"%.4f\n\",x}")
    new_width=$(awk "BEGIN {x=${width}/${ratio}; printf \"%d\",x}")
  fi
  width=${new_width}
  height=${new_height}
  changed_resolution=1
fi

# make resolution multiple of 8
width=$(( (${width} / 8) * 8 ))
height=$(( (${height} / 8) * 8 ))
resolution=${width}x${height}

echo "New Resolution = ${resolution}"

# ffprobe -v error -select_streams v:0 -show_entries stream=width -of default=nw=1:nk=1
# ffmpeg -i ../../testeCapacidadeMidias/images/FotoFoneMaq_Zap_IncendioBernardoSayao10-07-21_Felipe_Spina_Avino_WWF\ \(1\).jpeg -vf scale=840:840:force_original_aspect_ratio=decrease output.jpg
# ffmpeg -i blue.png -vf scale=out_color_matrix=bt709:flags=full_chroma_int+accurate_rnd,format=yuv420p yuv420p_709.yuv
# -vf scale=840:840:force_original_aspect_ratio=decrease

# input will be converted to YUV 4:2:0 10 bit


if [ ${changed_resolution} -eq "1" ]; then
  echo "Content will be downscaled"
  ffmpeg -y -i "${input_file}"  -f rawvideo -vf scale=width=${width}:height=${height}:out_color_matrix=bt709,format=yuv420p10le ${TEMPFILEYUV}
else
  ffmpeg -y -i "${input_file}"  -f rawvideo -vf scale=out_color_matrix=bt709,format=yuv420p10le ${TEMPFILEYUV}
fi

if [ ${IMAGE_FORMAT} = "evc" ]; then

    echo ${EVC_ENC} -w ${width} -h ${height} -d 10 -z 1 -m 2 --profile main --preset medium --bitrate $(( ${TARGET_SIZE} / 1000 ))  -i ${TEMPFILEYUV} -o  ${TEMPFILE}
    ${EVC_ENC} -w ${width} -h ${height} -d 10  -z 1 -m 2 --profile main --preset medium --bitrate $(( ${TARGET_SIZE} / 1000 ))  -i ${TEMPFILEYUV} -o  ${TEMPFILE}

elif [ ${IMAGE_FORMAT} = "vvc" ]; then

    ${VVC_ENC} -i ${TEMPFILEYUV} --profile main_10_still_picture --qpa 1 -f 1 -c yuv420_10 -t 2 -r 1 --qp ${VVC_QP} -s ${resolution} --preset medium -o  ${TEMPFILE}

    if [ "$(stat -c%s "${TEMPFILE}")" -gt "${MAX_SIZE}" ]; then
      ${VVC_ENC} -i ${TEMPFILEYUV} --profile main_10_still_picture --qpa 1 -f 1 --pass 1 --rcstatsfile ${RCFILE} -c yuv420_10 -t 2 -r 1 -b ${TARGET_SIZE} -s ${resolution} --preset medium -o  ${TEMPFILE}
      ${VVC_ENC} -i ${TEMPFILEYUV} --profile main_10_still_picture --qpa 1 -f 1 --pass 2 --rcstatsfile ${RCFILE} -c yuv420_10 -t 2 -r 1 -b ${TARGET_SIZE} -s ${resolution} --preset medium -o  ${TEMPFILE}
      rm -f ${RCFILE}
    fi

## old QP-based rate control
#    while [ "$(stat -c%s "${TEMPFILE}")" -gt "${MAX_SIZE}" ] && [ "$VVC_QP" -lt "61" ]; do
#      VVC_QP=$((VVC_QP+3))
#      ${VVC_ENC} -i ${TEMPFILEYUV} --profile main_10_still_picture --qpa 1 -c yuv420_10 -t 2 -r 1 --qp ${VVC_QP} -s ${resolution} --preset medium -o  ${TEMPFILE}
#    done;

    rm -f ${TEMPFILEYUV}

elif [ ${IMAGE_FORMAT} = "avif" ]; then
  echo "AVIF support not implemented."
  exit
# TODO
##  ${AV1_ENC} --target-bitrate=${TARGET_SIZE} --end-usage=cbr --bit-depth=8 ...
elif [ ${IMAGE_FORMAT} = "jpg" ]; then

  echo "JPEG support is temporarily removed."
  exit

  while [ "$(stat -c%s "${TEMPFILE}")" -gt "$MAX_SIZE" ] && [ "${QUALITY}" -gt "5" ]; do
    convert -resize "840x840>" "${input_file}" pnm:- | /opt/mozjpeg/bin/cjpeg -quality ${QUALITY} > ${TEMPFILE}
    QUALITY=$((QUALITY-10))
  done;

elif [ ${IMAGE_FORMAT} = "heic" ]; then

  echo "HEIF support not implemented."
  exit

# TODO
##  while [ "$(stat -c%s "${TEMPFILE}")" -gt "$MAX_SIZE" ] && [ "$QUALITY" -gt "5" ]; do
##    convert -resize "840x840>"  "${input_file}" -quality ${QUALITY} ${TEMPFILE}
##    QUALITY=$((QUALITY-10))
##  done;

else
  echo "Unsupported extension: ${IMAGE_FORMAT}."
  exit
fi

# in place
#if [ $# -eq 1 ]; then
#  mv ${TEMPFILE} "${input_file}"
#fi

rm -f ${TEMPFILEYUV}

echo "Final file size: $(stat -c%s "${TEMPFILE}")"


# with output file specified
if [ $# -eq 2 ]; then
  mv ${TEMPFILE} "${output_file}"
fi
