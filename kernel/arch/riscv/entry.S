        # UBoot starts running on hart 0 and makes it jump to _entry
	# hart 0 wakes up the remaining harts and they jump to _entry
 
        # kernel.ld causes the following code to
        # be placed at 0x11000000.

	# Note - bootloader loads the kernel in S-mode
	# The RISC-V kernel expects the following of bootloaders and platform firmware:
	# a0 to contain the hartid of the current core.
	# a1 to contain the address of the devicetree in memory.
	# xv6 doesn't require the devicetree at the moment - it uses memlayout.h
.section .text
.global _entry
_entry:
        # set up a stack for C.
        # stack0 is declared in start.c,
        # with a 4096-byte stack per CPU.
        # sp = stack0 + ((hartid + 1) * 4096)

	# a0 = hartid, a1 = dtb pointer - both left untouched in case
        # devicetree support or hartid reuse is added later.
        la sp, stack0
	mv tp, a0 	# Save hartid in tp register

        li a2, 1024*4
        addi a3, tp, 1  
        mul a2, a2, a3 	
        add sp, sp, a2

# hart 0 is responsible for clearing bss section
bss_init:
	la t0,bss_start
	la t1,bss_end
clear_bss:
	bge t0,t1, call_start
	sd zero, 0(t0)
	addi t0,t0,8
	j clear_bss

call_start:
        # jump to start() in start.c
        call start
spin:
        j spin # Should not reach here!
