#!/bin/bash
ARCH="x64"
MSVC_WINEPREFIX="${HOME}/.wine-msvc"
MSVC_ROOT="${HOME}/packages/msvc-wine/msvc"
MSVC_BIN="${MSVC_ROOT}/bin/${ARCH}"
PATH=$PATH:$MSVC_BIN
MSBUILD_FLAGS="/nologo /v:m /clp:ForceConsoleColor"
PROPERTIES="-p:Configuration=Release -p:Platform=x64"
PROJECT_FILE="KGlab" # vcxproj dir
msbuild $MSBUILD_FLAGS $PROPERTIES $PROJECT_FILE $@ 2>&1 | tee
exit ${PIPESTATUS[0]}
