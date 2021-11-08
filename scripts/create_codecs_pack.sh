#!/bin/sh

CURR_PWD=${PWD}

cd /
tar zcpvf hermes_codecs-1.1-2.tgz opt
alien -d --target=amd64 hermes_codecs-1.1-2.tgz

mv hermes-codecs_1.1-2_amd64.deb ${CURR_PWD}

cd ${CURR_PWD}
