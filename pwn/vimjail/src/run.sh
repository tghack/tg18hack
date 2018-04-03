#!/bin/bash

set -eux

apt install -y git lua5.2 liblua5.2-dev libncurses5-dev
git clone https://github.com/vim/vim.git
cd vim
./configure --enable-luainterp
make -j4
make install

useradd -m flag
cp flag_reader flag.txt /home/flag
chown flag:flag /home/flag/flag{.txt,_reader}
chmod 701 /home/flag/flag_reader
chmod +s /home/flag/flag_reader
chmod 700 /home/flag/flag.txt

chsh -s /usr/bin/shell tghack
