PROJECT_NAME      := bootloader
SDK_PATH          := ../nrf5_sdk
PROJECT_PATH      := .
SRC_NAME          := src
KEY_NAME          := keys
SRC_PATH          := $(PROJECT_PATH)/$(SRC_NAME)
KEY_PATH          := $(PROJECT_PATH)/$(KEY_NAME)
GNU_PREFIX        := arm-none-eabi
SDK_CONFIG_FILE   := $(SRC_PATH)/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_PATH)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar

# Choose output file name
ifeq ("$(TARGET)","nrf52832")
    OUTPUT_FILENAME   := bootloader_nrf52832
else
    ifeq ("$(TARGET)","nrf52840")
        OUTPUT_FILENAME   := bootloader_nrf52840
    else
        $(error Bad target MCU, choose either "TARGET=nrf52832" or "TARGET=nrf52840")
    endif
endif

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

C_SOURCE_FILES += \
        $(SDK_PATH)/components/libraries/log/src/nrf_log_frontend.c \
        $(SDK_PATH)/components/libraries/log/src/nrf_log_str_formatter.c \
        $(SDK_PATH)/components/libraries/util/app_error_weak.c \
        $(SDK_PATH)/components/libraries/scheduler/app_scheduler.c \
        $(SDK_PATH)/components/libraries/util/app_util_platform.c \
        $(SDK_PATH)/components/libraries/crc32/crc32.c \
        $(SDK_PATH)/components/libraries/mem_manager/mem_manager.c \
        $(SDK_PATH)/components/libraries/util/nrf_assert.c \
        $(SDK_PATH)/components/libraries/atomic_fifo/nrf_atfifo.c \
        $(SDK_PATH)/components/libraries/atomic/nrf_atomic.c \
        $(SDK_PATH)/components/libraries/balloc/nrf_balloc.c \
        $(SDK_PATH)/external/fprintf/nrf_fprintf.c \
        $(SDK_PATH)/external/fprintf/nrf_fprintf_format.c \
        $(SDK_PATH)/components/libraries/fstorage/nrf_fstorage.c \
        $(SDK_PATH)/components/libraries/fstorage/nrf_fstorage_nvmc.c \
        $(SDK_PATH)/components/libraries/fstorage/nrf_fstorage_sd.c \
        $(SDK_PATH)/components/libraries/memobj/nrf_memobj.c \
        $(SDK_PATH)/components/libraries/queue/nrf_queue.c \
        $(SDK_PATH)/components/libraries/ringbuf/nrf_ringbuf.c \
        $(SDK_PATH)/components/libraries/experimental_section_vars/nrf_section_iter.c \
        $(SDK_PATH)/components/libraries/strerror/nrf_strerror.c \
        $(SDK_PATH)/components/boards/boards.c \
        $(SDK_PATH)/components/softdevice/common/nrf_sdh.c \
        $(SDK_PATH)/components/softdevice/common/nrf_sdh_ble.c \
        $(SDK_PATH)/components/softdevice/common/nrf_sdh_soc.c \
        $(SDK_PATH)/modules/nrfx/hal/nrf_nvmc.c \
        $(SDK_PATH)/modules/nrfx/soc/nrfx_atomic.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_ecc.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_ecdsa.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_hash.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_init.c \
        $(SDK_PATH)/components/libraries/crypto/nrf_crypto_shared.c \
        $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_svci.c \
        $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_svci_handler.c \
        $(SDK_PATH)/components/libraries/svc/nrf_svc_handler.c \
        $(SDK_PATH)/components/ble/common/ble_srv_common.c \
        $(SDK_PATH)/components/libraries/bootloader/nrf_bootloader.c \
        $(SDK_PATH)/components/libraries/bootloader/nrf_bootloader_app_start.c \
        $(SDK_PATH)/components/libraries/bootloader/nrf_bootloader_app_start_final.c \
        $(SDK_PATH)/components/libraries/bootloader/nrf_bootloader_dfu_timers.c \
        $(SDK_PATH)/components/libraries/bootloader/nrf_bootloader_fw_activation.c \
        $(SDK_PATH)/components/libraries/bootloader/nrf_bootloader_info.c \
        $(SDK_PATH)/components/libraries/bootloader/nrf_bootloader_wdt.c \
        $(SDK_PATH)/external/nano-pb/pb_common.c \
        $(SDK_PATH)/external/nano-pb/pb_decode.c \
        $(SDK_PATH)/components/libraries/bootloader/dfu/dfu-cc.pb.c \
        $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu.c \
        $(SDK_PATH)/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c \
        $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_flash.c \
        $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_handling_error.c \
        $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_mbr.c \
        $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c \
        $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_settings.c \
        $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_settings_svci.c \
        $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_transport.c \
        $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_utils.c \
        $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_validation.c \
        $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_ver_validation.c \
        $(SDK_PATH)/components/libraries/crypto/backend/oberon/oberon_backend_chacha_poly_aead.c \
        $(SDK_PATH)/components/libraries/crypto/backend/oberon/oberon_backend_ecc.c \
        $(SDK_PATH)/components/libraries/crypto/backend/oberon/oberon_backend_ecdh.c \
        $(SDK_PATH)/components/libraries/crypto/backend/oberon/oberon_backend_ecdsa.c \
        $(SDK_PATH)/components/libraries/crypto/backend/oberon/oberon_backend_eddsa.c \
        $(SDK_PATH)/components/libraries/crypto/backend/oberon/oberon_backend_hash.c \
        $(SDK_PATH)/components/libraries/crypto/backend/oberon/oberon_backend_hmac.c \

# C source files specific to target
ifeq ("$(TARGET)","nrf52832")
C_SOURCE_FILES += \
        $(SDK_PATH)/components/libraries/sha256/sha256.c \
        $(SDK_PATH)/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecc.c \
        $(SDK_PATH)/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecdh.c \
        $(SDK_PATH)/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecdsa.c \
        $(SDK_PATH)/components/libraries/crypto/backend/nrf_sw/nrf_sw_backend_hash.c \

else
C_SOURCE_FILES += \
        $(SDK_PATH)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_ecc.c \
        $(SDK_PATH)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_ecdsa.c \
        $(SDK_PATH)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_hash.c \
        $(SDK_PATH)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_init.c \
        $(SDK_PATH)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_shared.c \

endif

ifeq ("$(TARGET)","nrf52832")
C_SOURCE_FILES += \
        $(SDK_PATH)/modules/nrfx/mdk/system_nrf52.c \

else
C_SOURCE_FILES += \
        $(SDK_PATH)/modules/nrfx/mdk/system_nrf52840.c \

endif

C_SOURCE_FILES += \
        $(KEY_PATH)/dfu_public_key.c \
        $(SRC_PATH)/main.c \

# Assembly files
ifeq ("$(TARGET)","nrf52832")
ASM_SOURCE_FILES += \
        $(SDK_PATH)/modules/nrfx/mdk/gcc_startup_nrf52.S \

else
ASM_SOURCE_FILES += \
        $(SDK_PATH)/modules/nrfx/mdk/gcc_startup_nrf52840.S \

endif

# Includes paths common to all targets
INC_PATHS += \
        -I$(SDK_PATH)/components/libraries/crypto/backend/micro_ecc \
        -I$(SDK_PATH)/components/libraries/memobj \
        -I$(SDK_PATH)/components/libraries/crc32 \
        -I$(SDK_PATH)/components/libraries/experimental_section_vars \
        -I$(SDK_PATH)/components/libraries/mem_manager \
        -I$(SDK_PATH)/components/libraries/fstorage \
        -I$(SDK_PATH)/components/libraries/util \
        -I$(SDK_PATH)/modules/nrfx \
        -I$(SDK_PATH)/external/nrf_oberon/include \
        -I$(SDK_PATH)/components/libraries/crypto/backend/oberon \
        -I$(SDK_PATH)/components/libraries/crypto/backend/cifra \
        -I$(SDK_PATH)/components/libraries/atomic \
        -I$(SDK_PATH)/integration/nrfx \
        -I$(SDK_PATH)/components/libraries/crypto/backend/cc310_bl \
        -I$(SDK_PATH)/components/libraries/log/src \
        -I$(SDK_PATH)/components/libraries/bootloader/dfu \
        -I$(SDK_PATH)/components/ble/common \
        -I$(SDK_PATH)/components/libraries/delay \
        -I$(SDK_PATH)/components/libraries/svc \
        -I$(SDK_PATH)/components/libraries/stack_info \
        -I$(SDK_PATH)/components/libraries/crypto/backend/nrf_hw \
        -I$(SDK_PATH)/components/libraries/log \
        -I$(SDK_PATH)/external/nrf_oberon \
        -I$(SDK_PATH)/components/libraries/strerror \
        -I$(SDK_PATH)/components/libraries/crypto/backend/mbedtls \
        -I$(SDK_PATH)/components/boards \
        -I$(SDK_PATH)/components/libraries/crypto/backend/cc310 \
        -I$(SDK_PATH)/components/libraries/bootloader \
        -I$(SDK_PATH)/components/libraries/crypto \
        -I$(SDK_PATH)/components/libraries/crypto/backend/optiga \
        -I$(SDK_PATH)/components/libraries/scheduler \
        -I$(SDK_PATH)/external/nrf_cc310_bl/include \
        -I$(SDK_PATH)/modules/nrfx/hal \
        -I$(SDK_PATH)/components/toolchain/cmsis/include \
        -I$(SDK_PATH)/components/libraries/balloc \
        -I$(SDK_PATH)/components/libraries/atomic_fifo \
        -I$(SDK_PATH)/components/libraries/crypto/backend/nrf_sw \
        -I$(SDK_PATH)/modules/nrfx/mdk \
        -I$(SDK_PATH)/components/libraries/bootloader/ble_dfu \
        -I$(SDK_PATH)/components/softdevice/common \
        -I$(SDK_PATH)/external/nano-pb \
        -I$(SDK_PATH)/components/libraries/que \
        -I$(SDK_PATH)/components/libraries/ringbuf \
        -I$(SDK_PATH)/components \

# Include paths specific to target
ifeq ("$(TARGET)","nrf52832")
INC_PATHS += \
        -I$(SDK_PATH)/components/softdevice/s132/headers \
        -I$(SDK_PATH)/components/softdevice/s132/headers/nrf52 \
        -I$(SDK_PATH)/components/libraries/sha256 \
        -I$(SDK_PATH)/external/micro-ecc/micro-ecc \

else
INC_PATHS += \
        -I$(SDK_PATH)/components/softdevice/s140/headers \
        -I$(SDK_PATH)/components/softdevice/s140/headers/nrf52 \
        -I$(SDK_PATH)/external/nrf_cc310/include \
        -I$(SDK_PATH)/external/fprintf \
        -I$(SDK_PATH)/components/libraries/mutex \

endif

INC_PATHS += \
        -I$(KEY_PATH) \
        -I$(SRC_PATH) \

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
CFLAGS += -DuECC_ENABLE_VLI_API=0
CFLAGS += -DuECC_OPTIMIZATION_LEVEL=3
CFLAGS += -DuECC_SQUARE_FUNC=0
CFLAGS += -DuECC_SUPPORT_COMPRESSED_POINT=0
CFLAGS += -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1
else
CFLAGS += -DBOARD_PCA10056
CFLAGS += -DNRF52840_XXAA
CFLAGS += -DS140
endif

CFLAGS += -D__HEAP_SIZE=0
CFLAGS += -D__STACK_SIZE=2048
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DNRF_DFU_SETTINGS_VERSION=2
CFLAGS += -DNRF_DFU_SVCI_ENABLED
CFLAGS += -DNRF_SD_BLE_API_VERSION=7
CFLAGS += -DSVC_INTERFACE_CALL_AS_NORMAL_FUNCTION
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DINITIALIZE_USER_SECTIONS
CFLAGS += -DNO_VTOR_CONFIG
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Werror
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# Assembler flags
ifeq ("$(TARGET)","nrf52832")
ASMFLAGS += -DBOARD_PCA10040
ASMFLAGS += -DNRF52832_XXAA
ASMFLAGS += -DNRF52
ASMFLAGS += -DNRF52_PAN_74
ASMFLAGS += -DS132
ASMFLAGS += -DuECC_ENABLE_VLI_API=0
ASMFLAGS += -DuECC_OPTIMIZATION_LEVEL=3
ASMFLAGS += -DuECC_SQUARE_FUNC=0
ASMFLAGS += -DuECC_SUPPORT_COMPRESSED_POINT=0
ASMFLAGS += -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1
else
ASMFLAGS += -DBOARD_PCA10056
ASMFLAGS += -DNRF52840_XXAA
ASMFLAGS += -DS140
endif

# Assembler flags common to all targets
ASMFLAGS += -D__HEAP_SIZE=0
ASMFLAGS += -D__STACK_SIZE=2048
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DNO_VTOR_CONFIG
ASMFLAGS += -DINITIALIZE_USER_SECTIONS
ASMFLAGS += -DNRF_DFU_SETTINGS_VERSION=2
ASMFLAGS += -DNRF_DFU_SVCI_ENABLED
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=7
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DSVC_INTERFACE_CALL_AS_NORMAL_FUNCTION

# Linker flags
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_PATH)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs
LDFLAGS += -Xlinker -Map=$(LISTING_DIRECTORY)/$(OUTPUT_FILENAME).map

# Libraries common to all targets
LIBS += \
        $(SDK_PATH)/external/nrf_oberon/lib/cortex-m4/hard-float/liboberon_3.0.8.a \

# Libraries speceific to target
ifeq ("$(TARGET)","nrf52832")
LIBS += \
        $(SDK_PATH)/external/micro-ecc/nrf52hf_armgcc/armgcc/micro_ecc_lib_nrf52.a \

else
LIBS += \
        $(SDK_PATH)/external/nrf_cc310_bl/lib/cortex-m4/hard-float/libnrf_cc310_bl_0.9.13.a \

endif

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

# Release
.PHONY: debug
debug: clean
debug: CFLAGS += -O0 -g2 
debug: LDFLAGS += -O0 -g2
debug: ASMFLAGS += -O0 -g2
debug: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out

# Release
.PHONY: release
release: clean
release: CFLAGS += -DNDEBUG -Os -g3
release: LDFLAGS += -DNDEBUG -Os -g3
release: ASMFLAGS += -DNDEBUG -Os -g3
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

# # Write the bootloader start and end addresses
# .PHONY: write_bootloader_addr
# write_bootloader_addr:
# 	nrfjprog -f nrf52 --memwr 0x10001014 --val 0x00078000
# 	nrfjprog -f nrf52 --memwr 0x10001018 --val 0x0007E000

# Erase nRF52
erase:
	nrfjprog -f nrf52 --eraseall

# Run the SDK configuration tool
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
