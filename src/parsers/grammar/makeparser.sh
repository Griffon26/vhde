#!/bin/sh

set -ex

SCRIPTDIR="$(dirname "$0")"

die() {
  echo "ERROR: $*" >> /dev/stderr
  exit -1
}

[ -e "${SCRIPTDIR}/vhdl.g4" ] || die "Can't find vhdl.g4 in ${SCRIPTDIR}"
cd "${SCRIPTDIR}" || die "Unable to change directory to ${SCRIPTDIR}"
antlr4 -o "../generated" -visitor -no-listener -Dlanguage=Cpp vhdl.g4 || die "Parser generation failed"

