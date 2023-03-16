#!/bin/bash
# ******************************************************************************
# @copyright © DeezNutz 2021. All rights reserved.
# @author    E King
# @date      18/03/19
# @filename  create_dfu_package.sh
# @brief     Create a DFU package for the device firmware application
# @arg       1: "nrf52832" or "nrf52840"
# @arg       2: "complete" for a package with the bootloader and SoftDevice too
# ******************************************************************************

WORKING_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null && pwd)"

if [[ "$1" = "nrf52832" ]]; then
    echo "Target is: nrf52832"
    SD="../nrf5_sdk/components/softdevice/s132/hex/s132_nrf52_7.2.0_softdevice.hex"
    SD_REQ="0xAF,0x0101"
    SD_ID="0x0101"
elif [[ "$1" = "nrf52840" ]]; then
    echo "Target is: nrf52840"
    SD="../nrf5_sdk/components/softdevice/s140/hex/s140_nrf52_7.2.0_softdevice.hex"
    SD_REQ="0xAE,0x0100"
    SD_ID="0x0100"
else
    echo "Unrecognised device"
    exit 1
fi

echo "********************************************************************************"
if [[ "$2" = "complete" ]]; then
    echo "Creating a complete DFU package containing the bootloader, SoftDevice and the app..."
    nrfutil pkg generate \
            --hw-version 52 \
            --application-version 2 \
            --application ../project/build/project.hex \
            --softdevice $SD \
            --sd-req $SD_REQ \
            --sd-id $SD_ID \
            --bootloader-version 2 \
            --bootloader ../bootloader/build/bootloader_nrf5280.hex \
            --key-file ../bootloader/keys/dfu_private.key \
            dfu_package.zip

else
    echo "Creating a DFU package containing just the app..."
    nrfutil pkg generate \
            --hw-version 52 \
            --application-version 2 \
            --application ../project/build/project.hex \
            --sd-req $SD_REQ \
            --key-file ../bootloader/keys/dfu_private.key \
            dfu_package.zip
fi
