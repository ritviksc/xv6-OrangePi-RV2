#ifndef IRQ_H
#define IRQ_H

/*
 * RV2 PLIC hardware interrupt source IDs.
 * xv6 wont map these ids to a irq number space
 * like linux does, and instead use them directly.
 *
 * HWIRQ 0 is reserved by the PLIC.
 *
 * NOTE: These are not RISC-V scause values.
 * For example, UART0 is PLIC source 42, whereas a
 * supervisor external interrupt has scause code 9.
 */

#define IRQ_CAN0                  16
#define IRQ_CAN1                  17
#define IRQ_I2C7                  18
#define IRQ_I2C8                  19
#define IRQ_RESERVED20            20
#define IRQ_RTC_HZCLK             21
#define IRQ_RTC_SLEEP_ALARM       22

#define IRQ_TIMER0_1              23
#define IRQ_TIMER0_2              24
#define IRQ_TIMER0_3              25
#define IRQ_TIMER1_1              26
#define IRQ_TIMER1_2              27
#define IRQ_TIMER1_3              28
#define IRQ_NEW_TIMER1            29
#define IRQ_NEW_TIMER2            30
#define IRQ_NEW_TIMER3            31
#define IRQ_NDR_TIMER1            32
#define IRQ_NDR_TIMER2            33
#define IRQ_NDR_TIMER3            34

#define IRQ_WDT                   35

#define IRQ_I2C0                  36
#define IRQ_I2C1                  37
#define IRQ_I2C2                  38
#define IRQ_I2C3                  39
#define IRQ_I2C4                  40
#define IRQ_I2C5                  41

#define IRQ_UART0                 42
#define IRQ_UART1                 43
#define IRQ_UART2                 44
#define IRQ_UART3                 45
#define IRQ_UART4                 46
#define IRQ_UART5                 47
#define IRQ_UART6                 48
#define IRQ_UART7                 49
#define IRQ_UART8                 50
#define IRQ_UART9                 51

#define IRQ_IPC_ADSP2AP           52
#define IRQ_RIPC0_INT0            53

#define IRQ_SSP2                  54
#define IRQ_SSP3                  55
#define IRQ_SSP4                  56
#define IRQ_SSP5                  57

#define IRQ_GPIO_AP               58
#define IRQ_GPIO_AP_SEC           59
#define IRQ_GPIO_EDGE_WAKEUP      60
#define IRQ_TSEN                  61
#define IRQ_ONEWIRE               62
#define IRQ_KEYPAD                63
#define IRQ_PMIC                  64
#define IRQ_PM_XSC_WAKEUP         65
#define IRQ_SEC_RTC_HZCLK         66
#define IRQ_SEC_RTC_SLEEP_ALARM   67
#define IRQ_DRO                   68
#define IRQ_IRC                   69
#define IRQ_I2C6                  70
#define IRQ_RESERVED71            71

#define IRQ_DMA_AP_NONSEC         72
#define IRQ_DMA_AP_SEC            73

#define IRQ_VPU                   74
#define IRQ_GPU                   75
#define IRQ_GPU_JOB               76
#define IRQ_GPU_EVENT             77
#define IRQ_GPU_MMU               78

#define IRQ_ISP                   79
#define IRQ_ISP_MMU               80
#define IRQ_IPE                   81
#define IRQ_IPE2                  82
#define IRQ_IPE3                  83
#define IRQ_CPP                   84
#define IRQ_ISP_DMA               85
#define IRQ_V2D                   86
#define IRQ_JPEG                  87

#define IRQ_DPU_OFFL1             88
#define IRQ_DPU_OFFL0             89
#define IRQ_DPU_ONL2              90
#define IRQ_LCD_DE                91
#define IRQ_RESERVED92            92
#define IRQ_LCD_SEC               93
#define IRQ_DMMU                  94
#define IRQ_DSI_A                 95

#define IRQ_AUDIO_AD              96
#define IRQ_AUDIO_WAKEUP          97
#define IRQ_RESERVED98            98

#define IRQ_MMC1                  99
#define IRQ_MMC2                  100
#define IRQ_MMC3                  101
#define IRQ_SDH_WAKEUP1           102
#define IRQ_SDH_WAKEUP2           103
#define IRQ_SDH_WAKEUP3           104

#define IRQ_USB                   105
#define IRQ_USB_VBUS_WAKEUP       106
#define IRQ_AEU                   107
#define IRQ_FABRIC0_TIMEOUT       108
#define IRQ_DDR_ARM               109
#define IRQ_WTM_HST               110
#define IRQ_WTM_SP                111
#define IRQ_PMU2                  112
#define IRQ_BCM0                  113
#define IRQ_AUDIO_WDT             114
#define IRQ_MC                    115
#define IRQ_DDRPHY                116
#define IRQ_QSPI                  117
#define IRQ_USBP1                 118
#define IRQ_RESERVED119           119
#define IRQ_ARB_TIMEOUT1          120
#define IRQ_APB_MON               121
#define IRQ_MC_SCY                122
#define IRQ_ACLK_OFF_FAB          123
#define IRQ_RESERVED124           124
#define IRQ_USB3                  125
#define IRQ_HOOK_KEY              126
#define IRQ_AUDIO_PLUG            127
#define IRQ_SM2_PMDONE            128
#define IRQ_SM2_PADONE            129
#define IRQ_LCD_SPI               130

#define IRQ_EMAC0                 131
#define IRQ_EMAC0_WAKEUP          132
#define IRQ_EMAC1                 133
#define IRQ_EMAC1_WAKEUP          134

#define IRQ_HDMI0                 135
#define IRQ_HDMI1                 136
#define IRQ_HDMI_DPU_OFFL1        137
#define IRQ_HDMI_DPU_OFFL0        138
#define IRQ_HDMI_DPU_ONL2         139
#define IRQ_HDMI_SEC              140

#define IRQ_PCIE_PORTA            141
#define IRQ_PCIE_PORTB            142
#define IRQ_PCIE_PORTC            143
#define IRQ_CCI_ERROR             144
#define IRQ_PCIE_PORTA_WAKEUP     145
#define IRQ_PCIE_PORTB_WAKEUP     146
#define IRQ_PCIE_PORTC_WAKEUP     147

#define IRQ_USBP1_VBUS_WAKEUP     148
#define IRQ_USB3_VBUS_WAKEUP      149

#define IRQ_DMA2_AP_NONSEC        150
#define IRQ_DMA2_AP_SEC           151


#endif
