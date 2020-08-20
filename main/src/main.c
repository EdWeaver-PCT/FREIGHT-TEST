/**************************************************************************//**
* @file     main.c
* @brief    template file for main code
*
* Template implementing the freight base library and morse library.
*
* @author   Ed Weaver
* @version  V00.00.00
* @date     21 May 2020
*****************************************************************************/

#include "stm32l071.h"
#include <stdint.h>
#include "pct_mcu.h"
#include "morse.h"

extern mcu_queue eia485_output_queue;
extern mcu_queue eia485_input_queue;
extern mcu_queue serial_output_queue;
extern mcu_queue serial_input_queue;

morse_stack led_morse_stack;

void initialize(void);
void delay(uint32_t ticks);
void SysTick_Handler(void);
uint32_t Tick_Counter(void);
void SysTick_Init(void);

static uint32_t volatile global_tick_counter;

uint8_t led_message[7] = "TEST 01";

uint8_t morse_counter = 0U;

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
  Morse_Initialize(&led_morse_stack, led_message, 7U);
  
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

void MCU_CB_TIM2(void)
{
  uint32_t queue_length;
  int8_t status;
  
  status = MCU_Queue_Length(&serial_output_queue, &queue_length);
  if (status == MCU_QUEUE_SUCCESS)
  {
    if (queue_length > 0U)
    {
      MCU_Activate_RED_LED();
      MCU_EUART_Transmit();
    }
  }
  
  status = MCU_Queue_Length(&eia485_output_queue, &queue_length);
  if (status == MCU_QUEUE_SUCCESS)
  {
    if (queue_length > 0U)
    {
      MCU_Activate_RED_LED();
      MCU_485_Transmit();
    }
  }
  
  if (morse_counter < 2U)
  {
    morse_counter = morse_counter + 1U;
  }
  else
  {
    morse_counter = 0U;
    status = Morse_Next_Level(&led_morse_stack);
    if (status == MORSE_LEVEL_HIGH)
    {
      MCU_Activate_GRN_LED();
    }
    else if (status == MORSE_LEVEL_LOW)
    {
      MCU_Deactivate_GRN_LED();
    }
    else if (status == MORSE_NEXT_CHARACTER)
    {
      MCU_Deactivate_GRN_LED();
      Morse_Next_Character(&led_morse_stack);
    }
    else
    {
      MCU_Deactivate_GRN_LED();
      Morse_First_Character(&led_morse_stack);
    }
  }
}

void MCU_CB_EUART_Byte_Transmitted(void)
{
  uint32_t queue_length;
  int8_t status;
  
  status = MCU_Queue_Length(&serial_output_queue, &queue_length);
  if (status == MCU_QUEUE_SUCCESS)
  {
    MCU_EUART_Transmit();
    MCU_Deactivate_RED_LED();
  }
}

void MCU_CB_EUART_Byte_Received(uint8_t * character)
{
  if ((character[0] == '\r') || (character[0] == '\n'))
  {
    MCU_Queue_Push(&serial_output_queue, "Enter Pressed on UART\r\n", 23U);
  }
}

void MCU_CB_485_Byte_Transmitted(void)
{
  uint32_t queue_length;
  int8_t status;
  
  status = MCU_Queue_Length(&eia485_output_queue, &queue_length);
  if (status == MCU_QUEUE_SUCCESS)
  {
    MCU_485_Transmit();
    MCU_Deactivate_RED_LED();
  }
}

void MCU_CB_485_Byte_Received(uint8_t * character)
{
  if ((character[0] == '\r') || (character[0] == '\n'))
  {
    MCU_Queue_Push(&serial_output_queue, "Enter Pressed on EIA-485\r\n", 26U);
  }
}

void main(void)
{
  uint8_t state;
  uint8_t serial_input_buffer[100];
  uint8_t eia485_output_buffer[100];
  uint8_t serial_output_buffer[100];
  uint8_t eia485_input_buffer[100];
  int8_t status;
  uint32_t queue_length;
  
  initialize();
  state = 0U;
  
  MCU_Deactivate_LED_All();
  
  MCU_Queue_Initialize(&eia485_output_queue, eia485_output_buffer, 100U);
  MCU_Queue_Initialize(&eia485_input_queue, eia485_input_buffer, 100U);
  MCU_Queue_Initialize(&serial_output_queue, serial_output_buffer, 100U);
  MCU_Queue_Initialize(&serial_input_queue, serial_input_buffer, 100U);
  
  /* Clear Terminals */
  MCU_Queue_Push(&serial_output_queue, "\x1b[2J\n\r", 4U);
  MCU_Queue_Push(&eia485_output_queue, "\x1b[2J\n\r", 4U);

  Morse_First_Character(&led_morse_stack);
  MCU_Queue_Push(&serial_output_queue, "Initializing UART\n\r", 19U);
  MCU_Queue_Push(&eia485_output_queue, "Initializing EIA-485\n\r", 22U);
  
  while (1)
  {
    /* Reintialize MCU Queue if Memory Leak occurs */
    status = MCU_Queue_Length(&eia485_output_queue, &queue_length);
    if (status == MCU_QUEUE_MEMORY_LEAK)
    {
      MCU_Queue_Initialize(&eia485_output_queue, eia485_output_buffer, 100U);
    }
    status = MCU_Queue_Length(&serial_output_queue, &queue_length);
    if (status == MCU_QUEUE_MEMORY_LEAK)
    {
      MCU_Queue_Initialize(&serial_output_queue, serial_output_buffer, 100U);
    }
    status = MCU_Queue_Length(&eia485_input_queue, &queue_length);
    if (status == MCU_QUEUE_MEMORY_LEAK)
    {
      MCU_Queue_Initialize(&eia485_input_queue, eia485_input_buffer, 100U);
    }
    status = MCU_Queue_Length(&serial_input_queue, &queue_length);
    if (status == MCU_QUEUE_MEMORY_LEAK)
    {
      MCU_Queue_Initialize(&serial_input_queue, serial_input_buffer, 100U);
    }
    
    switch (state)
    {
    case 0:
      status = MCU_Queue_Push(&serial_output_queue, "Orange LED ON\n\r", 15U);
      status = MCU_Queue_Push(&eia485_output_queue, "LED Active\n\r", 12U);
      MCU_Activate_ORG_LED();
      state = state + 1U;
      break;
    default:
      status = MCU_Queue_Push(&serial_output_queue, "Orange LED OFF\n\r", 16U);
      status = MCU_Queue_Push(&eia485_output_queue, "LED Inactive\n\r", 14U);
      MCU_Deactivate_ORG_LED();
      state = 0U;
      break;
    }
    
    while (global_tick_counter < 1000U)
    {
      ;
    }
    __disable_irq();
    global_tick_counter = global_tick_counter - 1000U;
    __enable_irq();
  }
}
