#include <xv6/types.h>
#include <xv6/param.h>
#include <cpu/cpu.h>
#include <drivers/uart/sbi_uart.h>
#include <drivers/uart/uart.h>
#include <drivers/tty/console.h>
#include <drivers/tty/printk.h>
#include <drivers/plic/plic.h>
#include <trap/trap.h>
#include <arch/riscv/rv2/memlayout.h>
#include <arch/riscv/rv2/arch.h>
#include <arch/riscv/riscv.h>
#include <sbi/probe.h>
#include <drivers/watchdog/wdt.h>
#include <drivers/watchdog/pmic_wdt.h>
// #include "defs.h"

// volatile static int started = 0;
// extern void timerinit();

struct
sbi_ext_info {
  sbi_eid_t eid;
  const char *ext_name;
};

/* Extension Compatability Table
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
*/

// start() jumps here on all CPUs eventually.
void
main()
{
  uart_puts("M0: entered main\n");

  uart_puts("M1: before cpuid\n");
  int id = cpuid();
  uart_puts("M2: after cpuid\n");

  if (id == 0)
    uart_puts("M3: cpuid is zero\n");
  else
    uart_puts("M3: cpuid is NOT zero\n");

  uart_puts("M4: before consoleinit\n");

  consoleinit();

  uart_puts("M5: after consoleinit\n");
 
  for (;;)
     ;
}
