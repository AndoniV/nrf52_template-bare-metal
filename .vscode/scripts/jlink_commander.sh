#!/bin/bash
# ******************************************************************************
# @copyright © Glue Home Ltd, 2022. All rights reserved.
# @author    A Villarreal
# @date      04/03/22
# @filename  jlink_commander.sh
# @brief     
# @arg       1: J-Link serial number
# ******************************************************************************

echo "********************************************************************************"
echo "Starting JLinkExe SN $1"
JLinkExe -device NRF52 -if SWD -speed 4000 -autoconnect 1 -SelectEmuBySN $1

