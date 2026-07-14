section .text
.global sbi_hart_start
SBI_EXT_HSM_HART_START:
        li a6, 0            # SBI_EXT_HSM_HART_START
        li a7, 0x48534D     # SBI_EXT_HSM
        ecall
        ret
