#!/bin/bash
# ******************************************************************************
# @author    A Villarreal
# @date      22/04/21
# @filename  build_firmware.sh
# @brief     Build the firmware with the bootloader installed
# @arg       1: "nrf52832" or "nrf52840"
# @arg       2: "debug" to build for debug, otherwise build for release
# ******************************************************************************

WORKING_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null && pwd)"

if [[ "$1" = "nrf52832" ]]; then
    echo "Target is nrf52832"
    TARGET="TARGET=nrf52832"
    FAMILY=NRF52
elif [[ "$1" = "nrf52840" ]]; then
    echo "Target is nrf52840"
    TARGET="TARGET=nrf52840"
    FAMILY=NRF52840
else
    echo "Unrecognised target"
    exit 1
fi

echo "********************************************************************************"
cd ../project
if [[ "$2" = "debug" ]]; then
    echo "Building debug application..."
    make ${TARGET} debug
else
    echo "Building release application..."
    make ${TARGET} release
fi
cd ${WORKING_DIR}

echo "********************************************************************************"
echo "Generating bootloader settings page..."
nrfutil settings generate \
        --family ${FAMILY} \
        --application ../project/build/project.hex \
        --application-version 1 \
        --bootloader-version 1 \
        --bl-settings-version 2 \
        bootloader_settings.hex

echo "********************************************************************************"
echo "Merging bootloader settings page with application image..."
APPLICATION_FILEPATH="./project_merged.hex"
mergehex -m ../project/build/project.hex bootloader_settings.hex \
        -o ${APPLICATION_FILEPATH}
