#!/bin/bash

PROJECT="$1"; shift
PACKAGE="$1"; shift
if [ "$1" != "-p" ]; then
  VERSION="$1"; shift
fi
if [ "$1" == "-p" ]; then
  PATCH="$2"
  shift; shift
fi
REMAININGARGS="$*"

if [ -z "${VERSION}" ]; then
  VERSION="master"
fi

set -e

if [[ -z "${PROJECT}" || -z "${PACKAGE}" || -n "${REMAININGARGS}" ]]; then
  echo "Usage: $0 <project> <package> [version] [-p <patch>]"
  echo ""
  echo "  project    : github project name"
  echo "  package    : github repository name"
  echo "  version    : an optional version of the package to use (must be a tag or branch). Defaults to \"master\""
  echo "  -p <patch> : an optional patch that will be applied to the source code before the build"
  false
fi

echo "-----------------------------------------------------------------------------------------------------"
echo "Installing ${PROJECT}/${PACKAGE} version ${VERSION} from github"
if [ -n "${PATCH}" ]; then
  echo "  patch: ${PATCH}"
fi
echo "-----------------------------------------------------------------------------------------------------"

set -x

INSTALLDIR="${HOME}/.local"
TMPZIP="/tmp/${PACKAGE}-${VERSION}.zip"
SRCDIR="${PACKAGE}-${VERSION}"

if [ ! -d "${SRCDIR}" ]; then
  wget "https://github.com/${PROJECT}/${PACKAGE}/archive/${VERSION}.zip" -O "${TMPZIP}"
  unzip -q "${TMPZIP}"
  rm -f "${TMPZIP}"
  if [ -n "${PATCH}" ]; then
    pushd "${SRCDIR}" > /dev/null
    patch -p1 < "../${PATCH}"
    popd > /dev/null
  fi
fi

# Create this directory so aclocal will not complain when we use it
mkdir -p "${INSTALLDIR}/share/aclocal"

pushd "${SRCDIR}" > /dev/null

PATH="${PATH}:${INSTALLDIR}/bin" ACLOCAL_FLAGS="-I ${INSTALLDIR}/share/aclocal" ./autogen.sh --prefix="${INSTALLDIR}" --disable-documentation

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

