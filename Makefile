CROSS   = arm-none-eabi-
CC      = $(CROSS)gcc
OBJCOPY = $(CROSS)objcopy

CFLAGS  = -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -nostdlib -ffreestanding -Os -Wall -Wextra -Werror -ffunction-sections -fdata-sections
LDFLAGS = -T link.ld -Wl,--gc-sections -Wl,-Map=app.map

# Every .c in this directory. An exercise that adds a source file needs no
# edit here -- forgetting to add one produced an `undefined reference` that
# reads like a code error rather than a build-configuration one.
# Link order is irrelevant: link.ld places the vector table with
# KEEP(*(.isr_vector)), so startup.o need not come first.
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: app.elf app.hex

app.elf: $(OBJS) link.ld
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

app.hex: app.elf
	$(OBJCOPY) -O ihex $< $@

clean:
	rm -f $(OBJS) app.elf app.hex app.map

# Deliberately not called `test`. This target contacts the simulation service
# and spends metered simulation time from the account that owns
# WOKWI_CLI_TOKEN, so it must never run by reflex or as part of `all`. Build
# and inspect locally in the editor extension first, which is not metered.
#
# No expected string is written here. Every exercise prints something
# different, so pass one only when an exercise requires exact output:
#   make sim WOKWI_EXPECT_TEXT="Ready"
# Without it the run lasts the full timeout and reports whatever was printed.
WOKWI_TIMEOUT_MS ?= 2000

sim: app.elf
	@echo "This spends metered simulation time. Ctrl-C now to stop."
	wokwi-cli --timeout $(WOKWI_TIMEOUT_MS) \
	  $(if $(WOKWI_EXPECT_TEXT),--expect-text "$(WOKWI_EXPECT_TEXT)") .

.PHONY: all clean sim
