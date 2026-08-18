#ifndef SBI_LEGACY_H
#define SBI_LEGACY_H

#include <sbi/sbi.h>
#include <xv6/types.h>

/* SBI LEGACY EXTENSION CODES (EIDS #0x00-#0x0F) */
#define SBI_LGCY_SET_TIMER          0x00
#define SBI_LGCY_CNSLE_PUTCHR       0x01 
#define SBI_LGCY_CNSLE_GETCHR       0x02
#define SBI_LGCY_CLR_IPI            0x03
#define SBI_LGCY_SEND_IPI           0x04
#define SBI_LGCY_RMTE_FENCI_I       0x05
#define SBI_LGCY_SFENCE_VMA         0x06
#define SBI_LGCY_SFENCY_VMA_ASID    0x07
#define SBI_LGCY_SHUTDOWN           0x08

/* Legacy SBI extensions have a slightly different calling convention as compared to modern SBI extensions
   FID in a6 is ignored, and nothing is returned in a1
   Value returned in a0 register is SBI legacy extension specfic
*/
sbi_ret_t 
sbi_lgcy_ecall(
    sbi_eid_t eid,
    sbi_arg_t arg0,
    sbi_arg_t arg1,
    sbi_arg_t arg2,
    sbi_arg_t arg3,
    sbi_arg_t arg4,
    sbi_arg_t arg5
);

sbi_ret_t sbi_set_timer(sbi_arg_t stime_value);
sbi_ret_t sbi_console_putchar(sbi_arg_t ch);
sbi_ret_t sbi_console_getchar(void);
sbi_ret_t sbi_clear_ipi(void);
sbi_ret_t sbi_send_ipi(const sbi_arg_t *hart_mask);
sbi_ret_t sbi_remote_fence_i(const sbi_arg_t *hart_mask);
sbi_ret_t sbi_remote_sfence_vma(const sbi_arg_t *hart_mask, sbi_arg_t start, sbi_arg_t end);
sbi_ret_t sbi_remote_sfence_vma_asid(const sbi_arg_t *hart_mask, sbi_arg_t start, sbi_arg_t end, sbi_arg_t asid);
sbi_ret_t sbi_shutdown(void);


#endif
