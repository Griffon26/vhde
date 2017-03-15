#!/bin/sh
set -ex
wget https://github.com/GNOME/cluttermm/archive/master.zip -O /tmp/cluttermm.zip
unzip /tmp/cluttermm.zip
cd cluttermm-master && ./autogen.sh && ./configure --disable-documentation && make && make install
