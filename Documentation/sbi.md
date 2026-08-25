## The RV2 ubuntu image uses:  
SBI 1.0  
OpenSBI 1.3  
Machine:  
Vendor ID 710  
Architecture ID 8000000058000001  
Implementation ID 1000000049772200  
Extensions:  
sbi_set_timer  
sbi_console_putchar  
sbi_console_getchar  
sbi_clear_ipi  
sbi_send_ipi  
sbi_remote_fence_i  
sbi_remote_sfence_vma  
sbi_remote_sfence_vma_asid  
sbi_shutdown  
SBI Base Functionality  
IPI Extension  
RFENCE Extension  
Hart State Management Extension  
Performance Monitoring Unit Extension 

## MODERN EXTENSIONS PROBE OUTPUT  
SBI extension supported: BASE  
SBI extension not supported: TIME  
SBI extension supported: IPI  
SBI extension supported: RFNC  
SBI extension supported: HSM  
SBI extension not supported: SRST  
SBI extension supported: PMU  
SBI extension supported: DBCN  
SBI extension supported: SUSP  
SBI extension not supported: CPPC  
SBI extension not supported: NACL  
SBI extension not supported: STA  
SBI extension not supported: SSE  
SBI extension not supported: FWFT  
SBI extension not supported: DBTR  
SBI extension not supported: MPXY  
  
If the other extensions are required use an updated version of OpenSBI.  
