#!/bin/bash
# ******************************************************************************
# @author    A Villarreal
# @filename  deploy_firmware.sh
# @brief     Deploy the pre-built device firmware with the bootloader installed.
# @arg       1: "nrf52832" or "nrf52840"
# @arg       2: J-Link serial number
# ******************************************************************************

if [[ "$1" = "nrf52832" ]]; then
    SOFTDEVICE_FILEPATH="../nrf5_sdk/components/softdevice/s132/hex/s132_nrf52_7.2.0_softdevice.hex"
    BOOTLOADER_FILE="bootloader_nrf52832.hex"
elif [[ "$1" = "nrf52840" ]]; then
    SOFTDEVICE_FILEPATH="../nrf5_sdk/components/softdevice/s140/hex/s140_nrf52_7.2.0_softdevice.hex"
    BOOTLOADER_FILE="bootloader_nrf52840.hex"
else
    echo "Unrecognised device"
    exit 1
fi

if [[ "$2" != "" ]]; then
    echo "JLink is $2"
    JLINK="-s $2"
else
    JLINK=""
fi

echo "********************************************************************************"
echo "Erasing nRF52..."
nrfjprog \
        -f nrf52 \
        $JLINK \
        --eraseall

echo "********************************************************************************"
echo "Flashing SoftDevice..."
chmod 777 ${SOFTDEVICE_FILEPATH}
nrfjprog \
        -f nrf52 \
        $JLINK \
        --program ${SOFTDEVICE_FILEPATH} \
        --verify

echo "********************************************************************************"
echo "Flashing merged application image..."
APPLICATION_FILEPATH="./project_merged.hex"
chmod 777 ${APPLICATION_FILEPATH}
nrfjprog \
        -f nrf52 \
        $JLINK \
        --program ${APPLICATION_FILEPATH} \
        --verify

echo "********************************************************************************"
echo "Flashing bootloader..."
BOOTLOADER_FILEPATH="../bootloader/build/${BOOTLOADER_FILE}"
chmod 777 ${BOOTLOADER_FILEPATH}
nrfjprog \
        -f nrf52 \
        $JLINK \
        --program ${BOOTLOADER_FILEPATH} \
        --verify

echo "********************************************************************************"
echo "Resetting nRF52..."
nrfjprog \
        -f nrf52 \
        $JLINK \
        --reset
