#!/bin/bash
WINEPREFIX="${HOME}/.wine-msvc"
./build.sh && WINEPREFIX=$WINEPREFIX wine ./KGlab/build/Release\ x64/KGlab.exe
