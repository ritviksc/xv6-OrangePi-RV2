#include <sbi/sbi_probe.h>
#include <sbi/sbi_ext.h>
#include <drivers/tty/printk.h>

struct
sbi_ext_info {
  sbi_eid_t eid;
  const char *ext_name;
};

// Extension Compatability Table
static const
struct sbi_ext_info extensions[] = {
  { SBI_BASE_EXT, "BASE" },
  { SBI_TIME_EXT, "TIME" },
  { SBI_IPI_EXT,  "IPI"  },
  { SBI_RFNC_EXT, "RFNC" },
  { SBI_HSM_EXT,  "HSM"  },
  { SBI_SRST_EXT, "SRST" },
  { SBI_PMU_EXT,  "PMU"  },
  { SBI_DBCN_EXT, "DBCN" },
  { SBI_SUSP_EXT, "SUSP" },
  { SBI_CPPC_EXT, "CPPC" },
  { SBI_NACL_EXT, "NACL" },
  { SBI_STA_EXT,  "STA"  },
  { SBI_SSE_EXT,  "SSE"  },
  { SBI_FWFT_EXT, "FWFT" },
  { SBI_DBTR_EXT, "DBTR" },
  { SBI_MPXY_EXT, "MPXY" }
};
#define EXT_TABLE_SIZE \
        (sizeof(extensions) / sizeof(extensions[0]))

struct sbiret 
sbi_get_spec_version()
{
  return sbi_ecall(SBI_BASE_EXT, SBI_SPEC_VER, 0, 0, 0, 0, 0, 0);
}

struct sbiret 
sbi_get_impl_id()
{
  return sbi_ecall(SBI_BASE_EXT, SBI_IMPL_ID, 0, 0, 0, 0, 0, 0);
}

struct sbiret
sbi_get_impl_version()
{
  return sbi_ecall(SBI_BASE_EXT, SBI_IMPL_VER, 0, 0, 0, 0, 0, 0);
}

// Returns 0 if the EID is not avaliable, or 1 if avaliable
// Function may return a non-zero value other than 1 in some
// implementations
struct sbiret
sbi_probe_extension(sbi_eid_t eid)
{
  return sbi_ecall(SBI_BASE_EXT, SBI_PROBE_EXT, eid, 0, 0, 0, 0, 0);
}

struct sbiret
sbi_get_mvendorid()
{
  return sbi_ecall(SBI_BASE_EXT, SBI_MVNDR_ID, 0, 0, 0, 0, 0, 0);
}

struct sbiret
sbi_get_marchid()
{
  return sbi_ecall(SBI_BASE_EXT, SBI_MARCH, 0, 0, 0, 0, 0, 0);
}

struct sbiret
sbi_get_mimpid()
{
  return sbi_ecall(SBI_BASE_EXT, SBI_MIMPID, 0, 0, 0, 0, 0, 0);
}

void sbiprobe()
{
  struct sbiret pres;
  // Probe extensions 
  for (uint32 i = 0; i < EXT_TABLE_SIZE; i++) {
    pres = sbi_probe_extension(extensions[i].eid);
    if (pres.error != SBI_SUCCESS) {
      printk("PROBE FAILED: %s\n", extensions[i].ext_name);
    } else if (pres.value == 0) {
      printk("SBI extension not supported: %s\n", extensions[i].ext_name);
    } else {
      printk("SBI extension supported: %s\n", extensions[i].ext_name);
    }
  }
}
