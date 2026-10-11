#ifndef SBI_HSM_H
#define SBI_HSM_H

#include <sbi/sbi.h>
#include <sbi/sbi_ext.h>
#include <xv6/types.h>

/**
 * The Hart State Management (HSM) Extension introduces a set of hart states and a set of functions
 * which allow the supervisor-mode software to request a hart state change.
 */
#define STARTED 		0
#define STOPPED 		1 
#define START_PENDING 		2 
#define STOP_PENDING 		3 
#define SUSPENDED 		4
#define SUSPEND_PENDING  	5
#define RESUME_PENDING 		6

// HSM FIDs
#define HSM_START		0
#define HSM_STOP		1
#define HSM_STATUS		2
#define HSM_SUSPEND		3

struct sbiret sbi_hart_start(sbi_arg_t hartid,
  			     sbi_arg_t start_addr,
 			     sbi_arg_t opaque);
struct sbiret sbi_hart_stop(void);
struct sbiret sbi_hart_get_status(sbi_arg_t hartid);
struct sbiret sbi_hart_suspend(uint32 suspend_type,
 			       sbi_arg_t resume_addr,
  			       sbi_arg_t opaque);


#endif
