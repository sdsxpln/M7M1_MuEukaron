/******************************************************************************
Filename   : platform_X64_SUPER.h
Author     : pry
Date       : 24/06/2017
Licence    : LGPL v3+; see COPYING for details.
Description: The configuration file for X64 supercomputer profile.
******************************************************************************/

/* Defines *******************************************************************/
/* The virtual memory start address for the kernel objects */
#define RME_KMEM_VA_START            0x20003000
/* The size of the kernel object virtual memory */
#define RME_KMEM_SIZE                0xD000
/* The virtual memory start address for the virtual machines - If no virtual machines is used, set to 0 */
#define RME_HYP_VA_START             0x20020000
/* The size of the hypervisor reserved virtual memory */
#define RME_HYP_SIZE                 0x60000
/* The granularity of kernel memory allocation, in bytes */
#define RME_KMEM_SLOT_ORDER          4
/* The maximum number of preemption priority levels in the system.
 * This parameter must be divisible by the word length - 64 is usually sufficient */
#define RME_MAX_PREEMPT_PRIO         64

/* Shared interrupt flag region address - always use 256*4 = 1kB memory */
#define RME_CMX_INT_FLAG_ADDR        0x20010000
/* Initial kenel object frontier limit */
#define RME_CMX_KMEM_BOOT_FRONTIER   0x20003400
/* Number of MPU regions available */
#define RME_CMX_MPU_REGIONS          8
/* Init process's first thread's entry point address */
#define RME_CMX_INIT_ENTRY           0x08010001
/* Init process's first thread's stack address */
#define RME_CMX_INIT_STACK           0x2001FFF0
/* What is the FPU type? */
#define RME_CMX_FPU_TYPE             RME_CMX_FPV5_DP
/* What is the NVIC priority grouping? */
#define RME_CMX_NVIC_GROUPING        RME_CMX_NVIC_GROUPING_P2S6
/* What is the Systick value? - 10ms per tick*/
#define RME_CMX_SYSTICK_VAL          2160000

/* Kernel functions standard to Cortex-M, interrupt management and power */
#define RME_CMX_KERN_INT(X)          (X)
#define RME_CMX_INT_OP               0
#define RME_CMX_INT_ENABLE           1
#define RME_CMX_INT_DISABLE          0
#define RME_CMX_INT_PRIO             1
#define RME_CMX_KERN_PWR             240

/* Interrupt handler definitions - to facilitate transparent interrupts */
#define  WWDG_IRQHandler                         IRQ0_Handler        /* Window WatchDog */                                       
#define  PVD_IRQHandler                          IRQ1_Handler        /* PVD through EXTI Line detection */
#define  TAMP_STAMP_IRQHandler                   IRQ2_Handler        /* Tamper and TimeStamps through the EXTI line */
#define  RTC_WKUP_IRQHandler                     IRQ3_Handler        /* RTC Wakeup through the EXTI line */
#define  FLASH_IRQHandler                        IRQ4_Handler        /* FLASH */
#define  RCC_IRQHandler                          IRQ5_Handler        /* RCC */
#define  EXTI0_IRQHandler                        IRQ6_Handler        /* EXTI Line0 */
#define  EXTI1_IRQHandler                        IRQ7_Handler        /* EXTI Line1 */
#define  EXTI2_IRQHandler                        IRQ8_Handler        /* EXTI Line2 */
#define  EXTI3_IRQHandler                        IRQ9_Handler        /* EXTI Line3 */

#define  EXTI4_IRQHandler                        IRQ10_Handler       /* EXTI Line4 */
#define  DMA1_Stream0_IRQHandler                 IRQ11_Handler       /* DMA1 Stream 0 */
#define  DMA1_Stream1_IRQHandler                 IRQ12_Handler       /* DMA1 Stream 1 */
#define  DMA1_Stream2_IRQHandler                 IRQ13_Handler       /* DMA1 Stream 2 */
#define  DMA1_Stream3_IRQHandler                 IRQ14_Handler       /* DMA1 Stream 3 */
#define  DMA1_Stream4_IRQHandler                 IRQ15_Handler       /* DMA1 Stream 4 */
#define  DMA1_Stream5_IRQHandler                 IRQ16_Handler       /* DMA1 Stream 5 */
#define  DMA1_Stream6_IRQHandler                 IRQ17_Handler       /* DMA1 Stream 6 */
#define  ADC_IRQHandler                          IRQ18_Handler       /* ADC1, ADC2 and ADC3s */
#define  CAN1_TX_IRQHandler                      IRQ19_Handler       /* CAN1 TX */         

#define  CAN1_RX0_IRQHandler                     IRQ20_Handler       /* CAN1 RX0 */
#define  CAN1_RX1_IRQHandler                     IRQ21_Handler       /* CAN1 RX1 */
#define  CAN1_SCE_IRQHandler                     IRQ22_Handler       /* CAN1 SCE */
#define  EXTI9_5_IRQHandler                      IRQ23_Handler       /* External Line[9:5]s */
#define  TIM1_BRK_TIM9_IRQHandler                IRQ24_Handler       /* TIM1 Break and TIM9 */
#define  TIM1_UP_TIM10_IRQHandler                IRQ25_Handler       /* TIM1 Update and TIM10 */
#define  TIM1_TRG_COM_TIM11_IRQHandler           IRQ26_Handler       /* TIM1 Trigger and Commutation and TIM11 */
#define  TIM1_CC_IRQHandler                      IRQ27_Handler       /* TIM1 Capture Compare */
#define  TIM2_IRQHandler                         IRQ28_Handler       /* TIM2 */
#define  TIM3_IRQHandler                         IRQ29_Handler       /* TIM3 */

#define  TIM4_IRQHandler                         IRQ30_Handler       /* TIM4 */
#define  I2C1_EV_IRQHandler                      IRQ31_Handler       /* I2C1 Event */
#define  I2C1_ER_IRQHandler                      IRQ32_Handler       /* I2C1 Error */
#define  I2C2_EV_IRQHandler                      IRQ33_Handler       /* I2C2 Event */
#define  I2C2_ER_IRQHandler                      IRQ34_Handler       /* I2C2 Error */
#define  SPI1_IRQHandler                         IRQ35_Handler       /* SPI1 */
#define  SPI2_IRQHandler                         IRQ36_Handler       /* SPI2 */
#define  USART1_IRQHandler                       IRQ37_Handler       /* USART1 */
#define  USART2_IRQHandler                       IRQ38_Handler       /* USART2 */
#define  USART3_IRQHandler                       IRQ39_Handler       /* USART3 */

#define  EXTI15_10_IRQHandler                    IRQ40_Handler       /* External Line[15:10]s */
#define  RTC_Alarm_IRQHandler                    IRQ41_Handler       /* RTC Alarm (A and B) through EXTI Line */
#define  OTG_FS_WKUP_IRQHandler                  IRQ42_Handler       /* USB OTG FS Wakeup through EXTI line */
#define  TIM8_BRK_TIM12_IRQHandler               IRQ43_Handler       /* TIM8 Break and TIM12 */
#define  TIM8_UP_TIM13_IRQHandler                IRQ44_Handler       /* TIM8 Update and TIM13 */
#define  TIM8_TRG_COM_TIM14_IRQHandler           IRQ45_Handler       /* TIM8 Trigger and Commutation and TIM14 */
#define  TIM8_CC_IRQHandler                      IRQ46_Handler       /* TIM8 Capture Compare */
#define  DMA1_Stream7_IRQHandler                 IRQ47_Handler       /* DMA1 Stream7 */
#define  FMC_IRQHandler                          IRQ48_Handler       /* FMC */
#define  SDMMC1_IRQHandler                       IRQ49_Handler       /* SDMMC1 */

#define  TIM5_IRQHandler                         IRQ50_Handler       /* TIM5 */
#define  SPI3_IRQHandler                         IRQ51_Handler       /* SPI3 */
#define  UART4_IRQHandler                        IRQ52_Handler       /* UART4 */
#define  UART5_IRQHandler                        IRQ53_Handler       /* UART5 */
#define  TIM6_DAC_IRQHandler                     IRQ54_Handler       /* TIM6 and DAC1&2 underrun errors */
#define  TIM7_IRQHandler                         IRQ55_Handler       /* TIM7 */
#define  DMA2_Stream0_IRQHandler                 IRQ56_Handler       /* DMA2 Stream 0 */
#define  DMA2_Stream1_IRQHandler                 IRQ57_Handler       /* DMA2 Stream 1 */
#define  DMA2_Stream2_IRQHandler                 IRQ58_Handler       /* DMA2 Stream 2 */
#define  DMA2_Stream3_IRQHandler                 IRQ59_Handler       /* DMA2 Stream 3 */   

#define  DMA2_Stream4_IRQHandler                 IRQ60_Handler       /* DMA2 Stream 4 */
#define  ETH_IRQHandler                          IRQ61_Handler       /* Ethernet */
#define  ETH_WKUP_IRQHandler                     IRQ62_Handler       /* Ethernet Wakeup through EXTI line */
#define  CAN2_TX_IRQHandler                      IRQ63_Handler       /* CAN2 TX */
#define  CAN2_RX0_IRQHandler                     IRQ64_Handler       /* CAN2 RX0 */
#define  CAN2_RX1_IRQHandler                     IRQ65_Handler       /* CAN2 RX1 */
#define  CAN2_SCE_IRQHandler                     IRQ66_Handler       /* CAN2 SCE */
#define  OTG_FS_IRQHandler                       IRQ67_Handler       /* USB OTG FS */
#define  DMA2_Stream5_IRQHandler                 IRQ68_Handler       /* DMA2 Stream 5 */
#define  DMA2_Stream6_IRQHandler                 IRQ69_Handler       /* DMA2 Stream 6 */

#define  DMA2_Stream7_IRQHandler                 IRQ70_Handler       /* DMA2 Stream 7 */
#define  USART6_IRQHandler                       IRQ71_Handler       /* USART6 */
#define  I2C3_EV_IRQHandler                      IRQ72_Handler       /* I2C3 event */
#define  I2C3_ER_IRQHandler                      IRQ73_Handler       /* I2C3 error */
#define  OTG_HS_EP1_OUT_IRQHandler               IRQ74_Handler       /* USB OTG HS End Point 1 Out */
#define  OTG_HS_EP1_IN_IRQHandler                IRQ75_Handler       /* USB OTG HS End Point 1 In */
#define  OTG_HS_WKUP_IRQHandler                  IRQ76_Handler       /* USB OTG HS Wakeup through EXTI */
#define  OTG_HS_IRQHandler                       IRQ77_Handler       /* USB OTG HS */
#define  DCMI_IRQHandler                         IRQ78_Handler       /* DCMI */
#define  Reserved0_IRQHandler                    IRQ79_Handler       /* Reserved */

#define  RNG_IRQHandler                          IRQ80_Handler       /* Rng */
#define  FPU_IRQHandler                          IRQ81_Handler       /* FPU */
#define  UART7_IRQHandler                        IRQ82_Handler       /* UART7 */
#define  UART8_IRQHandler                        IRQ83_Handler       /* UART8 */
#define  SPI4_IRQHandler                         IRQ84_Handler       /* SPI4 */
#define  SPI5_IRQHandler                         IRQ85_Handler       /* SPI5 */
#define  SPI6_IRQHandler                         IRQ86_Handler       /* SPI6 */
#define  SAI1_IRQHandler                         IRQ87_Handler       /* SAI1 */
#define  LTDC_IRQHandler                         IRQ88_Handler       /* LTDC */
#define  LTDC_ER_IRQHandler                      IRQ89_Handler       /* LTDC error */

#define  DMA2D_IRQHandler                        IRQ90_Handler       /* DMA2D */
#define  SAI2_IRQHandler                         IRQ91_Handler       /* SAI2 */
#define  QUADSPI_IRQHandler                      IRQ92_Handler       /* QUADSPI */
#define  LPTIM1_IRQHandler                       IRQ93_Handler       /* LPTIM1 */
#define  CEC_IRQHandler                          IRQ94_Handler       /* HDMI_CEC */
#define  I2C4_EV_IRQHandler                      IRQ95_Handler       /* I2C4 Event */
#define  I2C4_ER_IRQHandler                      IRQ96_Handler       /* I2C4 Error */
#define  SPDIF_RX_IRQHandler                     IRQ97_Handler       /* SPDIF_RX */
#define  Reserved1_IRQHandler                    IRQ98_Handler       /* Reserved */
#define  DFSDM1_FLT0_IRQHandler                  IRQ99_Handler       /* DFSDM1 Filter 0 global Interrupt */

#define  DFSDM1_FLT1_IRQHandler                  IRQ100_Handler      /* DFSDM1 Filter 1 global Interrupt */
#define  DFSDM1_FLT2_IRQHandler                  IRQ101_Handler      /* DFSDM1 Filter 2 global Interrupt */
#define  DFSDM1_FLT3_IRQHandler                  IRQ102_Handler      /* DFSDM1 Filter 3 global Interrupt */
#define  SDMMC2_IRQHandler                       IRQ103_Handler      /* SDMMC2 */
#define  CAN3_TX_IRQHandler                      IRQ104_Handler      /* CAN3 TX */
#define  CAN3_RX0_IRQHandler                     IRQ105_Handler      /* CAN3 RX0 */
#define  CAN3_RX1_IRQHandler                     IRQ106_Handler      /* CAN3 RX1 */
#define  CAN3_SCE_IRQHandler                     IRQ107_Handler      /* CAN3 SCE */
#define  JPEG_IRQHandler                         IRQ108_Handler      /* JPEG */
#define  MDIOS_IRQHandler                        IRQ109_Handler      /* MDIOS */

/* This is for debugging output */
#define RME_X64_PUTCHAR(CHAR) \
do \
{ \
} \
while(0)
/* End Defines ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
