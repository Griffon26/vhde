#!/bin/bash
export CC="gcc-5"
export CXX="g++-5"
export INSTALLDIR=${HOME}/.local
export PKG_CONFIG_PATH="${INSTALLDIR}/lib/pkgconfig:${INSTALLDIR}/share/pkgconfig"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${INSTALLDIR}/lib"

./install-mm-common.sh
./install-cluttermm.sh
./install-clutter-gtk.sh
./install-clutter-gtkmm.sh

cmake .
make
