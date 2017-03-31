#!/bin/bash
set -ex

TMPZIP="/tmp/mmcommon.zip"
SRCDIR="mm-common-master"
INSTALLDIR="${HOME}/.local"

if [ ! -d "${SRCDIR}" ]; then
  wget https://github.com/GNOME/mm-common/archive/master.zip -O "${TMPZIP}"
  unzip "${TMPZIP}"
  rm -f "${TMPZIP}"
fi

mkdir -p "${INSTALLDIR}"

pushd "${SRCDIR}" > /dev/null

./autogen.sh --prefix="${INSTALLDIR}" --enable-network --disable-documentation
make
make install

popd > /dev/null

