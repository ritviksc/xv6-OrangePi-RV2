// A minimal test point for the kernel
// This checks if xv6 is executing or not

#include <sbi/probe.h>
#include <drivers/uart/uart.h>
#include <drivers/watchdog/wdt.h>
#include <arch/riscv/rv2/memlayout.h>
#include <arch/riscv/rv2/arch.h>

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

void
main(void)
{
  uart_puts("DEVICE: ");
  uart_puts(DEVICE_NAME);
  uart_putc('\n');

  uart_puts("Xv6 kernel booting...\n");
  
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
  uart_puts("Watchdog disabled!");
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
  
  // Sleep cpu
  while (1) {
    uart_puts("w\n");
    //__asm__ volatile ("wfi");
  }

}
