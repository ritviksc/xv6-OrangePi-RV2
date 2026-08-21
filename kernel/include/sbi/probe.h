#ifndef PROBE_H
#define PROBE_H

#include <sbi/sbi_ext.h>

// base functionality to probe what extensions are avaliable
// also check version of SBI running on hardware

// SBI_BASE_EXT FID values
#define SBI_SPEC_VER 0x0
#define SBI_IMPL_ID   0x1
#define SBI_IMPL_VER  0x2
#define SBI_PROBE_EXT 0x3
#define SBI_MVNDR_ID  0x4
#define SBI_MARCH     0x5
#define SBI_MIMPID    0x6


struct sbiret sbi_get_spec_version(void);
struct sbiret sbi_get_impl_id(void);
struct sbiret sbi_get_impl_version(void);
struct sbiret sbi_probe_extension(sbi_eid_t eid);
struct sbiret sbi_get_mvendorid(void);
struct sbiret sbi_get_marchid(void);
struct sbiret sbi_get_mimpid(void);

#endif
