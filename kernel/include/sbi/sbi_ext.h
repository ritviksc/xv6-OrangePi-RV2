#ifndef SBI_EXT_H
#define SBI_EXT_H

#include <sbi/sbi.h>
#include <xv6/types.h>

/* SBI EXTENSIONS */
#define SBI_BASE_EXT        0x10
#define SBI_TIME_EXT        0x54494D45
#define SBI_IPI_EXT         0x735049
#define SBI_RFNC_EXT        0x52464E43
#define SBI_HSM_EXT         0x48534D
#define SBI_SRST_EXT        0x53525354
#define SBI_PMU_EXT         0x504D55
#define SBI_DBCN_EXT        0x4442434E
#define SBI_SUSP_EXT        0x53555350
#define SBI_CPPC_EXT        0x43505043
#define SBI_NACL_EXT        0x4E41434C
#define SBI_STA_EXT         0x535441
#define SBI_SSE_EXT         0x535345
#define SBI_FWFT_EXT        0x46574654      
#define SBI_DBTR_EXT        0x44425452
#define SBI_MPXY_EXT        0x4D505859

// Modern SBI functions return pair of values in a0 and a1, with a0 returning an error code
// If the collection or data has a size < (2 * XLEN), compiler will put it in a0 and a1
struct sbiret {
  sbi_ret_t error;
  sbi_ret_t value;
};

/* An ecall is used as the control transfer instruction between the supervisor and the SEE
   a7 contains the SBI extension ID (EID)
   a6 encodes  the SBI function ID (FID)
   a0-a5 are inputs for the specfic function (not all maybe used - NULLIFY leftovers)
   Both EID and FID are encoded as signed 32-bit integers
   All registers except a0 and a1 must be preservered across an SBI call
   SBI functions return the structure sbiret 
*/
struct sbiret
sbi_ecall(
    sbi_eid_t eid,
    sbi_fid_t fid,
    sbi_arg_t arg0,
    sbi_arg_t arg1,
    sbi_arg_t arg2,
    sbi_arg_t arg3,
    sbi_arg_t arg4,
    sbi_arg_t arg5
);

#endif
