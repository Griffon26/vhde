#!/bin/bash
set -ex

TMPZIP="/tmp/clutter-gtkmm.zip"
SRCDIR="clutter-gtkmm-CLUTTERMM_1_3_3"
#SRCDIR="clutter-gtkmm-cluttermm-0.8"
#SRCDIR="clutter-gtkmm-master"
INSTALLDIR="${HOME}/.local"

if [ ! -d "${SRCDIR}" ]; then
  #wget https://github.com/GNOME/clutter-gtkmm/archive/master.zip -O "${TMPZIP}"
  wget https://github.com/GNOME/clutter-gtkmm/archive/CLUTTERMM_1_3_3.zip -O "${TMPZIP}"
  #wget https://github.com/GNOME/clutter-gtkmm/archive/cluttermm-0.8.zip -O "${TMPZIP}"
  unzip "${TMPZIP}"
  rm -f "${TMPZIP}"
fi

mkdir -p "${INSTALLDIR}"

pushd "${SRCDIR}" > /dev/null

PATH="${PATH}:${INSTALLDIR}/bin" ACLOCAL_FLAGS="-I ${INSTALLDIR}/share/aclocal" ./autogen.sh --prefix="${INSTALLDIR}" --disable-documentation

set +e
make
sed -i "0,/Texture_Class/s//Texture_Class1/" clutter/cluttermm/wrap_init.cc
set -e

make
make install

popd > /dev/null

set +x
echo "------------------------------------------------------------------------------------------------"
echo "Don't forget to add the following settings to your environment before building and running VHDE:"
echo ""
echo "  export PKG_CONFIG_PATH=\"${INSTALLDIR}/lib/pkgconfig:${INSTALLDIR}/share/pkgconfig\""
echo "  export LD_LIBRARY_PATH=\"\${LD_LIBRARY_PATH}:${INSTALLDIR}/lib\""
echo ""
echo "------------------------------------------------------------------------------------------------"

