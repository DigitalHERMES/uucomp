#!/bin/sh

CURR_PWD=${PWD}

cd /
tar zcpvf hermes_codecs-1.0-1rd.tgz opt
alien -d hermes_codecs-1.0-1rd.tgz

mv hermes-codecs_1.0-2_all.deb ${CURR_PWD}

cd ${CURR_PWD}
