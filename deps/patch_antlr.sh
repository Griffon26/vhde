#!/bin/bash

set -e

PATCHDIR=$(dirname $0)
RUNTIMEDIR="$1"

# Only execute this move and patch once
if [ -d "${RUNTIMEDIR}/runtime/Cpp" ]; then
  mv "${RUNTIMEDIR}" "${RUNTIMEDIR}_original"
  mv "${RUNTIMEDIR}_original/runtime/Cpp" "${RUNTIMEDIR}"
  cd "${RUNTIMEDIR}"
  patch -p1 < "${PATCHDIR}/antlr4-cpp-runtime-4.7-only-build-shared.patch"
else
  echo "-- Avoiding repeated move & patch, runtime/Cpp has already been moved"
fi

