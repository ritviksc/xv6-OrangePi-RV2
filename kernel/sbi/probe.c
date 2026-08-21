#include <sbi/probe.h>


struct sbiret 
sbi_get_spec_version(void)
{
  return sbi_ecall(SBI_BASE_EXT,SBI_SPEC_VER,0,0,0,0,0,0);
}

struct sbiret 
sbi_get_impl_id(void)
{
  return sbi_ecall(SBI_BASE_EXT,SBI_IMPL_ID,0,0,0,0,0,0);
}

struct sbiret
sbi_get_impl_version(void)
{
  return sbi_ecall(SBI_BASE_EXT,SBI_IMPL_VER,0,0,0,0,0,0);
}

// Returns 0 if the EID is not avaliable, or 1 if avaliable
// Function may return a non-zero value other than 1 in some
// implementations
struct sbiret
sbi_probe_extension(sbi_eid_t eid)
{
  return sbi_ecall(SBI_BASE_EXT,SBI_PROBE_EXT,0,0,0,0,0,0);
}

struct sbiret
sbi_get_mvendorid(void)
{
  return sbi_ecall(SBI_BASE_EXT,SBI_MVNDR_ID,0,0,0,0,0,0);
}

struct sbiret
sbi_get_marchid(void)
{
  return sbi_ecall(SBI_BASE_EXT,SBI_MARCH_ID,0,0,0,0,0,0);
}

struct sbiret
sbi_get_mimpid(void)
{
  return sbi_ecall(SBI_BASE_EXT,SBI_MIMPID,0,0,0,0,0,0);
}

