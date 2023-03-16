#!/bin/bash
# ******************************************************************************
# @author    A Villarreal
# @date      22/04/21
# @filename  build_bootloader.sh
# @brief     Build the bootloader
# @arg       1: "nrf52832" or "nrf52840"
# ******************************************************************************

WORKING_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null && pwd)"

if [[ "$1" = "nrf52832" ]]; then
    echo "Target is nrf52832"
    TARGET="TARGET=nrf52832"
elif [[ "$1" = "nrf52840" ]]; then
    echo "Target is nrf52840"
    TARGET="TARGET=nrf52840"
else
    echo "Unrecognised target"
    exit 1
fi

echo "********************************************************************************"
cd ../bootloader
echo "Building bootloader..."
make ${TARGET} release
cd ${WORKING_DIR}
