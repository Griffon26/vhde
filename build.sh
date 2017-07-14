#!/bin/bash
export CC="gcc-5"
export CXX="g++-5"
export CXXFLAGS="-Wno-deprecated-declarations"
export INSTALLDIR=${HOME}/.local
export PKG_CONFIG_PATH="${INSTALLDIR}/lib/pkgconfig:${INSTALLDIR}/share/pkgconfig"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${INSTALLDIR}/lib"

./install-from-github.sh GNOME mm-common
./install-from-github.sh GNOME cluttermm 1.3.2 -p cluttermm-1.3.2-cairo-texture-namespace-fix.patch
./install-from-github.sh GNOME clutter-gtk 1.5.2
./install-from-github.sh GNOME clutter-gtkmm CLUTTER_GTKMM_0_91_8

cmake .
make
