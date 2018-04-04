#!/bin/bash

set -eux

#tmp=$(mktemp -d "${TMPDIR:-/tmp/}XXXXXXXXXXXX")
#
#echo $tmp
#cp loader.img $tmp/

#( sleep 5 ; kill -9 $(cat $tmp/vm.pid) ; rm -rf $tmp ; reset) &
#
## run qemu in sub-shell
#./qemu-system-i386 \
#	-bios real_mode/src/bin/bios.bin \
#	-m 256 \
#	-smp 1 \
#	-pidfile $tmp/vm.pid \
#	-nographic \
#	-net none \
#	-drive file=real_mode/src/bin/loader.img,format=raw

socat tcp-listen:8086,fork,reuseaddr exec:"python2 run.py",stderr
