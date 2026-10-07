# interrupts and exceptions while in supervisor
# mode come here, so in our case when the kernel is running.
#
# The current stack is a kernel stack.
# push registers, call kerneltrap().
#
# When kerneltrap() returns from handling the kernel trap,
# restore registers and return.
.globl kerneltrap
.globl kernelvec

# stvec must be aligned to at least a 4 byte boundary so interrupts 
# use direct mode.
# the lowest two bits in stvec must be either 01 or 00. 00 for direct mode,
# or 01 for vectored mode.
# xv6 aligns it to a 16 byte boundary to make sure we use direct mode 
# NOTE: align x implies aligning to a 2^x byte boundary (if using GAS)
.align 4

kernelvec:
        # make room to save registers.
        addi sp, sp, -256

        # save caller-saved registers.
        sd ra, 0(sp)
        # sd sp, 8(sp)
        sd gp, 16(sp)
        # sd tp, 24(sp)
        sd t0, 32(sp)
        sd t1, 40(sp)
        sd t2, 48(sp)
        sd a0, 72(sp)
        sd a1, 80(sp)
        sd a2, 88(sp)
        sd a3, 96(sp)
        sd a4, 104(sp)
        sd a5, 112(sp)
        sd a6, 120(sp)
        sd a7, 128(sp)
        sd t3, 216(sp)
        sd t4, 224(sp)
        sd t5, 232(sp)
        sd t6, 240(sp)

        # call the kernel trap handler
        call kerneltrap

        # restore registers.
        ld ra, 0(sp)
        # ld sp, 8(sp)
        ld gp, 16(sp)
        # not tp (contains hartid), in case we moved CPUs
        ld t0, 32(sp)
        ld t1, 40(sp)
        ld t2, 48(sp)
        ld a0, 72(sp)
        ld a1, 80(sp)
        ld a2, 88(sp)
        ld a3, 96(sp)
        ld a4, 104(sp)
        ld a5, 112(sp)
        ld a6, 120(sp)
        ld a7, 128(sp)
        ld t3, 216(sp)
        ld t4, 224(sp)
        ld t5, 232(sp)
        ld t6, 240(sp)

        addi sp, sp, 256

        # return to whatever we were doing in the kernel.
        sret
