#include <sbi/sbi_legacy.h>

// Invoke legacy SBI functions using ecall
static sbi_ret_t 
sbi_lgcy_ecall(
    sbi_eid_t eid,
    sbi_arg_t arg0,
    sbi_arg_t arg1,
    sbi_arg_t arg2,
    sbi_arg_t arg3,
    sbi_arg_t arg4,
    sbi_arg_t arg5
)
{
  // Make sure legacy convention is followed
  register sbi_eid_t eid_val  __asm__("a7") = eid;
  register sbi_arg_t arg0_val __asm__("a0") = arg0;
  register sbi_arg_t arg1_val __asm__("a1") = arg1;
  register sbi_arg_t arg2_val __asm__("a2") = arg2;
  register sbi_arg_t arg3_val __asm__("a3") = arg3;
  register sbi_arg_t arg4_val __asm__("a4") = arg4;
  register sbi_arg_t arg5_val __asm__("a5") = arg5;

  __asm__ volatile (
       "ecall"
       : "+r"(arg0_val)
       : "r"(arg1_val),
	 "r"(arg2_val),
	 "r"(arg3_val),
	 "r"(arg4_val),
	 "r"(arg5_val),
	 "r"(eid_val)
       : "memory"
 );

  return (sbi_ret_t)arg0_val;

} 

// Programs the clock for next event after stime_value time
// Function also clears pending timer interrupt bit
// Returns 0 upon success or an implementation specfic negative error code
sbi_ret_t 
sbi_set_timer(sbi_arg_t stime_value)
{
  return sbi_lgcy_ecall(SBI_LGCY_SET_TIMER,stime_value,0,0,0,0,0);
}

// Write a char to debug console
// Returns 0 upon success or an implementation specfic negative error code
sbi_ret_t
sbi_console_putchar(sbi_arg_t ch)
{
  return sbi_lgcy_ecall(SBI_LGCY_CNSLE_PUTCHR,ch,0,0,0,0,0);
}

// Read a char from debug console
// Returns value of char  upon success or -1 for failure
sbi_ret_t
sbi_console_getchar(void)
{
  return sbi_lgcy_ecall(SBI_LGCY_CNSLE_GETCHR,0,0,0,0,0,0);
}

// Clears the pending IPIs if any. IPI is cleared only in the hart for which this
// SBI call is invoked
// Returns 0 upon success or an implementation specfic negative error code
sbi_ret_t
sbi_clear_ipi(void)
{
  return sbi_lgcy_ecall(SBI_LGCY_CLR_IPI,0,0,0,0,0,0);
}

// Send an inter-processor interrupt to all harts defined in hart_mask
// Returns 0 upon success or an implementation specfic negative error code
sbi_ret_t
sbi_send_ipi(const sbi_arg_t *hart_mask)
{
  return sbi_lgcy_ecall(SBI_LGCY_SEND_IPI,(sbi_arg_t)hart_mask,0,0,0,0,0);
}

// Instructs remote harts to execute the FENC.I instruction
// Returns 0 upon success or an implementation specfic negative error code
sbi_ret_t
sbi_remote_fence_i(const sbi_arg_t *hart_mask)
{
  return sbi_lgcy_ecall(SBI_LGCY_RMTE_FENCE_I,(sbi_arg_t)hart_mask,0,0,0,0,0);
}

// Instructs the remote harts to execute one or more SFENCE.VMA instructions,
// covering range of addresses between start and start + size
// Returns 0 upon success or an implementation specfic negative error code
sbi_ret_t
sbi_remote_sfence_vma(const sbi_arg_t *hart_mask, 
                            sbi_arg_t start,
                            sbi_arg_t end)
{
  return sbi_lgcy_ecall(SBI_LGCY_SFENCE_VMA,(sbi_arg_t)hart_mask,start,end,0,0,0);
}

// Instruct the remote harts to execute on or more SFENCE.VMA insructions,
// covering range of addresses between start and start + size
// This only covers the given ASID 
// Returns 0 upon success or an implementation specfic negative error code
sbi_ret_t
sbi_remote_sfence_vma_asid(const sbi_arg_t *hart_mask,
                                 sbi_arg_t start,
                                 sbi_arg_t end,
                                 sbi_arg_t asid)
{
  return sbi_lgcy_ecall(SBI_LGCY_SFENCE_VMA_ASID,(sbi_arg_t)hart_mask,start,end,asid,0,0);
}

// Puts harts in shutdown state from supervisor point of view
// Doesn't return irrespective of the call fails or succeeds
void sbi_shutdown(void)
{
  sbi_lgcy_ecall(SBI_LGCY_SHUTDOWN,0,0,0,0,0,0);
}


