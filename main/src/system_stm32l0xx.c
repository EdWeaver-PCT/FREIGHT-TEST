/**
  ******************************************************************************
  * @file    system_stm32l0xx.c
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    25-November-2016
  * @brief   CMSIS Cortex-M0+ Device Peripheral Access Layer System Source File.
  *
  *   This file provides two functions and one global variable to be called from 
  *   user application:
  *      - SystemInit(): This function is called at startup just after reset and 
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32l0xx.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick 
  *                                  timer or configure other parameters.
  *                                     
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

  /* RCC->CR Register Settings    RM0377 pages 178-179 */
  /* ------------------------------------------------- */
  /* PLL State configure                clr 0x01000000 */
  /*     PLL Active                     set 0x01000000 */
  /*     PLL Inactive                   set 0x00000000 */
  /* RTC Prescalar configure            clr 0x00300000 */
  /*     HSE / 16 for RTC Clock         set 0x00300000 */
  /*     HSE / 8 for RTC Clock          set 0x00200000 */
  /*     HSE / 4 for RTC Clock          set 0x00100000 */
  /*     HSE / 2 for RTC Clock          set 0x00000000 */
  /* HSE Clock Security configure       clr 0x00080000 */
  /*     Clock Security Enabled         set 0x00080000 */
  /*     Clock Security Disabled        set 0x00000000 */
  /* HSE Oscillator Bypass configure    clr 0x00040000 */
  /*     Oscillator Bypassed w/ Clock   set 0x00040000 */
  /*     Oscillator Not Bypassed        set 0x00000000 */
  /* HSE Clock Enable configure         clr 0x00010000 */
  /*     HSE Active                     set 0x00010000 */
  /*     HSE Inactive                   set 0x00000000 */
  /* MSI Clock Enable configure         clr 0x00000100 */
  /*     MSI Active                     set 0x00000100 */
  /*     MSI Inactive                   set 0x00000000 */
  /* HSI16 Output Enable configure      clr 0x00000020 */
  /*     Output Enabled                 set 0x00000020 */
  /*     Output Disabled                set 0x00000000 */
  /* HSI16 Divider configure            clr 0x00000008 */
  /*     HSI16 divided by 4             set 0x00000008 */
  /*     HSI16 not divided              set 0x00000000 */
  /* HSI16 Forced On configure          clr 0x00000002 */
  /*     HSI16 forced on in Stop Mode   set 0x00000002 */
  /*     HSI16 not forced on            set 0x00000000 */
  /* HSI16 Clock Enable configure       clr 0x00000001 */
  /*     HSI16 Active                   set 0x00000001 */
  /*     HSI16 Inactive                 set 0x00000000 */

  /* RCC->CFGR Register Settings  RM0377 pages 182-184 */
  /* ------------------------------------------------- */
  /* PLLDIV configure                   clr 0x00C00000 */
  /*     PLLDIV divided by 2            set 0x00400000 */
  /*     PLLDIV divided by 3            set 0x00800000 */
  /*     PLLDIV divided by 4            set 0x00C00000 */
  /* PLLMUL configure                   clr 0x003C0000 */
  /*     PLLMUL multiplied by 3         set 0x00000000 */
  /*     PLLMUL multiplied by 4         set 0x00040000 */
  /*     PLLMUL multiplied by 6         set 0x00080000 */
  /*     PLLMUL multiplied by 8         set 0x000C0000 */
  /*     PLLMUL multiplied by 12        set 0x00100000 */
  /*     PLLMUL multiplied by 16        set 0x00140000 */
  /*     PLLMUL multiplied by 24        set 0x00180000 */
  /*     PLLMUL multiplied by 32        set 0x001C0000 */
  /*     PLLMUL multiplied by 48        set 0x00200000 */
  /* PLLSRC configure                   clr 0x00010000 */
  /*     clock is HSE                   set 0x00010000 */
  /*     clock is HSI16                 set 0x00000000 */
  /* STOPWUCK wake-up configure         clr 0x00008000 */
  /*     wake-up clock is MSI           set 0x00000000 */
  /*     wake-up clock is HSI16         set 0x00008000 */
  /* PPRE2 configure                    clr 0x00003800 */
  /*     PPRE2 not divided              set 0x00000000 */
  /*     PPRE2 divided by 2             set 0x00002000 */
  /*     PPRE2 divided by 4             set 0x00002800 */
  /*     PPRE2 divided by 8             set 0x00003000 */
  /*     PPRE2 divided by 16            set 0x00003800 */
  /* PPRE1 configure                    clr 0x00000700 */
  /*     PPRE1 not divided              set 0x00000000 */
  /*     PPRE1 divided by 2             set 0x00000400 */
  /*     PPRE1 divided by 4             set 0x00000500 */
  /*     PPRE1 divided by 8             set 0x00000600 */
  /*     PPRE1 divided by 16            set 0x00000700 */
  /* HPRE configure                     clr 0x000000F0 */
  /*     HPRE not divided               set 0x00000000 */
  /*     HPRE divided by 2              set 0x00000080 */
  /*     HPRE divided by 4              set 0x00000090 */
  /*     HPRE divided by 8              set 0x000000A0 */
  /*     HPRE divided by 16             set 0x000000B0 */
  /*     HPRE divided by 64             set 0x000000C0 */
  /*     HPRE divided by 128            set 0x000000D0 */
  /*     HPRE divided by 256            set 0x000000E0 */
  /*     HPRE divided by 512            set 0x000000F0 */
  /* SW System Clock configure          clr 0x00000003 */
  /*     SW System Clock to MSI         set 0x00000000 */
  /*     SW System Clock to HSI16       set 0x00000001 */
  /*     SW System Clock to HSE         set 0x00000002 */
  /*     SW System Clock to PLL         set 0x00000003 */
    
/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup stm32l0xx_system
  * @{
  */  
  
/** @addtogroup STM32L0xx_System_Private_Includes
  * @{
  */

#include "stm32l0xx.h"

#if !defined  (HSE_VALUE) 
  #define HSE_VALUE    ((uint32_t)8000000) /*!< Value of the External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (MSI_VALUE)
  #define MSI_VALUE    ((uint32_t)2000000) /*!< Value of the Internal oscillator in Hz*/
#endif /* MSI_VALUE */
   
#if !defined  (HSI_VALUE)
  #define HSI_VALUE    ((uint32_t)16000000) /*!< Value of the Internal oscillator in Hz*/
#endif /* HSI_VALUE */


/**
  * @}
  */

/** @addtogroup STM32L0xx_System_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32L0xx_System_Private_Defines
  * @{
  */
/************************* Miscellaneous Configuration ************************/

/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x00 /*!< Vector Table base offset field. 
                                   This value must be a multiple of 0x200. */
/******************************************************************************/
/**
  * @}
  */

/** @addtogroup STM32L0xx_System_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32L0xx_System_Private_Variables
  * @{
  */
  /* This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetHCLKFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency 
         Note: If you use this function to configure the system clock; then there
               is no need to call the 2 first functions listed above, since SystemCoreClock
               variable is updated automatically.
  */
//  uint32_t SystemCoreClock = 24000000;
//  uint32_t SystemCoreClock = 4000000;
  uint32_t SystemCoreClock = 4194304;
  const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
  const uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};
  const uint8_t PLLMulTable[9] = {3, 4, 6, 8, 12, 16, 24, 32, 48};

/**
  * @}
  */

/** @addtogroup STM32L0xx_System_Private_FunctionPrototypes
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32L0xx_System_Private_Functions
  * @{
  */

/**
  * @brief  Setup the microcontroller system.
  * @param  None
  * @retval None
  */
void SystemInit (void)
{
  /* Set System Clock to MSI at 4 MHz */
  {  
    /* MSI Clock range configure          clr 0x0000E000 */
    /* ------------------------------------------------- */
    /* Clear                                  0x0000E000 */
    /* Mask of Clear                          0xFFFF1FFF */
    /* ------------------------------------------------- */
    /*     MSI range 6 around 4.194 MHz   set 0x0000C000 */
    /* ------------------------------------------------- */
    /* Set                                    0x0000C000 */
    RCC->ICSCR = (RCC->ICSCR & ((uint32_t)0xFFFF1FFF)) | ((uint32_t)0x0000C000);
    
    /* STOPWUCK wake-up configure         clr 0x00008000 */
    /* PPRE2 configure                    clr 0x00003800 */
    /* PPRE1 configure                    clr 0x00000700 */
    /* HPRE configure                     clr 0x000000F0 */
    /* ------------------------------------------------- */
    /* Clear                                  0x0000BFF0 */
    /* Mask of Clear                          0xFFFF400F */
    /* ------------------------------------------------- */
    /*     wake-up clock is MSI           set 0x00000000 */
    /*     PPRE2 not divided              set 0x00000000 */
    /*     PPRE1 not divided              set 0x00000000 */
    /*     HPRE not divided               set 0x00000000 */
    /* ------------------------------------------------- */
    /* Set                                    0x00000000 */
    RCC->CFGR = (RCC->CFGR & ((uint32_t)0xFFFF400F)) | ((uint32_t)0x00000000);
    
    /* MSI Clock Enable configure         clr 0x00000100 */
    /* ------------------------------------------------- */
    /* Mask of Clear                          0xFFFFFEFF */
    /* ------------------------------------------------- */
    /*     MSI Active                     set 0x00000100 */
    /* ------------------------------------------------- */
    /* Set                                    0x00000000 */
    RCC->CR = (RCC->CR & ((uint32_t)0xFFFFFEFF)) | ((uint32_t)0x00000100);
    
    /* Wait unit MSI is ready */
//    while((RCC->CR & RCC_CR_MSIRDY) == 0)
    {
      ;
    }
    
    /* SW System Clock configure          clr 0x00000003 */
    /* Mask of Clear                          0xFFFFFFFC */
    /* ------------------------------------------------- */
    /*     SW System Clock to MSI         set 0x00000000 */
    RCC->CFGR = (RCC->CFGR & ((uint32_t)0xFFFFFFFC)) | ((uint32_t)0x00000000);
    
    /* Wait unit PLL is running as the System Clock */
//    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI)
    {
      ;
    }
  }
  
  /* Deactivate HSI16, HSE and PLL */
  {
    /* PLL State configure                clr 0x01000000 */
    /* HSE Clock Enable configure         clr 0x00010000 */
    /* MSI Clock Enable configure         clr 0x00000100 */
    /* ------------------------------------------------- */
    /* Clear                                  0x010101FF */
    /* Mask of Clear                          0xFEFEFEFF */
    /* ------------------------------------------------- */
    /*     PLL Inactive                   set 0x00000000 */
    /*     HSE Inactive                   set 0x00000000 */
    /*     MSI Inactive                   set 0x00000000 */
    /* ------------------------------------------------- */
    /* Set                                    0x00000000 */
    RCC->CR = (RCC->CR & ((uint32_t)0xFEFEFEFF)) | ((uint32_t)0x00000000);
  }
  
  /* Switch System Clock from MSI to HSI16 at 4 MHz */
  if (0)
  {
    /* HSI16 Output Enable configure      clr 0x00000020 */
    /* HSI16 Divider configure            clr 0x00000008 */
    /* HSI16 Forced On configure          clr 0x00000002 */
    /* HSI16 Clock Enable configure       clr 0x00000001 */
    /* ------------------------------------------------- */
    /* Clear                                  0x0000002B */
    /* Mask of Clear                          0xFFFFFFD4 */
    /* ------------------------------------------------- */
    /*     HSI16 Output Disabled          set 0x00000000 */
    /*     HSI16 divided by 4             set 0x00000008 */
    /*     HSI16 not forced on            set 0x00000000 */
    /*     HSI16 Active                   set 0x00000001 */
    /* ------------------------------------------------- */
    /* Set                                    0x00000009 */
    RCC->CR = (RCC->CR & ((uint32_t)0xFFFFFFD4)) | ((uint32_t)0x00000009);
    
    /* SW System Clock configure          clr 0x00000003 */
    /* Mask of Clear                          0xFFFFFFFC */
    /* ------------------------------------------------- */
    /*     SW System Clock to HSI16       set 0x00000001 */
    /* ------------------------------------------------- */
    /* Set                                    0x00000001 */
    RCC->CFGR = (RCC->CFGR & ((uint32_t)0xFFFFFFFC)) | ((uint32_t)0x00000001);
    
    /* Wait until HSI16 is stable */
//    while((RCC->CR & RCC_CR_HSIRDY) == 0)
    {
      ;
    }
  }
  
  /* Switch System Clock from MSI to HSI16 at 16 MHz */
  if (0)
  {
    
    /* HSI16 Output Enable configure      clr 0x00000020 */
    /* HSI16 Divider configure            clr 0x00000008 */
    /* HSI16 Forced On configure          clr 0x00000002 */
    /* HSI16 Clock Enable configure       clr 0x00000001 */
    /* ------------------------------------------------- */
    /* Clear                                  0x0000002B */
    /* Mask of Clear                          0xFFFFFFD4 */
    /* ------------------------------------------------- */
    /*     Output Disabled                set 0x00000000 */
    /*     HSI16 not divided              set 0x00000000 */
    /*     HSI16 not forced on            set 0x00000000 */
    /*     HSI16 Active                   set 0x00000001 */
    /* ------------------------------------------------- */
    /* Set                                    0x00000001 */
    RCC->CR = (RCC->CR & ((uint32_t)0xFFFFFFD4)) | ((uint32_t)0x00000001);
    
    /* SW System Clock configure          clr 0x00000003 */
    /* Mask of Clear                          0xFFFFFFFC */
    /* ------------------------------------------------- */
    /*     SW System Clock to HSI16       set 0x00000001 */
    /* ------------------------------------------------- */
    /* Set                                    0x00000001 */
    RCC->CFGR = (RCC->CFGR & ((uint32_t)0xFFFFFFFC)) | ((uint32_t)0x00000001);
  }
  
  /* Switch System Clock from MSI to PLL with Source HSI16 at 16 MHz */
  if (0)
  {
      /* HSI16 Output Enable configure      clr 0x00000020 */
      /* HSI16 Divider configure            clr 0x00000008 */
      /* HSI16 Forced On configure          clr 0x00000002 */
      /* HSI16 Clock Enable configure       clr 0x00000001 */
      /* ------------------------------------------------- */
      /* Clear                                  0x0000002B */
      /* Mask of Clear                          0xFFFFFFD4 */
      /* ------------------------------------------------- */
      /*     Output Disabled                set 0x00000000 */
      /*     HSI16 not divided              set 0x00000000 */
      /*     HSI16 not forced on            set 0x00000000 */
      /*     HSI16 Active                   set 0x00000001 */
      /* ------------------------------------------------- */
      /* Set                                    0x00000001 */
      RCC->CR = (RCC->CR & ((uint32_t)0xFFFFFFD4)) | ((uint32_t)0x00000001);
      
      /* SW System Clock configure          clr 0x00000003 */
      /* Mask of Clear                          0xFFFFFFFC */
      /* ------------------------------------------------- */
      /*     SW System Clock to HSI16       set 0x00000001 */
      /* ------------------------------------------------- */
      /* Set                                    0x00000001 */
      RCC->CFGR = (RCC->CFGR & ((uint32_t)0xFFFFFFFC)) | ((uint32_t)0x00000001);
      
      /* Wait until HSI16 is stable */
      while((RCC->CR & RCC_CR_HSIRDY) == 0)
      {
          ;
      }
      
      /* PLL State configure                clr 0x01000000 */
      /* ------------------------------------------------- */
      /* Clear                                  0x01000000 */
      /* Mask of Clear                          0xFEFFFFFF */
      /* ------------------------------------------------- */
      /*     PLL Inactive                   set 0x00000000 */
      /* ------------------------------------------------- */
      /* Set                                    0x00000000 */
      RCC->CR = (RCC->CR & ((uint32_t)0xFEFFFFFF)) | ((uint32_t)0x00000000);
      
      /* Verify PLL is Off */
      while((RCC->CR & RCC_CR_PLLRDY) != 0)
      {
          ;
      }
      
      /* PLLDIV configure                   clr 0x00C00000 */
      /* PLLMUL configure                   clr 0x003C0000 */
      /* PLLSRC configure                   clr 0x00010000 */
      /* STOPWUCK wake-up configure         clr 0x00008000 */
      /* PPRE2 configure                    clr 0x00003800 */
      /* PPRE1 configure                    clr 0x00000700 */
      /* HPRE configure                     clr 0x000000F0 */
      /* ------------------------------------------------- */
      /* Clear                                  0x00FDBFF0 */
      /* Mask of Clear                          0xFF02400F */
      /* ------------------------------------------------- */
      /*     PLLDIV divided by 4            set 0x00C00000 */
      /*     PLLMUL multiplied by 4         set 0x00040000 */
      /*     clock is HSI16                 set 0x00000000 */
      /*     wake-up clock is HSI16         set 0x00008000 */
      /*     PPRE2 not divided              set 0x00000000 */
      /*     PPRE1 not divided              set 0x00000000 */
      /*     HPRE not divided               set 0x00000000 */
      /* ------------------------------------------------- */
      /* Set                                    0x00408000 */
      RCC->CFGR = (RCC->CFGR & ((uint32_t)0xFF02400F)) | ((uint32_t)0x00C48000);
      
      /* PLL State configure                clr 0x01000000 */
      /* ------------------------------------------------- */
      /* Mask of Clear                          0xFEFFFFFF */
      /* ------------------------------------------------- */
      /*     PLL Active                     set 0x01000000 */
      /* ------------------------------------------------- */
      /* Set                                    0x00000000 */
      /* PLL Enabled set 0x01000000 */
      RCC->CR = (RCC->CR & ((uint32_t)0xFEFFFFFF)) | ((uint32_t)0x01000000);
      
      /* Wait unit PLL is ready */
      while((RCC->CR & RCC_CR_PLLRDY) != 0)
      {
          ;
      }
      
      /* SW System Clock configure          clr 0x00000003 */
      /* Mask of Clear                          0xFFFFFFFC */
      /* ------------------------------------------------- */
      /*     SW System Clock to PLL         set 0x00000003 */
      RCC->CFGR = (RCC->CFGR & ((uint32_t)0xFFFFFFFC)) | ((uint32_t)0x00000003);
      
      /* Wait unit PLL is running as the System Clock */
      while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
      {
          ;
      }
  }

  /* Disable all interrupts */
  RCC->CIER = 0x00000000;
    
    /* Configure the Vector Table location add offset address ------------------*/
#ifdef VECT_TAB_SRAM
  SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
#else
  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
#endif
}

/**
  * @brief  Update SystemCoreClock according to Clock Register Values
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *           
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.         
  *     
  * @note   - The system frequency computed by this function is not the real 
  *           frequency in the chip. It is calculated based on the predefined 
  *           constant and the selected clock source:
  *             
  *           - If SYSCLK source is MSI, SystemCoreClock will contain the MSI 
  *             value as defined by the MSI range.
  *                                   
  *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
  *                                              
  *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
  *                          
  *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**)
  *             or HSI_VALUE(*) multiplied/divided by the PLL factors.
  *         
  *         (*) HSI_VALUE is a constant defined in stm32l0xx_hal.h file (default value
  *             16 MHz) but the real value may vary depending on the variations
  *             in voltage and temperature.   
  *    
  *         (**) HSE_VALUE is a constant defined in stm32l0xx_hal.h file (default value
  *              8 MHz), user has to ensure that HSE_VALUE is same as the real
  *              frequency of the crystal used. Otherwise, this function may
  *              have wrong result.
  *                
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate (void)
{
  uint32_t tmp = 0, pllmul = 0, plldiv = 0, pllsource = 0, msirange = 0;

  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;
  
  switch (tmp)
  {
    case 0x00:  /* MSI used as system clock */
      msirange = (RCC->ICSCR & RCC_ICSCR_MSIRANGE) >> 13;
      SystemCoreClock = (32768 * (1 << (msirange + 1)));
      break;
    case 0x04:  /* HSI used as system clock */
      SystemCoreClock = HSI_VALUE;
      break;
    case 0x08:  /* HSE used as system clock */
      SystemCoreClock = HSE_VALUE;
      break;
    case 0x0C:  /* PLL used as system clock */
      /* Get PLL clock source and multiplication factor ----------------------*/
      pllmul = RCC->CFGR & RCC_CFGR_PLLMUL;
      plldiv = RCC->CFGR & RCC_CFGR_PLLDIV;
      pllmul = PLLMulTable[(pllmul >> 18)];
      plldiv = (plldiv >> 22) + 1;
      
      pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;

      if (pllsource == 0x00)
      {
        /* HSI oscillator clock selected as PLL clock entry */
        SystemCoreClock = (((HSI_VALUE) * pllmul) / plldiv);
      }
      else
      {
        /* HSE selected as PLL clock entry */
        SystemCoreClock = (((HSE_VALUE) * pllmul) / plldiv);
      }
      break;
    default: /* MSI used as system clock */
      msirange = (RCC->ICSCR & RCC_ICSCR_MSIRANGE) >> 13;
      SystemCoreClock = (32768 * (1 << (msirange + 1)));
      break;
  }
  /* Compute HCLK clock frequency --------------------------------------------*/
  /* Get HCLK prescaler */
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
  /* HCLK clock frequency */
  SystemCoreClock >>= tmp;
}



/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
