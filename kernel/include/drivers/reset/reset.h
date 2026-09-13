/* reset.h 
   This file refers to reset.h in the linux/u-boot repositories
   for context
*/

#ifndef RESET_H
#define RESET_H

#include <xv6/types.h>

/*
 * A reset is a hardware signal indicating that a HW module (or IP block, or
 * sometimes an entire off-CPU chip) reset all of its internal state to some
 * known-good initial state. Drivers will often reset HW modules when they
 * begin execution to ensure that hardware correctly responds to all requests,
 * or in response to some error condition. Reset signals are often controlled
 * externally to the HW module being reset, by an entity this API calls a reset
 * controller. This API provides a standard means for drivers to request that
 * reset controllers set or clear reset signals.
 */

#define RESET_BASE 0xd4050000UL

// Register offsets
/* APBC register offset */
#define APBC_UART1_CLK_RST      0x0
#define APBC_UART2_CLK_RST      0x4
#define APBC_GPIO_CLK_RST       0x8
#define APBC_PWM0_CLK_RST       0xc
#define APBC_PWM1_CLK_RST       0x10
#define APBC_PWM2_CLK_RST       0x14
#define APBC_PWM3_CLK_RST       0x18
#define APBC_TWSI8_CLK_RST      0x20
#define APBC_UART3_CLK_RST      0x24
#define APBC_RTC_CLK_RST        0x28
#define APBC_TWSI0_CLK_RST      0x2c
#define APBC_TWSI1_CLK_RST      0x30
#define APBC_TIMERS1_CLK_RST    0x34
#define APBC_TWSI2_CLK_RST      0x38
#define APBC_AIB_CLK_RST        0x3c
#define APBC_TWSI4_CLK_RST      0x40
#define APBC_TIMERS2_CLK_RST    0x44
#define APBC_ONEWIRE_CLK_RST    0x48
#define APBC_TWSI5_CLK_RST      0x4c
#define APBC_DRO_CLK_RST        0x58
#define APBC_IR_CLK_RST         0x5c
#define APBC_TWSI6_CLK_RST      0x60
#define APBC_TWSI7_CLK_RST      0x68
#define APBC_TSEN_CLK_RST       0x6c

#define APBC_UART4_CLK_RST      0x70
#define APBC_UART5_CLK_RST      0x74
#define APBC_UART6_CLK_RST      0x78
#define APBC_SSP3_CLK_RST       0x7c

#define APBC_SSPA0_CLK_RST      0x80
#define APBC_SSPA1_CLK_RST      0x84

#define APBC_IPC_AP2AUD_CLK_RST 0x90
#define APBC_UART7_CLK_RST      0x94
#define APBC_UART8_CLK_RST      0x98
#define APBC_UART9_CLK_RST      0x9c

#define APBC_CAN0_CLK_RST       0xa0
#define APBC_PWM4_CLK_RST       0xa8
#define APBC_PWM5_CLK_RST       0xac
#define APBC_PWM6_CLK_RST       0xb0
#define APBC_PWM7_CLK_RST       0xb4
#define APBC_PWM8_CLK_RST       0xb8
#define APBC_PWM9_CLK_RST       0xbc
#define APBC_PWM10_CLK_RST      0xc0
#define APBC_PWM11_CLK_RST      0xc4
#define APBC_PWM12_CLK_RST      0xc8
#define APBC_PWM13_CLK_RST      0xcc
#define APBC_PWM14_CLK_RST      0xd0
#define APBC_PWM15_CLK_RST      0xd4
#define APBC_PWM16_CLK_RST      0xd8
#define APBC_PWM17_CLK_RST      0xdc
#define APBC_PWM18_CLK_RST      0xe0
#define APBC_PWM19_CLK_RST      0xe4
/* end of APBC register offset */

/* MPMU register offset */
#define MPMU_WDTPCR     0x200
/* end of MPMU register offset */

/* APMU register offset */
#define APMU_JPG_CLK_RES_CTRL       0x20
#define APMU_CSI_CCIC2_CLK_RES_CTRL 0x24
#define APMU_ISP_CLK_RES_CTRL       0x38
#define APMU_LCD_CLK_RES_CTRL1      0x44
#define APMU_LCD_SPI_CLK_RES_CTRL   0x48
#define APMU_LCD_CLK_RES_CTRL2      0x4c
#define APMU_CCIC_CLK_RES_CTRL      0x50
#define APMU_SDH0_CLK_RES_CTRL      0x54
#define APMU_SDH1_CLK_RES_CTRL      0x58
#define APMU_USB_CLK_RES_CTRL       0x5c
#define APMU_QSPI_CLK_RES_CTRL      0x60
#define APMU_USB_CLK_RES_CTRL       0x5c
#define APMU_QSPI_CLK_RES_CTRL      0x60
#define APMU_USB_CLK_RES_CTRL       0x5c
#define APMU_DMA_CLK_RES_CTRL       0x64
#define APMU_AES_CLK_RES_CTRL       0x68
#define APMU_VPU_CLK_RES_CTRL       0xa4
#define APMU_GPU_CLK_RES_CTRL       0xcc
#define APMU_SDH2_CLK_RES_CTRL      0xe0
#define APMU_PMUA_MC_CTRL           0xe8
#define APMU_PMU_CC2_AP             0x100
#define APMU_PMUA_EM_CLK_RES_CTRL   0x104

#define APMU_AUDIO_CLK_RES_CTRL     0x14c
#define APMU_HDMI_CLK_RES_CTRL      0x1B8

#define APMU_PCIE_CLK_RES_CTRL_0    0x3cc
#define APMU_PCIE_CLK_RES_CTRL_1    0x3d4
#define APMU_PCIE_CLK_RES_CTRL_2    0x3dc

#define APMU_EMAC0_CLK_RES_CTRL     0x3e4
#define APMU_EMAC1_CLK_RES_CTRL     0x3ec
/* end of APMU register offset */

/* APBC2 register offset */
#define APBC2_UART1_CLK_RST             0x00
#define APBC2_SSP2_CLK_RST              0x04
#define APBC2_TWSI3_CLK_RST             0x08
#define APBC2_RTC_CLK_RST               0x0c
#define APBC2_TIMERS0_CLK_RST           0x10
#define APBC2_KPC_CLK_RST               0x14
#define APBC2_GPIO_CLK_RST              0x1c
/* end of APBC2 register offset */

/* For a specfic device, the mask and reassert/deassert value
   will be different, so refer to reset-ky-x1.c. The driver for
   xv6 is short and sweet, but a driver class framework(s) can be 
   implemented if you want to do it :)
*/
void reset_set(uint32 offset, uint32 mask, uint32 set);

#endif
