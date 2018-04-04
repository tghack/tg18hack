#!/bin/bash

git clone git://git.seabios.org/seabios.git seabios
cd seabios
patch -p1 < ../boot.patch
make
cd ..

echo "yay, now run \"make loader\" and test with \"make run\""
