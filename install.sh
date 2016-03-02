#!/bin/bash

make

mkdir -p /usr/lib/edison_config_tools/blockr
cp kr_6050_control.bin /usr/lib/edison_config_tools/blockr/

cp kr_6050_control.service /lib/systemd/system/
ln -s /lib/systemd/system/kr_6050_control.service /etc/systemd/system/default.target.wants/kr_6050_control.service


