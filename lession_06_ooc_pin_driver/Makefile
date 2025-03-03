# DEBUG
DEBUG 		= true

# Project name
PROJECT     := TARGET

# Define compile tool
CC_PREFIX   := arm-none-eabi-
CXX         := $(CC_PREFIX)g++
CC          := $(CC_PREFIX)gcc
CP          := $(CC_PREFIX)objcopy
GDB         := $(CC_PREFIX)gdb
SIZE        := $(CC_PREFIX)size
AS          := $(CXX) -x assembler-with-cpp
HEX         := $(CP) -O ihex
BIN         := $(CP) -O binary -S

# parameters
SPECS 		+= -mthumb #define cortex-m instruction set
SPECS 		+= -mcpu=cortex-m0plus #define MCU architecture
SPECS       += -O0 #define optimization level
SPECS       += -fsingle-precision-constant  # this means that by default, all constants are single-float, not double type
SPECS       += -fno-common  # Inhibiting variables multiple definition
SPECS       += -ffunction-sections -fdata-sections  # Each function is divided into individual section
SPECS_C     += $(SPECS) -std=c99
SPECS_CXX   += $(SPECS) -std=c++11

# Define link script
LINK_SCRIPT := $(shell find . -name "*.ld")
ELF_FILES 	:= $(shell find . -name "*.elf")
MAP_FILES 	:= $(shell find . -name "*.map")
HEX_FILES 	:= $(shell find . -name "*.hex")
BIN_FILES 	:= $(shell find . -name "*.bin")

################################################################################
# Define the flags in compiling and linking
ifeq ($(DEBUG), true)
FLAGS	 	+= -g -gdwarf-2 #enable debugging function , default debugging form  is dwarf-5 in .o files
FLAGS	 	+=  -fverbose-asm #-save-temps  # generate debugging files 
LIBS        += --specs=rdimon.specs -u _printf_float -lm -lc -lrdimon # Use Semihosting Debug When Compiling with GCC
																# and enable floating point support in printf. 
																# It is not enabled by default
else
LIBS 		+= -specs=nano.specs -lm # nano.specs is smaller than rdimon.specs . 
									# lm is math lib,
FLAGS	   	+= -fomit-frame-pointer #save memory ,but it is not good for debugging
endif
FLAGS	 	+= -c -MD -MP   # generate .d files, keep track of dependencies between .h and .c files
FLAGS	   	+= -Wall  # enable all warnings
FLAGS_CXX   += $(FLAGS) $(SPECS_CXX)  
FLAGS_C   	+= $(FLAGS) $(SPECS_C)  

FLAGS_AS 	+= $(SPECS_C) $(FLAGS)  #enable debugging function , default debugging form  is dwarf-5 in .o files
FLAGS_LD 	+= $(SPECS_C) $(LIBS)   # link standard library
FLAGS_LD	+= -Xlinker --gc-section  # delete untapped code or data to save memory 
FLAGS_LD 	+= -T$(LINK_SCRIPT) -lc -lnosys # link .ld file
FLAGS_LD 	+= -Wl,-Map=$(PROJECT).map,--cref,--no-warn-mismatch # generate .map file , 
																# --cref , generate debugging message about cross-reference
																# --no-warn-mismatch,  disable the warning when  some symbols mismatch

# Define SRC, OBJ, DPT, INC files paths
# for c
C_INC_FILES           := $(shell find ./ -name "*.h" | xargs dirname)
C_INC_FILES           := $(addprefix -I, $(C_INC_FILES))
C_SRC_FILES           := $(shell find ./ -name "*.c")
C_PRE_COMPILE_FILES   := $(patsubst %.c,%.i,$(C_SRC_FILES))  # C pre-compile
# for cpp
CXX_SRC_FILES         := $(shell find ./ -name "*.cpp")  # Change from *.c to *.cpp for C++ source files
CXX_PRE_COMPILE_FILES := $(patsubst %.cpp,%.ii,$(CXX_SRC_FILES))  # Update for C++ pre-compile
# general setting
C_ASM_FILES           := $(shell find ./ -name "*.s")
C_ASM_FILES_TEMP      += $(patsubst %.cpp,%.s,$(CXX_SRC_FILES))  # Update to handle C++ files
C_ASM_FILES_TEMP      += $(patsubst %.c,%.s,$(C_SRC_FILES))  # Update to handle C files
C_OBJ_FILES           := $(patsubst %.cpp,%.o,$(CXX_SRC_FILES))  # Change to handle C++ objects
C_OBJ_FILES           += $(patsubst %.c,%.o,$(C_SRC_FILES))
C_OBJ_FILES           += $(patsubst %.s,%.o,$(C_ASM_FILES))
C_DPT_FILES           += $(patsubst %.cpp,%.d,$(CXX_SRC_FILES))  # Change to handle C++ dependencies
C_DPT_FILES           += $(patsubst %.c,%.d,$(C_SRC_FILES))  # Change to handle C dependencies

# Define debug parameters
TYPE_LOAD             := openocd_load
TYPE_BURN             := openocd_swd_flash
TYPE_DEBUG            := openocd_swd_debug
TYPE_ERASE            := openocd_swd_erase

################################################################################
.PHONY: all burn debug erase clean test

all: $(C_OBJ_FILES) $(PROJECT).elf $(PROJECT).hex $(PROJECT).bin
	$(SIZE) $(PROJECT).elf

%.o: %.c
	$(CC) $(C_INC_FILES) $(FLAGS_C) $< -o $@
%.o: %.cpp
	$(CXX) $(C_INC_FILES) $(FLAGS_CXX) $< -o $@
%.o: %.s
	$(AS) $(FLAGS_AS) $< -o $@
%.elf: $(C_OBJ_FILES)
	$(CC) $(C_OBJ_FILES) $(FLAGS_LD) -o $@
%.hex: %.elf
	$(HEX) $< $@
%.bin: %.elf
	$(BIN) $< $@

################################################################################

load: $(TYPE_LOAD)
burn: $(TYPE_BURN)
debug: $(TYPE_DEBUG)
erase: $(TYPE_ERASE)

openocd_load:
	openocd -f /usr/share/openocd/scripts/interface/cmsis-dap.cfg \
	-f /usr/share/openocd/scripts/target/stm32g0x.cfg \

openocd_swd_flash: $(PROJECT).bin
	$(GDB) \
	-ex 'target remote localhost:3333' \
	-ex 'monitor reset init' -ex 'monitor reset halt' \
	-ex 'monitor sleep 100' -ex 'monitor wait_halt 2' \
	-ex 'monitor flash write_image erase $(PROJECT).bin 0x08000000' \
	-ex 'monitor sleep 100' -ex 'monitor verify_image $(PROJECT).bin 0x08000000' \

openocd_swd_debug: $(PROJECT).bin
	$(GDB) \
	-ex 'target remote localhost:3333' \
	-ex 'monitor reset init' -ex 'monitor reset halt' \
	-ex 'monitor arm semihosting enable' \
	-ex 'load' \
	$(PROJECT).elf \
	-ex 'b main:35'

openocd_swd_erase:
	openocd -f /usr/share/openocd/scripts/interface/cmsis-dap.cfg \
	-f /usr/share/openocd/scripts/target/stm32g0x.cfg \
	-c "init" -c "reset halt" -c "sleep 100" -c "stm32g0x mass_erase 0" \
	-c "sleep 100" -c "shutdown"

test:
	@echo $(C_SRC_FILES)
	@echo $(CXX_SRC_FILES)
	@echo $(C_INC_FILES)
	@echo $(C_OBJ_FILES)
	@echo $(C_DPT_FILES)
	@echo $(C_ASM_FILES_TEMP)
	@echo $(C_PRE_COMPILE_FILES)
	@echo $(CXX_PRE_COMPILE_FILES)

clean:
	-rm $(C_DPT_FILES)
	-rm $(C_OBJ_FILES)
	-rm $(C_ASM_FILES_TEMP)
	-rm $(C_PRE_COMPILE_FILES)
	-rm $(CXX_PRE_COMPILE_FILES)
	-rm $(ELF_FILES)
	-rm $(MAP_FILES)
	-rm $(HEX_FILES)
	-rm $(BIN_FILES)

