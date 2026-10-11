#include <sbi/sbi.h>
#include <sbi/sbi_ext.h>
#include <sbi/sbi_hsm.h>


struct sbiret sbi_hart_start(sbi_arg_t hartid,
                             sbi_arg_t start_addr,
                             sbi_arg_t opaque)
{
  return sbi_ecall(SBI_HSM_EXT, HSM_START, hartid, start_addr, opaque, 0, 0, 0);
}

struct sbiret sbi_hart_stop()
{
  return sbi_ecall(SBI_HSM_EXT, HSM_STOP, 0, 0, 0, 0, 0, 0);
}

struct sbiret sbi_hart_get_status(sbi_arg_t hartid)
{
  return sbi_ecall(SBI_HSM_EXT, HSM_STATUS, hartid, 0, 0, 0, 0 ,0);
}

struct sbiret sbi_hart_suspend(uint32 suspend_type,
                               sbi_arg_t resume_addr,
                               sbi_arg_t opaque)
{
  return sbi_ecall(SBI_HSM_EXT, HSM_SUSPEND, suspend_type, resume_addr, opaque, 0, 0, 0);
}




