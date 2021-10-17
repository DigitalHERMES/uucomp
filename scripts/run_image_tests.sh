#!/bin/sh

CODEC=vvc

SOURCE_DIR=/home/hermes/content/images
ENCODED_DIR=/home/hermes/content/encoded-vvc
RECONSTRUCTED_DIR=/home/hermes/content/reconstructed-vvc

COMPRESS_IMAGE=/usr/bin/compress_image.sh
DECOMPRESS_IMAGE=/usr/bin/decompress_image.sh

cd ${SOURCE_DIR}

mkdir -p ${ENCODED_DIR}/
mkdir -p ${RECONSTRUCTED_DIR}/

rm -f ${ENCODED_DIR}/*
rm -f ${RECONSTRUCTED_DIR}/*

# compress data
for i in *; do

    no_extension=${i%.*}

    input_file=\"${i}\"
    output_file=\"${ENCODED_DIR}/${no_extension}.${CODEC}\"
    echo "==== COMPRESSION"
    echo "==== INPUT: ${input_file}"
    echo "==== OUTPUT: ${output_file}"
    eval ${COMPRESS_IMAGE} ${input_file} ${output_file}

    echo "==== DONE"
done

cd ${ENCODED_DIR}

# decompress data
for i in *; do

    no_extension=${i%.*}

    input_file=\"${i}\"
    output_file=\"${RECONSTRUCTED_DIR}/${no_extension}.jpg\"
    echo "==== DECOMPRESSION"
    echo "==== INPUT: ${input_file}"
    echo "==== OUTPUT: ${output_file}"
    eval ${DECOMPRESS_IMAGE} ${input_file} ${output_file}

done
