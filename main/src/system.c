/**************************************************************************//**
* @file     system.c
* @brief    template file for implementing the system functions
*
* @author   Ed Weaver
* @version  V00.00.00
* @date     21 May 2020
*****************************************************************************/

#include "system.h"

void initialize(void);

void delay(uint32_t ticks);

void SysTick_Handler(void);

uint32_t Tick_Counter(void);

void SysTick_Init(void);


static uint32_t volatile global_tick_counter;

uint8_t led_message[9] = "PCTBUS 1";

void SysTick_Init(void) {
    SysTick->CTRL = 0U;
    global_tick_counter = 0U;
    SysTick->VAL = 0U; /* Load the SysTick Counter Value */
    SysTick->CTRL = (SysTick_CTRL_TICKINT_Msk   |  /* Enable SysTick exception */
                     SysTick_CTRL_ENABLE_Msk) |    /* Enable SysTick system timer */
        SysTick_CTRL_CLKSOURCE_Msk;   /* Use processor clock source */
}

void initialize(void)
{
    SystemInit();
    SysTick_Config(SystemCoreClock / 1000U);
    MCU_Modify_SYSCLK_to_MSI(MCU_MSI_RANGE_4_MHZ);
    NVIC_SetPriority(SysTick_IRQn,(((uint32_t)1U<<__NVIC_PRIO_BITS) - 1U));
    NVIC_EnableIRQ(SysTick_IRQn);
    Morse_Initialize(&led_morse_stack, led_message, 9U);
    
    MCU_Initialize_GPIO();
    __enable_irq();
}

void SysTick_Handler(void) {
    ++global_tick_counter;
}

uint32_t Tick_Counter(void) {
    uint32_t return_value;
    
    __disable_irq();
    return_value = global_tick_counter;
    __enable_irq();
    
    return return_value;
}

void delay(uint32_t ticks)
{
    uint32_t counter = Tick_Counter();
    while ((Tick_Counter() - counter) < ticks)
    {
        ;
    }    
}

