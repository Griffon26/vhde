#!/bin/bash

PACKAGE="cluttermm"

set -e

if [ $# -eq 0 ]; then
  TAG="master"
elif [ $# -eq 1 ]; then
  TAG="$1"
else
  echo "Usage: $0 [git_tag]"
  echo ""
  echo "  git_tag: the version of the package to use (must be a git tag or branch)"
  false
fi

set -x

INSTALLDIR="${HOME}/.local"
TMPZIP="/tmp/${PACKAGE}-${TAG}.zip"
SRCDIR="${PACKAGE}-${TAG}"

if [ ! -d "${SRCDIR}" ]; then
  wget https://github.com/GNOME/${PACKAGE}/archive/${TAG}.zip -O "${TMPZIP}"
  unzip -q "${TMPZIP}"
  rm -f "${TMPZIP}"
fi

mkdir -p "${INSTALLDIR}"

pushd "${SRCDIR}" > /dev/null

PATH="${PATH}:${INSTALLDIR}/bin" ACLOCAL_FLAGS="-I ${INSTALLDIR}/share/aclocal" ./autogen.sh --prefix="${INSTALLDIR}" --disable-documentation
#set +e
#make
#sed -i "0,/Texture_Class/s//Texture_Class1/" clutter/cluttermm/wrap_init.cc
#set -e
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

