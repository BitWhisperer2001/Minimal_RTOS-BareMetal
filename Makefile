# Toolchain
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size
OBJDUMP = arm-none-eabi-objdump.exe

# Project name
TARGET = mini_rtos

# Directories
BUILD_DIR = build
INC_DIR = inc
SRC_DIR = source
SCHED_INC_DIR = scheduler/inc
SCHED_SRC_DIR = scheduler/src
STARTUP_DIR = startup

# Source files
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/led.c \
		  $(SRC_DIR)/syscalls.c \
          $(SCHED_SRC_DIR)/mini_rtos.c \
          $(STARTUP_DIR)/startup_stm32f411re.c

# Object files
OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.c=.o)))

# Include paths
INCLUDES = -I$(INC_DIR) \
           -I$(SCHED_INC_DIR)

# Compiler flags
CFLAGS = -mcpu=cortex-m4 \
         -mthumb \
         -mfloat-abi=soft \
         -Wall \
         -Wextra \
         -O0 \
         -g3 \
         -ffunction-sections \
         -fdata-sections \
         $(INCLUDES)

# Linker flags
LDFLAGS = -mcpu=cortex-m4 \
          -mthumb \
          -mfloat-abi=soft \
          --specs=nano.specs \
          -T linker_scripts.ld \
          -Wl,-Map=$(BUILD_DIR)/$(TARGET).map \
          -Wl,--print-memory-usage

# VPATH for source file locations
VPATH = $(SRC_DIR):$(SCHED_SRC_DIR):$(STARTUP_DIR)

# Default target
all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hex
	@echo "================================"
	@echo "Build complete!"
	@echo "================================"
	@$(SIZE) $(BUILD_DIR)/$(TARGET).elf

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	@echo "Linking: $@"
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Compile scheduler files
$(BUILD_DIR)/%.o: $(SCHED_SRC_DIR)/%.c
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Compile startup files
$(BUILD_DIR)/%.o: $(STARTUP_DIR)/%.c
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Generate .bin file
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	@echo "Creating binary: $@"
	$(OBJCOPY) -O binary $< $@

# Generate .hex file
$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	@echo "Creating hex: $@"
	$(OBJCOPY) -O ihex $< $@

# Clean
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)

# Dump
objdump:
	@echo "Dump final file !"
	$(OBJDUMP) -h $(BUILD_DIR)/$(TARGET).elf

# Flash to board (using st-link)
# flash: all
# 	@echo "Flashing to board..."
# 	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x08000000

# Flash using OpenOCD (alternative)
# flash-openocd: all
# 	@echo "Flashing via OpenOCD..."
# 	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"

# Debug with GDB
# debug:
# 	arm-none-eabi-gdb $(BUILD_DIR)/$(TARGET).elf

# Show size
size: $(BUILD_DIR)/$(TARGET).elf
	@echo "Memory usage:"
	$(SIZE) $

# Rebuild
rebuild: clean all

# Show project structure
tree:
	@echo "Project Structure:"
	@echo "."
	@echo "|── inc/"
	@echo "│   ├── led.h"
	@echo "│   └── main.h"
	@echo "|── source/"
	@echo "│   ├── led.c"
	@echo "│   └── main.c"
	@echo "├── scheduler/"
	@echo "│   ├── inc/"
	@echo "│   │   ├── config.h"
	@echo "│   │   └── mini_rtos.h"
	@echo "│   └── src/"
	@echo "│       └── mini_rtos.c"
	@echo "├── startup/"
	@echo "│   └── startup_stm32f411re.c"
	@echo "├── linker_scripts.ld"
	@echo "└── Makefile"

.PHONY: all clean flash flash-openocd debug size rebuild tree