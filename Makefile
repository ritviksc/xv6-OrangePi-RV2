K=kernel
U=user

OBJS = \
  $K/entry.o \
  $K/start.o \
  $K/test_main.o

ifndef TOOLPREFIX
TOOLPREFIX := $(shell if riscv64-unknown-elf-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-unknown-elf-'; \
	elif riscv64-elf-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-elf-'; \
	elif riscv64-none-elf-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-none-elf-'; \
	elif riscv64-linux-gnu-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-linux-gnu-'; \
	elif riscv64-unknown-linux-gnu-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-unknown-linux-gnu-'; \
	else echo "*** Error: Couldn't find RISC-V toolchain" 1>&2; exit 1; fi)
endif

CC = $(TOOLPREFIX)gcc
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

CFLAGS = -Wall -Werror -Wno-unknown-attributes -O -fno-omit-frame-pointer -ggdb -gdwarf-2
CFLAGS += -march=rv64gc
CFLAGS += -mabi=lp64
CFLAGS += -std=gnu99
CFLAGS += -MD
CFLAGS += -mcmodel=medany
CFLAGS += -ffreestanding
CFLAGS += -fno-common
CFLAGS += -nostdlib
CFLAGS += -fno-builtin-strncpy -fno-builtin-strncmp -fno-builtin-strlen
CFLAGS += -fno-builtin-memset -fno-builtin-memmove -fno-builtin-memcmp
CFLAGS += -fno-builtin-memcpy -fno-builtin-bzero
CFLAGS += -Wno-main
CFLAGS += -I.
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)

ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
CFLAGS += -fno-pie -no-pie
endif

ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
CFLAGS += -fno-pie -nopie
endif

LDFLAGS = -z max-page-size=4096

$(K)/%.o: $(K)/%.S
	$(CC) $(CFLAGS) -c -o $@ $<

$(K)/%.o: $(K)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K)/kernel: $(OBJS) $(K)/kernel.ld
	$(LD) $(LDFLAGS) -T $(K)/kernel.ld -o $@ $(OBJS)
	$(OBJDUMP) -S $@ > $(K)/kernel.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(K)/kernel.sym

RV2_LOAD_ADDR = 0x11000000

$(K)/kernel.bin: $(K)/kernel
	$(OBJCOPY) -O binary $< $@

$(K)/uImage-xv6: $(K)/kernel.bin
	mkimage -A riscv -O linux -T kernel -C none \
		-a $(RV2_LOAD_ADDR) \
		-e $(RV2_LOAD_ADDR) \
		-n "xv6-minimal" \
		-d $< $@

.PHONY: rv2
rv2: $(K)/uImage-xv6

-include kernel/*.d

clean:
	rm -f $(K)/*.o $(K)/*.d $(K)/*.asm $(K)/*.sym \
	$(K)/kernel $(K)/kernel.bin $(K)/uImage-xv6

.PHONY: clean
