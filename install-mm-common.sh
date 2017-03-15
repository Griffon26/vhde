#!/bin/sh
set -ex
wget https://github.com/GNOME/mm-common/archive/master.zip -O /tmp/mmcommon.zip
unzip /tmp/mmcommon.zip
mkdir -p $HOME/external
cd mm-common-master && \
./autogen.sh && \
./configure --prefix=$HOME/external --enable-network --disable-documentation && \
make && make install
