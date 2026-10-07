#include <xv6/types.h>
#include <xv6/param.h>
#include <cpu/cpu.h>
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

volatile static int started = 0;

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


// start() jumps here on all CPUs eventually.
__attribute__((noreturn)) void
main()
{
  if (cpuid() == 0) {
    consoleinit();
    printkinit();
    printk("\n");
    printk("DEVICE:%s\n",DEVICE_NAME);
    printk("Xv6 kernel is booting...\n");
    printk("\n");

    #if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    uart_puts("Little endian architecture detected\n");
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    uart_puts("Big endian architecture detected\n");
#elif defined(__BYTE_ORDER__) && defined(__ORDER_PDP_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_PDP_ENDIAN__)
    uart_puts("Mixed endianness detected\n");
#else
    uart_puts("Endianness undefined\n");
#endif

    wdt_start(1000); // 1s timeout
    wdt_stop();
    uart_puts("SoC watchdog disabled!");

    wdt_ret = pmic_wdt_start(1000);
    if (wdt_ret)
      uart_puts("PMIC WDT start failed\n");

    wdt_ret = pmic_wdt_stop();
    if (wdt_ret)
      uart_puts("PMIC WDT stop failed\n");

    uart_puts("PMIC watchdog disabled!");

    struct sbiret pres;
    // Probe extensions 
    for (uint32 i = 0; i < EXT_TABLE_SIZE; i++) {
      pres = sbi_probe_extension(extensions[i].eid);
      if (pres.error != SBI_SUCCESS) {
        uart_puts("PROBE FAILED: ");
        uart_puts(extensions[i].ext_name);
        uart_putc('\n');
      } else if (pres.value == 0) {
        uart_puts("SBI extension not supported: ");
        uart_puts(extensions[i].ext_name);
        uart_putc('\n');
      } else {
        uart_puts("SBI extension supported: ");
        uart_puts(extensions[i].ext_name);
        uart_putc('\n');
      }
    }

    // kinit();            // physical page allocator
    // kvminit();          // create kernel page table
    // kvminithart();      // turn on paging
    // procinit();         // process table
    trapinit();         // trap vectors
    trapinithart();     // install kernel trap vector
    plicinit();         // set up interrupt controller
    plicinithart();     // ask PLIC for device interrupts
    // binit();            // buffer cache
    // iinit();            // inode table
    // fileinit();         // file table
    // virtio_disk_init(); // emulated hard disk - will need to replace this
   // userinit();         // first user process
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    started = 1;
  } else {
    while (started == 0)
      ;
    __atomic_thread_fence(__ATOMIC_SEQ_CST);

    printk("hart %d starting\n", cpuid());
    // kvminithart();  // turn on paging
    trapinithart(); // install kernel trap vector
    plicinithart(); // ask PLIC for device interrupts
  }

  // scheduler();
  __builtin_unreachable();

}
