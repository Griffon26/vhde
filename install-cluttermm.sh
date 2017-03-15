#!/bin/sh
set -ex
wget https://github.com/GNOME/cluttermm/archive/master.zip -O /tmp/cluttermm.zip
unzip /tmp/cluttermm.zip
mkdir -p $HOME/external
cd cluttermm-master && \
./autogen.sh && \
./configure --prefix=$HOME/external --disable-documentation && \
make && make install
