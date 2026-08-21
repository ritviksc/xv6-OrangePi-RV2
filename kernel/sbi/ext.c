#include <sbi/sbi_ext.h>

// Invoke SBI functions using ecall
static struct sbiret
sbi_ecall(
    sbi_eid_t eid,
    sbi_fid_t fid,
    sbi_arg_t arg0,
    sbi_arg_t arg1,
    sbi_arg_t arg2,
    sbi_arg_t arg3,
    sbi_arg_t arg4,
    sbi_arg_t arg5
)
{
  // Make sure modern convention is followed
  volatile register sbi_eid_t eid_val  __asm__("a7") = eid;
  volatile register sbi_fid_t fid_val  __asm__("a6") = fid;
  volatile register sbi_arg_t arg0_val __asm__("a0") = arg0;
  volatile register sbi_arg_t arg1_val __asm__("a1") = arg1;
  volatile register sbi_arg_t arg2_val __asm__("a2") = arg2;
  volatile register sbi_arg_t arg3_val __asm__("a3") = arg3;
  volatile register sbi_arg_t arg4_val __asm__("a4") = arg4;
  volatile register sbi_arg_t arg5_val __asm__("a5") = arg5;

  __asm__ volatile (
       "ecall"
       : "+r"(arg0_val),
	 "+r"(arg1_val)
       : "r"(arg2_val),
         "r"(arg3_val),
         "r"(arg4_val),
         "r"(arg5_val),
         "r"(eid_val),
         "r"(fid_val)
       : "memory"
 );

  return (struct sbiret){ 
      .error = arg0_val,
      .value = arg1_val
       };

}

