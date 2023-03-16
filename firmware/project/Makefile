PROJECT_NAME      := project
SDK_PATH          := ../nrf5_sdk
DEPLOY_PATH       := ../deploy
BOOTLOADER_PATH   := ../bootloader
PROJECT_PATH      := .
SRC_NAME          := src
SRC_PATH          := $(PROJECT_PATH)/$(SRC_NAME)
OUTPUT_FILENAME   := project
GNU_PREFIX        := arm-none-eabi
SDK_CONFIG_FILE   := $(SRC_PATH)/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_PATH)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar

export OUTPUT_FILENAME
MAKEFILE_NAME := $(MAKEFILE_LIST)
MAKEFILE_PATH := $(dir $(MAKEFILE_NAME))

# Allow verbose output
ifeq ("$(VERBOSE)","1")
    NO_ECHO :=
else
    NO_ECHO := @
endif

# Choose target MCU
ifeq ("$(TARGET)","nrf52832")
    LINKER_SCRIPT := nrf52832.ld
else
    ifeq ("$(TARGET)","nrf52840")
        LINKER_SCRIPT := nrf52840.ld
    else
        $(error Bad target MCU, choose either "TARGET=nrf52832" or "TARGET=nrf52840")
    endif
endif

# Define SoftDevice from target
ifeq ("$(TARGET)","nrf52832")
    SOFTDEVICE_TYPE     := s132
    SOFTDEVICE_FILENAME := s132_nrf52_7.2.0_softdevice.hex
else
    SOFTDEVICE_TYPE     := s140
    SOFTDEVICE_FILENAME := s140_nrf52_7.2.0_softdevice.hex
endif

# Define platform-specific details for the build
ifeq ($(OS),Windows_NT)
    TOOLS_PATH       := ../../../nrf52-tools/windows
    GNU_INSTALL_ROOT := $(TOOLS_PATH)/gcc-arm-none-eabi
    GNU_VERSION      := 9.3.1
    MK               := md
    RM               := rd /s /q
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        TOOLS_PATH       := ../../../nrf52-tools/linux
    endif
    ifeq ($(UNAME_S),Darwin)
        TOOLS_PATH       := ../../../nrf52-tools/macos
    endif
    GNU_INSTALL_ROOT := $(TOOLS_PATH)/gcc-arm-none-eabi
    GNU_VERSION      := 9.3.1
    MK               := mkdir
    RM               := rm -rf
endif

# Toolchain commands
AR      := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ar' -r
AS      := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-as'
CC      := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gcc'
LD      := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gcc'
NM      := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-nm'
OBJCOPY := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objcopy'
OBJDUMP := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objdump'
SIZE    := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-size'

# Function for removing duplicates in a list
remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

# C source files
C_SOURCE_FILES += \
        $(SDK_PATH)/components/ble/ble_advertising/ble_advertising.c \
        $(SDK_PATH)/components/ble/common/ble_advdata.c \
        $(SDK_PATH)/components/ble/common/ble_conn_params.c \
        $(SDK_PATH)/components/ble/common/ble_conn_state.c \
        $(SDK_PATH)/components/ble/common/ble_srv_common.c \
        $(SDK_PATH)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
        $(SDK_PATH)/components/ble/nrf_ble_qwr/nrf_ble_qwr.c \
        $(SDK_PATH)/components/ble/ble_services/ble_dfu/ble_dfu.c \
        $(SDK_PATH)/components/ble/ble_services/ble_dfu/ble_dfu_unbonded.c \
        $(SDK_PATH)/components/ble/peer_manager/auth_status_tracker.c \
        $(SDK_PATH)/components/ble/peer_manager/gatt_cache_manager.c \
        $(SDK_PATH)/components/ble/peer_manager/gatts_cache_manager.c \
        $(SDK_PATH)/components/ble/peer_manager/id_manager.c \
        $(SDK_PATH)/components/ble/peer_manager/peer_data_storage.c \
        $(SDK_PATH)/components/ble/peer_manager/peer_database.c \
        $(SDK_PATH)/components/ble/peer_manager/peer_id.c \
        $(SDK_PATH)/components/ble/peer_manager/peer_manager_handler.c \
        $(SDK_PATH)/components/ble/peer_manager/peer_manager.c \
        $(SDK_PATH)/components/ble/peer_manager/pm_buffer.c \
        $(SDK_PATH)/components/ble/peer_manager/security_dispatcher.c \
        $(SDK_PATH)/components/ble/peer_manager/security_manager.c \
        $(SDK_PATH)/components/boards/boards.c \
        $(SDK_PATH)/components/libraries/atomic_fifo/nrf_atfifo.c \
        $(SDK_PATH)/components/libraries/atomic_flags/nrf_atflags.c \
        $(SDK_PATH)/components/libraries/atomic/nrf_atomic.c \
        $(SDK_PATH)/components/libraries/balloc/nrf_balloc.c \
        $(SDK_PATH)/components/libraries/bsp/bsp_btn_ble.c \
        $(SDK_PATH)/components/libraries/bsp/bsp.c \
        $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_svci.c \
        $(SDK_PATH)/components/libraries/button/app_button.c \
        $(SDK_PATH)/components/libraries/crc16/crc16.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_aead.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_aes.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_aes_shared.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_ecc.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_ecdh.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_ecdsa.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_eddsa.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_error.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_hash.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_hkdf.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_hmac.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_init.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_rng.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_shared.c \
        $(SDK_PATH)/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_init.c \
        $(SDK_PATH)/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng.c \
        $(SDK_PATH)/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng_mbedtls.c \
        $(SDK_PATH)/components/libraries/experimental_section_vars/nrf_section_iter.c \
        $(SDK_PATH)/components/libraries/fds/fds.c \
        $(SDK_PATH)/components/libraries/fstorage/nrf_fstorage_sd.c \
        $(SDK_PATH)/components/libraries/fstorage/nrf_fstorage.c \
        $(SDK_PATH)/components/libraries/gpiote/app_gpiote.c \
        $(SDK_PATH)/components/libraries/hardfault/hardfault_implementation.c \
        $(SDK_PATH)/components/libraries/log/src/nrf_log_backend_rtt.c \
        $(SDK_PATH)/components/libraries/log/src/nrf_log_backend_serial.c \
        $(SDK_PATH)/components/libraries/log/src/nrf_log_backend_uart.c \
        $(SDK_PATH)/components/libraries/log/src/nrf_log_default_backends.c \
        $(SDK_PATH)/components/libraries/log/src/nrf_log_frontend.c \
        $(SDK_PATH)/components/libraries/log/src/nrf_log_str_formatter.c \
        $(SDK_PATH)/components/libraries/memobj/nrf_memobj.c \
        $(SDK_PATH)/components/libraries/queue/nrf_queue.c \
        $(SDK_PATH)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
        $(SDK_PATH)/components/libraries/ringbuf/nrf_ringbuf.c \
        $(SDK_PATH)/components/libraries/scheduler/app_scheduler.c \
        $(SDK_PATH)/components/libraries/sensorsim/sensorsim.c \
        $(SDK_PATH)/components/libraries/strerror/nrf_strerror.c \
        $(SDK_PATH)/components/libraries/timer/app_timer.c \
        $(SDK_PATH)/components/libraries/twi_mngr/nrf_twi_mngr.c \
        $(SDK_PATH)/components/libraries/util/app_error_handler_gcc.c \
        $(SDK_PATH)/components/libraries/util/app_error_weak.c \
        $(SDK_PATH)/components/libraries/util/app_error.c \
        $(SDK_PATH)/components/libraries/util/app_util_platform.c \
        $(SDK_PATH)/components/libraries/util/nrf_assert.c \
        $(SDK_PATH)/components/libraries/pwm/app_pwm.c \
        $(SDK_PATH)/components/libraries/spi_mngr/nrf_spi_mngr.c \
        $(SDK_PATH)/components/softdevice/common/nrf_sdh_ble.c \
        $(SDK_PATH)/components/softdevice/common/nrf_sdh_soc.c \
        $(SDK_PATH)/components/softdevice/common/nrf_sdh.c \
        $(SDK_PATH)/external/fprintf/nrf_fprintf_format.c \
        $(SDK_PATH)/external/fprintf/nrf_fprintf.c \
        $(SDK_PATH)/external/segger_rtt/SEGGER_RTT_printf.c \
        $(SDK_PATH)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
        $(SDK_PATH)/external/segger_rtt/SEGGER_RTT.c \
        $(SDK_PATH)/external/utf_converter/utf.c \
        $(SDK_PATH)/integration/nrfx/legacy/nrf_drv_clock.c \
        $(SDK_PATH)/integration/nrfx/legacy/nrf_drv_ppi.c \
        $(SDK_PATH)/integration/nrfx/legacy/nrf_drv_rng.c \
        $(SDK_PATH)/integration/nrfx/legacy/nrf_drv_uart.c \
        $(SDK_PATH)/integration/nrfx/legacy/nrf_drv_twi.c \
        $(SDK_PATH)/integration/nrfx/legacy/nrf_drv_spi.c \
        $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_clock.c \
        $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_gpiote.c \
        $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_power.c \
        $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_ppi.c \
        $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_rng.c \
        $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_saadc.c \
        $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_timer.c \
        $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_twi.c \
        $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_twim.c \
        $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_uart.c \
        $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_uarte.c \
        $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_wdt.c \
        $(SDK_PATH)/modules/nrfx/drivers/src/prs/nrfx_prs.c \
        $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_spi.c \
        $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_spim.c \

ifeq ("$(TARGET)","nrf52832")
C_SOURCE_FILES += \
        $(SDK_PATH)/modules/nrfx/mdk/system_nrf52.c \

else
C_SOURCE_FILES += \
        $(SDK_PATH)/modules/nrfx/mdk/system_nrf52840.c \

endif

C_SOURCE_FILES += \
        $(wildcard $(SDK_PATH)/external/mbedtls/library/*.c) \
        $(SDK_PATH)/external/nrf_tls/mbedtls/replacements/asn1write.c \
        $(SDK_PATH)/external/nrf_tls/mbedtls/replacements/ssl_srv.c \

C_SOURCE_FILES += \
        $(TOOLS_PATH)/jlink_monitor_mode_debug/JLINK_MONITOR.c \

C_SOURCE_FILES += \
        $(SRC_PATH)/main.c \
        $(SRC_PATH)/hal/board.c \
        $(SRC_PATH)/ble/ble_if.c \
        $(SRC_PATH)/ble/proto_ble_profile.c \
        $(SRC_PATH)/app/app.c \
        $(SRC_PATH)/app/device.c \

# Remove unwanted source files
C_SOURCE_FILES := $(filter-out $(SDK_PATH)/external/mbedtls/library/asn1write.c, $(C_SOURCE_FILES))
C_SOURCE_FILES := $(filter-out $(SDK_PATH)/external/mbedtls/library/ssl_srv.c, $(C_SOURCE_FILES))

# Assembly files common to all targets
ASM_SOURCE_FILES += \
        $(TOOLS_PATH)/jlink_monitor_mode_debug/JLINK_MONITOR_ISR_SES.S \

ifeq ("$(TARGET)","nrf52832")
ASM_SOURCE_FILES += \
        $(SDK_PATH)/modules/nrfx/mdk/gcc_startup_nrf52.S \

else
ASM_SOURCE_FILES += \
        $(SDK_PATH)/modules/nrfx/mdk/gcc_startup_nrf52840.S \

endif

# Includes paths common to all targets
INC_PATHS += \
        -I$(NANOPB_PATH) \
        -I$(PROTOCOL_PATH) \
        -I$(TOOLS_PATH)/jlink_monitor_mode_debug \
        -I$(SDK_PATH)/components \
        -I$(SDK_PATH)/components/ble/ble_advertising \
        -I$(SDK_PATH)/components/ble/ble_dtm \
        -I$(SDK_PATH)/components/ble/ble_racp \
        -I$(SDK_PATH)/components/ble/ble_services/ble_ancs_c \
        -I$(SDK_PATH)/components/ble/ble_services/ble_ans_c \
        -I$(SDK_PATH)/components/ble/ble_services/ble_bas \
        -I$(SDK_PATH)/components/ble/ble_services/ble_bas_c \
        -I$(SDK_PATH)/components/ble/ble_services/ble_cscs \
        -I$(SDK_PATH)/components/ble/ble_services/ble_cts_c \
        -I$(SDK_PATH)/components/ble/ble_services/ble_dfu \
        -I$(SDK_PATH)/components/ble/ble_services/ble_dis \
        -I$(SDK_PATH)/components/ble/ble_services/ble_gls \
        -I$(SDK_PATH)/components/ble/ble_services/ble_hids \
        -I$(SDK_PATH)/components/ble/ble_services/ble_hrs \
        -I$(SDK_PATH)/components/ble/ble_services/ble_hrs_c \
        -I$(SDK_PATH)/components/ble/ble_services/ble_hts \
        -I$(SDK_PATH)/components/ble/ble_services/ble_ias \
        -I$(SDK_PATH)/components/ble/ble_services/ble_ias_c \
        -I$(SDK_PATH)/components/ble/ble_services/ble_lbs \
        -I$(SDK_PATH)/components/ble/ble_services/ble_lbs_c \
        -I$(SDK_PATH)/components/ble/ble_services/ble_lls \
        -I$(SDK_PATH)/components/ble/ble_services/ble_nus \
        -I$(SDK_PATH)/components/ble/ble_services/ble_nus_c \
        -I$(SDK_PATH)/components/ble/ble_services/ble_rscs \
        -I$(SDK_PATH)/components/ble/ble_services/ble_rscs_c \
        -I$(SDK_PATH)/components/ble/ble_services/ble_tps \
        -I$(SDK_PATH)/components/ble/common \
        -I$(SDK_PATH)/components/ble/nrf_ble_gatt \
        -I$(SDK_PATH)/components/ble/nrf_ble_qwr \
        -I$(SDK_PATH)/components/ble/peer_manager \
        -I$(SDK_PATH)/components/boards \
        -I$(SDK_PATH)/components/drivers_nrf/usbd \
        -I$(SDK_PATH)/components/libraries/atomic \
        -I$(SDK_PATH)/components/libraries/atomic_fifo \
        -I$(SDK_PATH)/components/libraries/atomic_flags \
        -I$(SDK_PATH)/components/libraries/balloc \
        -I$(SDK_PATH)/components/libraries/bootloader \
        -I$(SDK_PATH)/components/libraries/bootloader/dfu \
        -I$(SDK_PATH)/components/libraries/bootloader/ble_dfu \
        -I$(SDK_PATH)/components/libraries/bsp \
        -I$(SDK_PATH)/components/libraries/button \
        -I$(SDK_PATH)/components/libraries/cli \
        -I$(SDK_PATH)/components/libraries/crc16 \
        -I$(SDK_PATH)/components/libraries/crc32 \
        -I$(SDK_PATH)/components/libraries/crypto \
        -I$(SDK_PATH)/components/libraries/crypto/backend/cc310 \
        -I$(SDK_PATH)/components/libraries/crypto/backend/cc310_bl \
        -I$(SDK_PATH)/components/libraries/crypto/backend/cifra \
        -I$(SDK_PATH)/components/libraries/crypto/backend/mbedtls \
        -I$(SDK_PATH)/components/libraries/crypto/backend/micro_ecc \
        -I$(SDK_PATH)/components/libraries/crypto/backend/nrf_sw \
        -I$(SDK_PATH)/components/libraries/crypto/backend/nrf_hw \
        -I$(SDK_PATH)/components/libraries/crypto/backend/oberon \
        -I$(SDK_PATH)/components/libraries/crypto/backend/optiga \
        -I$(SDK_PATH)/components/libraries/csense \
        -I$(SDK_PATH)/components/libraries/csense_drv \
        -I$(SDK_PATH)/components/libraries/delay \
        -I$(SDK_PATH)/components/libraries/ecc \
        -I$(SDK_PATH)/components/libraries/experimental_section_vars \
        -I$(SDK_PATH)/components/libraries/experimental_task_manager \
        -I$(SDK_PATH)/components/libraries/fds \
        -I$(SDK_PATH)/components/libraries/fstorage \
        -I$(SDK_PATH)/components/libraries/gfx \
        -I$(SDK_PATH)/components/libraries/gpiote \
        -I$(SDK_PATH)/components/libraries/hardfault \
        -I$(SDK_PATH)/components/libraries/hci \
        -I$(SDK_PATH)/components/libraries/led_softblink \
        -I$(SDK_PATH)/components/libraries/log \
        -I$(SDK_PATH)/components/libraries/log/src \
        -I$(SDK_PATH)/components/libraries/mem_manager \
        -I$(SDK_PATH)/components/libraries/memobj \
        -I$(SDK_PATH)/components/libraries/mpu \
        -I$(SDK_PATH)/components/libraries/mutex \
        -I$(SDK_PATH)/components/libraries/pwm \
        -I$(SDK_PATH)/components/libraries/pwr_mgmt \
        -I$(SDK_PATH)/components/libraries/queue \
        -I$(SDK_PATH)/components/libraries/ringbuf \
        -I$(SDK_PATH)/components/libraries/scheduler \
        -I$(SDK_PATH)/components/libraries/sdcard \
        -I$(SDK_PATH)/components/libraries/sensorsim \
        -I$(SDK_PATH)/components/libraries/slip \
        -I$(SDK_PATH)/components/libraries/sortlist \
        -I$(SDK_PATH)/components/libraries/spi_mngr \
        -I$(SDK_PATH)/components/libraries/stack_guard \
        -I$(SDK_PATH)/components/libraries/stack_info \
        -I$(SDK_PATH)/components/libraries/strerror \
        -I$(SDK_PATH)/components/libraries/svc \
        -I$(SDK_PATH)/components/libraries/timer \
        -I$(SDK_PATH)/components/libraries/twi_mngr \
        -I$(SDK_PATH)/components/libraries/twi_sensor \
        -I$(SDK_PATH)/components/libraries/usbd \
        -I$(SDK_PATH)/components/libraries/usbd/class/audio \
        -I$(SDK_PATH)/components/libraries/usbd/class/cdc \
        -I$(SDK_PATH)/components/libraries/usbd/class/cdc/acm \
        -I$(SDK_PATH)/components/libraries/usbd/class/hid \
        -I$(SDK_PATH)/components/libraries/usbd/class/hid/generic \
        -I$(SDK_PATH)/components/libraries/usbd/class/hid/kbd \
        -I$(SDK_PATH)/components/libraries/usbd/class/hid/mouse \
        -I$(SDK_PATH)/components/libraries/usbd/class/msc \
        -I$(SDK_PATH)/components/libraries/util \
        -I$(SDK_PATH)/components/nfc/ndef/conn_hand_parser \
        -I$(SDK_PATH)/components/nfc/ndef/conn_hand_parser/ac_rec_parser \
        -I$(SDK_PATH)/components/nfc/ndef/conn_hand_parser/ble_oob_advdata_parser \
        -I$(SDK_PATH)/components/nfc/ndef/conn_hand_parser/le_oob_rec_parser \
        -I$(SDK_PATH)/components/nfc/ndef/connection_handover/ac_rec \
        -I$(SDK_PATH)/components/nfc/ndef/connection_handover/ble_oob_advdata \
        -I$(SDK_PATH)/components/nfc/ndef/connection_handover/ble_pair_lib \
        -I$(SDK_PATH)/components/nfc/ndef/connection_handover/ble_pair_msg \
        -I$(SDK_PATH)/components/nfc/ndef/connection_handover/common \
        -I$(SDK_PATH)/components/nfc/ndef/connection_handover/ep_oob_rec \
        -I$(SDK_PATH)/components/nfc/ndef/connection_handover/hs_rec \
        -I$(SDK_PATH)/components/nfc/ndef/connection_handover/le_oob_rec \
        -I$(SDK_PATH)/components/nfc/ndef/generic/message \
        -I$(SDK_PATH)/components/nfc/ndef/generic/record \
        -I$(SDK_PATH)/components/nfc/ndef/launchapp \
        -I$(SDK_PATH)/components/nfc/ndef/parser/message \
        -I$(SDK_PATH)/components/nfc/ndef/parser/record \
        -I$(SDK_PATH)/components/nfc/ndef/text \
        -I$(SDK_PATH)/components/nfc/ndef/uri \
        -I$(SDK_PATH)/components/nfc/t2t_lib \
        -I$(SDK_PATH)/components/nfc/t2t_lib/hal_t2t \
        -I$(SDK_PATH)/components/nfc/t2t_parser \
        -I$(SDK_PATH)/components/nfc/t4t_lib \
        -I$(SDK_PATH)/components/nfc/t4t_lib/hal_t4t \
        -I$(SDK_PATH)/components/nfc/t4t_parser/apdu \
        -I$(SDK_PATH)/components/nfc/t4t_parser/cc_file \
        -I$(SDK_PATH)/components/nfc/t4t_parser/hl_detection_procedure \
        -I$(SDK_PATH)/components/nfc/t4t_parser/tlv \
        -I$(SDK_PATH)/components/softdevice/common \
        -I$(SDK_PATH)/components/toolchain/cmsis/include \
        -I$(SDK_PATH)/external/fprintf \
        -I$(SDK_PATH)/external/mbedtls/include \
        -I$(SDK_PATH)/external/segger_rtt \
        -I$(SDK_PATH)/external/utf_converter \
        -I$(SDK_PATH)/integration/nrfx \
        -I$(SDK_PATH)/integration/nrfx/legacy \
        -I$(SDK_PATH)/modules/nrfx \
        -I$(SDK_PATH)/modules/nrfx/drivers/include \
        -I$(SDK_PATH)/modules/nrfx/hal \
        -I$(SDK_PATH)/modules/nrfx/mdk \
        -I$(SRC_PATH) \
        -I$(SRC_PATH)/app \
        -I$(SRC_PATH)/ble \
        -I$(SRC_PATH)/hal \
        -I$(SRC_PATH)/protocol \
        -I$(SRC_PATH)/security \
        -I$(SRC_PATH)/test \
        -I$(SRC_PATH)/utils \

ifeq ("$(TARGET)","nrf52832")
INC_PATHS += \
        -I$(SDK_PATH)/components/softdevice/s132/headers \
        -I$(SDK_PATH)/components/softdevice/s132/headers/nrf52 \

else
INC_PATHS += \
        -I$(SDK_PATH)/components/softdevice/s140/headers \
        -I$(SDK_PATH)/components/softdevice/s140/headers/nrf52 \

endif

OBJECT_DIRECTORY = build
LISTING_DIRECTORY = $(OBJECT_DIRECTORY)
OUTPUT_BINARY_DIRECTORY = $(OBJECT_DIRECTORY)

# Sorting removes duplicates
BUILD_DIRECTORIES := $(sort $(OBJECT_DIRECTORY) $(OUTPUT_BINARY_DIRECTORY) $(LISTING_DIRECTORY) )

C_SOURCE_FILE_NAMES = $(notdir $(C_SOURCE_FILES))
C_PATHS = $(call remduplicates, $(dir $(C_SOURCE_FILES) ) )
C_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(C_SOURCE_FILE_NAMES:.c=.o) )

ASM_SOURCE_FILE_NAMES = $(notdir $(ASM_SOURCE_FILES))
ASM_PATHS = $(call remduplicates, $(dir $(ASM_SOURCE_FILES) ))
ASM_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(ASM_SOURCE_FILE_NAMES:.S=.o) )

vpath %.c $(C_PATHS)
vpath %.S $(ASM_PATHS)

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

# Compiler flags
ifeq ("$(TARGET)","nrf52832")
CFLAGS += -DBOARD_PCA10040
CFLAGS += -DNRF52832_XXAA
CFLAGS += -DNRF52
CFLAGS += -DNRF52_PAN_74
CFLAGS += -DS132
else
CFLAGS += -DBOARD_PCA10056
CFLAGS += -DNRF52840_XXAA
CFLAGS += -DS140
endif

CFLAGS += -D__HEAP_SIZE=8192
CFLAGS += -D__STACK_SIZE=8192
CFLAGS += -DBL_SETTINGS_ACCESS_ONLY
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DMBEDTLS_CONFIG_FILE=\"$(abspath $(SRC_NAME)/mbedtls_config.h)\"
CFLAGS += -DNRF_CRYPTO_MAX_INSTANCE_COUNT=1
CFLAGS += -DNRF_DFU_SVCI_ENABLED
CFLAGS += -DNRF_DFU_TRANSPORT_BLE=1
CFLAGS += -DNRF_SD_BLE_API_VERSION=6
CFLAGS += -DPB_FIELD_16BIT
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DSWI_DISABLE0
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Wno-unused-variable

# Linker flags
LDFLAGS += --specs=nano.specs
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_PATH)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Xlinker -Map=$(LISTING_DIRECTORY)/$(OUTPUT_FILENAME).map

# Assembler flags
ifeq ("$(TARGET)","nrf52832")
ASMFLAGS += -DBOARD_PCA10040
ASMFLAGS += -DNRF52832_XXAA
ASMFLAGS += -DNRF52
ASMFLAGS += -DNRF52_PAN_74
ASMFLAGS += -DS132
else
ASMFLAGS += -DBOARD_PCA10056
ASMFLAGS += -DNRF52840_XXAA
ASMFLAGS += -DS140
endif

ASMFLAGS += -D__HEAP_SIZE=8192
ASMFLAGS += -D__STACK_SIZE=8192
ASMFLAGS += -DBL_SETTINGS_ACCESS_ONLY
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DNRF_CRYPTO_MAX_INSTANCE_COUNT=1
ASMFLAGS += -DNRF_DFU_SVCI_ENABLED
ASMFLAGS += -DNRF_DFU_TRANSPORT_BLE=1
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=6
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -mthumb -mabi=aapcs

# Libraries
LIBS += -lm
LIBS += -lc
LIBS += -lnosys

# RULES ***********************************************************************/

# Default build target
.PHONY: all
all: debug

# Clean
.PHONY clean:
ifneq ($(wildcard $(BUILD_DIRECTORIES)),)
clean:
	$(RM) $(BUILD_DIRECTORIES)
endif

# Debug, defaulting to monitor mode
.PHONY: debug
debug: clean
debug: ASMFLAGS += -DDEBUG -g2 -O0
debug: CFLAGS += -DDEBUG -g2 -O0
debug: CFLAGS += -DDEBUG_NRF=1
debug: CFLAGS += -DNRF_LOG_USES_RTT=1
debug: CFLAGS += -DMMD=1
debug: CFLAGS += -DDEBUG_MONITOR_MODE=1
debug: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out

# Debug using monitor mode explicitly
.PHONY: debug_monitor_mode
debug_monitor_mode: debug

# Debug using halt (normal) mode. This is necessary to allow GPIO interrupts
.PHONY: debug_halt_mode
debug_halt_mode: clean
debug_halt_mode: ASMFLAGS += -DDEBUG -g2 -O0
debug_halt_mode: CFLAGS += -DDEBUG -g2 -O0
debug_halt_mode: CFLAGS += -DDEBUG_NRF=1
debug_halt_mode: CFLAGS += -DNRF_LOG_USES_RTT=1
debug_halt_mode: CFLAGS += -DDEBUG_MONITOR_MODE=0
debug_halt_mode: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out

# Release
.PHONY: release
release: clean
release: ASMFLAGS += -DNDEBUG -O0
release: CFLAGS += -DNDEBUG -O0
release: CFLAGS += -DNRF_LOG_ENABLED=1
release: CFLAGS += -DNRF_LOG_USES_RTT=1
release: CFLAGS += -DNRF_LOG_DEFAULT_LEVEL=3
release: CFLAGS += -DSEGGER_RTT_CONFIG_DEFAULT_MODE=0
release: LDFLAGS += -O2
release: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out

# Create build directories
$(BUILD_DIRECTORIES):
	$(MK) $@

# Compile C files
$(OBJECT_DIRECTORY)/%.o: %.c
	@echo COMPILING: $(notdir $<)
	$(NO_ECHO)$(CC) $(CFLAGS) $(INC_PATHS) -c -o $@ $<

# Assemble
$(OBJECT_DIRECTORY)/%.o: %.S
	@echo ASSEMBLING: $(notdir $<)
	$(NO_ECHO)$(CC) $(ASMFLAGS) $(INC_PATHS) -c -o $@ $<

# Link
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out: $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo LINKING: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(LD) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_PATH) -e finalise

# Finalise the build
finalise: genbin genhex echosize

# Create .bin file from .out file
genbin:
	@echo GENERATING IMAGE: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

# Create .hex file from .out file
genhex:
	@echo GENERATING IMAGE: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

# Print size
echosize:
	-@echo ''
	$(NO_ECHO)$(SIZE) $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	-@echo ''

# Flash firmware
.PHONY: flash
flash:
	@echo FLASHING FIRMWARE: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex
	nrfjprog --program $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex -f nrf52 --sectorerase
	nrfjprog --reset -f nrf52

# Flash SoftDevice
.PHONY: flash_softdevice
flash_softdevice:
	@echo FLASHING SOFTDEVICE: $(SOFTDEVICE_FILENAME)
	nrfjprog --program $(SDK_PATH)/components/softdevice/$(SOFTDEVICE_TYPE)/hex/$(SOFTDEVICE_FILENAME) -f nrf52 --sectorerase
	nrfjprog --reset -f nrf52

# Build and flash pre-built MM debug firmware with the bootloader installed
.PHONY: deploy_debug
deploy_debug: debug_monitor_mode
	@echo GENERATING BOOTLOADER SETTINGS
	nrfutil settings generate --family NRF52 --application $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex --application-version 2 --bootloader-version 2 --bl-settings-version 2 $(DEPLOY_PATH)/bootloader_settings.hex
	@echo MERGING APPLICATION AND BOOTLOADER SETTINGS
	mergehex -m $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex $(DEPLOY_PATH)/bootloader_settings.hex -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME)_merged.hex
	nrfjprog --eraseall -f nrf52
	@echo FLASHING SOFTDEVICE: $(SOFTDEVICE_FILENAME)
	nrfjprog --program $(SDK_PATH)/components/softdevice/$(SOFTDEVICE_TYPE)/hex/$(SOFTDEVICE_FILENAME) -f nrf52 --verify
	@echo FLASHING FIRMWARE: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex
	nrfjprog --program $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME)_merged.hex -f nrf52 --verify
	@echo FLASHING BOOTLOADER: $(BOOTLOADER_PATH)/build/release/bootloader.hex
	nrfjprog --program $(BOOTLOADER_PATH)/build/release/bootloader.hex -f nrf52 --verify
	nrfjprog --reset -f nrf52

# Alias for deploy_debug
.PHONY: deploy_debug_monitor_mode
deploy_debug_monitor_mode: deploy_debug

# Build and flash pre-built HM debug firmware with the bootloader installed
.PHONY: deploy_debug_halt_mode
deploy_debug_halt_mode: debug_halt_mode
	@echo GENERATING BOOTLOADER SETTINGS
	nrfutil settings generate --family NRF52 --application $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex --application-version 2 --bootloader-version 2 --bl-settings-version 2 $(DEPLOY_PATH)/bootloader_settings.hex
	@echo MERGING APPLICATION AND BOOTLOADER SETTINGS
	mergehex -m $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex $(DEPLOY_PATH)/bootloader_settings.hex -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME)_merged.hex
	nrfjprog --eraseall -f nrf52
	@echo FLASHING SOFTDEVICE: $(SOFTDEVICE_FILENAME)
	nrfjprog --program $(SDK_PATH)/components/softdevice/$(SOFTDEVICE_TYPE)/hex/$(SOFTDEVICE_FILENAME) -f nrf52 --verify
	@echo FLASHING FIRMWARE: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex
	nrfjprog --program $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME)_merged.hex -f nrf52 --verify
	@echo FLASHING BOOTLOADER: $(BOOTLOADER_PATH)/build/release/bootloader.hex
	nrfjprog --program $(BOOTLOADER_PATH)/build/release/bootloader.hex -f nrf52 --verify
	nrfjprog --reset -f nrf52

# Write the bootloader start and end addresses
.PHONY: write_bootloader_addr
write_bootloader_addr:
	nrfjprog -f nrf52 --memwr 0x10001014 --val 0x00078000
	nrfjprog -f nrf52 --memwr 0x10001018 --val 0x0007E000

# Erase nRF52
erase:
	nrfjprog -f nrf52 --eraseall

# Run the SDK configuration tool
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
