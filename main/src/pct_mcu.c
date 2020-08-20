#include "pct_mcu.h"

mcu_queue serial_queue;

mcu_queue rs485_queue;

void MCU_Initialize_GPIO(void)
{
    /* Timer 2 with 24 MHz  */
    {
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
        
        /* Disabled */
        TIM2->CR1 = TIM2->CR1 & 0xFFFEU;
        
        /* TIM2->CR1 Register Settings                       */
        /* ------------------------------------------------- */
        /* Clock division configure           clr 0x0300     */
        /*     Clock not divided              set 0x0000     */
        /*     Clock divided by 1             set 0x0100     */
        /*     Clock divided by 2             set 0x0200     */
        /* Auto Reload configured             clr 0x0080     */
        /*     TIM2->ARR not buffered         set 0x0000     */
        /*     TIM2->ARR buffered             set 0x0080     */
        /* Center aligned mode configure      clr 0x0060     */
        /*     Edge Mode                      set 0x0000     */
        /* Edge Mode Direction configure      clr 0x0010     */
        /*     Count Up                       set 0x0000     */
        /*     Count Down                     set 0x0010     */
        /* Timer Enabled/Disabled configure   clr 0x0001     */
        /*     Enabled                        set 0x0001     */
        /*     Disabled                       set 0x0000     */
        TIM2->CR1 = (TIM2->CR1 & 0xFC0EU) | 0x0200U;
        
        /* Set Prescalar to 1 msec for 16 MHz */
        /* TIM2->PSC = 0x1F40U; */
        /* Set Prescalar to 1 msec for 4 MHz */
        /* TIM2->PSC = 0x07D0U; */
        /* Set Prescalar to 1 msec for 4.2 MHz */
        TIM2->PSC = 0x0831U;
        
        /* 100 tick Countdown */        
        TIM2->CNT = 0xFF9BU;
        
        TIM2->DIER = TIM2->DIER | 0x0001U;
        
        /* Enabled */
        TIM2->CR1 = TIM2->CR1 | 0x0001U;
        
        NVIC_EnableIRQ(TIM2_IRQn);
    }
    
    /* MCU UART to Pins 7 & 8 on Connector  */
    {
        RCC->IOPENR |= RCC_IOPENR_IOPAEN;
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
        
        /* Set UART2 Peripheral Clock source as LSE value 0b11 on bit 3:2 */
        /* Set UART2 Peripheral Clock source as HSI16 value 0b10 on bit 3:2 */
        /* Set UART2 Peripheral Clock source as System value 0b01 on bit 3:2 */
        /* Set UART2 Peripheral Clock source as APB value 0b00 on bit 3:2 */
        RCC->CCIPR = (RCC->CCIPR & 0xFFFFFFF3U) | 0x00000004U;
        
        /* Enable GPIO A Clock */
        /* Configure as Alternate Function b10 for PA[3-2]*/
        GPIOA->MODER = (GPIOA->MODER & 0xFFFFFF0FU) | 0x000000A0U;
        /* Alternate Function AF4 b0100 for PA[3-2] */
        GPIOA->AFR[0] = (GPIOA->AFR[0] & 0xFFFF00FFU) | 0xFFFF44FFU;
        /* Configure as Push-Pull b0 for PC[3-2] */
        GPIOA->OTYPER = GPIOA->OTYPER & 0xFFFFFFF3U;
        /* Pull-up b00 for PA[3-2] */
        GPIOA->PUPDR = (GPIOA->PUPDR & 0xFFFFFF0FU) | 0x00000050U;
        /* Configure the GPIO as High Speed b11 for PA[3-2] */
        GPIOA->OSPEEDR = (GPIOA->OSPEEDR & 0xFFFFFF0FU) | 0x000000F0U;
        
        /* 115200 baud rate w/ 16 MHz UART Clock */
        /* USART2->BRR = 0x008B; */
        /* 115200 baud rate w/ 4 MHz UART Clock */
        USART2->BRR = 0x0023U;
        
        /* Oversampling mode = 16 clear 0x00008000 */
        /* 1 Start Bit, 8 data bits, n stop bits clear 0x10001000 */
        /* No Parity clear 0x00000400 */
        /* Transmitter enable set 0x00000008 */
        /* Receiver enable set 0x00000004 */
        /* Enable Interrupt on Transmission Completion set 0x00000040 */
        /* Enable Interrupt on Receive Buffer filed set 0x00000020 */
        USART2->CR1 = (USART2->CR1 & 0xEFFF6BFFU) | 0x0000006CU;
        
        /* Data Logic 1=H, 0=L clear 0x00040000 */
        /* TX Logic VCC=1, GND=0 clear 0x00020000 */
        /* RX Logic VCC=1, GND=0 clear 0x00010000 */
        /* TX & RX on correct pinouts clear 0x00008000 */
        /* LIN mode disabled clear 0x00004000 */
        /* 1 Stop Bit clear 0x00003000 */
        /* CK pin disabled 0x00000800 */
        USART2->CR2 = USART2->CR2 & 0xFFF807FFU;
        
        /* Smartcard mode disabled clear 0x00000020 */
        /* Half-Duplex mode disabled clear 0x00000008 */
        /* IrDA mode disabled clear 0x00000002 */
        USART2->CR3 = USART2->CR3 & 0xFFFFFFD5U;
        
        /* Enable UART set 00000001 */
        USART2->CR1 = USART2->CR1 | 0x00000001U;
        
        NVIC_EnableIRQ(USART2_IRQn);
        
        RCC->IOPENR |= RCC_IOPENR_IOPBEN;
        /* Initialize as PB[0] as High */
        GPIOB->BSRR = 0x00000001U;
        /* Configure the GPIO as Low Speed b00 for PB[0] */
        GPIOB->OSPEEDR = GPIOB->OSPEEDR & 0xFFFFFFFCU;
        /* Configure the Output as Push-Pull Totem for PB[0] */
        GPIOB->OTYPER = GPIOB->OTYPER & 0xFFFFFFFEU;
        /* Configure as General Purpose Output b01 for PB[0]*/
        GPIOB->MODER = (GPIOB->MODER & 0xFFFFFFFCU) | 0x00000001U;
        /* No Pull-up or Pull-down b00 for PB[0] */
        GPIOB->PUPDR = GPIOB->PUPDR & 0xFFFFFFFCU;
    }
    
    /* EIA-485 to Pins 9 & 10 on Connector  */
    {
        RCC->IOPENR |= RCC_IOPENR_IOPAEN;
        RCC->IOPENR |= RCC_IOPENR_IOPCEN;
        
        RCC->APB1ENR |= RCC_APB1ENR_USART4EN;
        
        /* Setup PC[11-10] TX/RX for UART4 */
        /* Configure as Alternate Function b10 for PC[11-10]*/
        GPIOC->MODER = (GPIOC->MODER & 0xFF0FFFFFU) | 0x00A00000U;
        /* Alternate Function AF6 b0110 for PC[11-10] */
        GPIOC->AFR[1] = (GPIOC->AFR[1] & 0xFFFF00FFU) | 0x00006600U;
        /* Configure as Push-Pull b0 for PC[11-10] */
        GPIOC->OTYPER = GPIOC->OTYPER & 0xFFFFF3FFU;
        /* Pull-up b01 for PC[11-10] */
        GPIOC->PUPDR = (GPIOC->PUPDR & 0xFF0FFFFFU) | 0x00500000U;
        /* Configure the GPIO as High Speed b11 for PC[11-10] */
        GPIOC->OSPEEDR = (GPIOC->OSPEEDR & 0xFF0FFFFFU) | 0x00F00000U;
        
        /* Setup PA[15] DE for UART4 */
        /* Alternate Function AF6 b0110 for PA[15] */
        GPIOA->AFR[1] = (GPIOA->AFR[1] & 0x0FFFFFFFU) | 0x60000000U;
        /* Configure as Alternate Function b10 for PA[15]*/
        GPIOA->MODER = (GPIOA->MODER & 0x3FFFFFFFU) | 0x80000000U;
        /* Configure as Push-Pull b0 for PA[15] */
        GPIOA->OTYPER = GPIOA->OTYPER & 0xFFFF7FFFU;
        /* Pull-up b01 for PA[15] */
        GPIOA->PUPDR = (GPIOA->PUPDR & 0x3FFFFFFFU) | 0x40000000U;
        /* Configure the GPIO as High Speed b11 for PA[15] */
        GPIOA->OSPEEDR = (GPIOA->OSPEEDR & 0x3FFFFFFFU) | 0xC0000000U;
        
        /* Set PA[12] as GPO Low */
        /* Configure as General Purpose Output b01 for PA[12]*/
        GPIOA->MODER = (GPIOA->MODER & 0xFCFFFFFFU) | 0x01000000U;
        /* Configure as Push-Pull b0 for PA[12] */
        GPIOA->OTYPER = GPIOA->OTYPER & 0xFFFFEFFFU;
        /* No Pull-up b00 for PA[12] */
        GPIOA->PUPDR = (GPIOA->PUPDR & 0xFCFFFFFFU) | 0x00000000U;
        /* Configure the GPIO as Low Speed b00 for PA[12] */
        GPIOA->OSPEEDR = (GPIOA->OSPEEDR & 0xFCFFFFFFU) | 0x00000000U;
        /* Set as PA[12] as Low */
        GPIOA->BRR = 0x00001000U;
        
        /* Set PA[15] as GPO Low */
        /* Configure as General Purpose Output b01 for PA[15]*/
        GPIOA->MODER = (GPIOA->MODER & 0x3FFFFFFFU) | 0x40000000U;
        /* Configure as Push-Pull b0 for PA[15] */
        GPIOA->OTYPER = GPIOA->OTYPER & 0xFFFFEFFFU;
        /* No Pull-up b00 for PA[15] */
        GPIOA->PUPDR = (GPIOA->PUPDR & 0x3FFFFFFFU) | 0x00000000U;
        /* Configure the GPIO as Low Speed b00 for PA[15] */
        GPIOA->OSPEEDR = (GPIOA->OSPEEDR & 0x3FFFFFFFU) | 0x00000000U;
        /* Set as PA[15] as Low */
        GPIOA->BRR = 0x00008000U;
        
        /* 115200 baud rate w/ 16 MHz UART Clock */
        /* USART2->BRR = 0x008B; */
        /* 115200 baud rate w/ 4 MHz UART Clock */
        USART4->BRR = 0x0023U;
        
        /* Oversampling mode = 16 clear 0x00008000 */
        /* 1 Start Bit, 8 data bits, n stop bits clear 0x10001000 */
        /* No Parity clear 0x00000400 */
        /* Transmitter enable set 0x00000008 */
        /* Receiver enable set 0x00000004 */
        /* Enable Interrupt on Transmission Completion set 0x00000040 */
        /* Enable Interrupt on Receive Buffer filed set 0x00000020 */
        USART4->CR1 = (USART4->CR1 & 0xEFFF6B93U) | 0x0000006CU;
        
        /* Data Logic 1=H, 0=L clear 0x00040000 */
        /* TX Logic VCC=1, GND=0 clear 0x00020000 */
        /* RX Logic VCC=1, GND=0 clear 0x00010000 */
        /* TX & RX on correct pinouts clear 0x00008000 */
        /* LIN mode disabled clear 0x00004000 */
        /* 1 Stop Bit clear 0x00003000 */
        /* CK pin disabled 0x00000800 */
        USART4->CR2 = USART4->CR2 & 0xFFF807FFU;
        
        /* Smartcard mode disabled clear 0x00000020 */
        /* Half-Duplex mode disabled clear 0x00000008 */
        /* IrDA mode disabled clear 0x00000002 */
        USART4->CR3 = USART4->CR3 & 0xFFFFFFD5U;
        
        /* Enable UART set 00000001 */
        USART4->CR1 = USART4->CR1 | 0x00000001U;
        
        NVIC_EnableIRQ(USART4_5_IRQn);
    }
    
    /* Arrow Off */
    {
        RCC->IOPENR |= RCC_IOPENR_IOPBEN;
        /* Initialize as PB[14] as Low */
        GPIOB->BRR = 0x00004000U;
        /* Configure the GPIO as Low Speed b00 for PB[14] */
        GPIOB->OSPEEDR = GPIOB->OSPEEDR & 0xCFFFFFFFU;
        /* Configure the Output as Push-Pull Totem for PB[14] */
        GPIOB->OTYPER = GPIOB->OTYPER & 0xFFFFBFFFU;
        /* Configure as General Purpose Output b01 for PB[14]*/
        GPIOB->MODER = (GPIOB->MODER & 0xCFFFFFFFU) | 0x10000000U;
        /* No Pull-up or Pull-down b00 for PB[14] */
        GPIOB->PUPDR = GPIOB->PUPDR & 0xCFFFFFFFU;
    }
    
    /* Battery Charger Off */
    {
        RCC->IOPENR |= RCC_IOPENR_IOPBEN;
        /* Initialize as PB[15] as High */
        GPIOB->BSRR = 0x10000000U;
        /* Configure the GPIO as Low Speed b00 for PB[15] */
        GPIOB->OSPEEDR = GPIOB->OSPEEDR & 0x3FFFFFFFU;
        /* Configure the Output as Push-Pull Totem for PB[15] */
        GPIOB->OTYPER = GPIOB->OTYPER & 0xFFFF7FFFU;
        /* Configure as General Purpose Output b01 for PB[15]*/
        GPIOB->MODER = (GPIOB->MODER & 0x3FFFFFFFU) | 0x40000000U;
        /* No Pull-up or Pull-down b00 for PB[15] */
        GPIOB->PUPDR = GPIOB->PUPDR & 0x3FFFFFFFU;
    }
    
    /* LEDs Off */
    {
        RCC->IOPENR |= RCC_IOPENR_IOPCEN;
        /* Initialize as High */
        GPIOC->BSRR = 0x00000340U;
        /* Configure the GPIO as Low Speed */
        GPIOC->OSPEEDR = GPIOC->OSPEEDR & 0xFFF0CFFFU;
        /* Configure the Output as Open Drain */
        GPIOC->OTYPER = GPIOC->OTYPER | 0x00000340U;
        /* Configure as General Purpose Output b01 */
        /* Configure as General Purpose Output b01 for PC[9-8,6]*/
        GPIOC->MODER = (GPIOC->MODER & 0xFFF0CFFFU) | 0x00051000U;
        /* Activate the Pull-up b01 */
        GPIOC->PUPDR = (GPIOC->PUPDR & 0xFFF0CFFFU) | 0x00051000U;
    }
}

void USART2_IRQHandler(void)
{
    uint8_t character;
    
    if (USART2->ISR & 0x00000040U)
    {
        /* Clear Transmission Complete Flag clear 0x00000040 */
        USART2->ICR = USART2->ICR | 0x00000040U;
        
        
        MCU_Callback_USART2_TC();
    }
    if (USART2->ISR & 0x00000020U)
    {
        USART2->ICR = USART2->ICR | 0x00000020U;
        character = USART2->RDR;
        MCU_Callback_USART2_RD(&character);
    }
}

__weak void MCU_Callback_USART2_TC(void)
{
    ;
}

__weak void MCU_Callback_USART2_RD(uint8_t * character)
{
    ;
}

void USART4_5_IRQHandler(void)
{
    uint8_t character;
    
    if (USART4->ISR & 0x00000040U)
    {
        /* Clear Transmission Complete Flag clear 0x00000040 */
        USART4->ICR = USART4->ICR | 0x00000040U;
        
        /* Set DE and /R/E as Low */
        GPIOA->BRR = 0x00009000U;
        
        MCU_Callback_USART4_TC();
    }
    if (USART4->ISR & 0x00000020U)
    {
        USART4->ICR = USART4->ICR | 0x00000020U;
        character = USART4->RDR;
        MCU_Callback_USART4_RD(&character);
    }
}

__weak void MCU_Callback_USART4_TC(void)
{
    ;
}

__weak void MCU_Callback_USART4_RD(uint8_t * character)
{
    ;
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & 0x0001U)
    {
        /* 100 tick Countdown */
        TIM2->CNT = 0xFF9BU;
        
        /* 200 tick Countdown */
        TIM2->CNT = 0xFF37U;
        
        /* Clear Interrupt Flag */
        TIM2->SR = TIM2->SR & 0xFFFEU;
        
        MCU_Callback_TIM2();
    }
}

__weak void MCU_Callback_TIM2(void)
{
    ;
}

void MCU_Transmit_485(void)
{
    int8_t return_value;
    uint8_t character;
    
    return_value = MCU_Queue_Pop(&rs485_queue, &character);
    
    if (return_value == MCU_QUEUE_SUCCESS)
    {
        /* Set DE and /R/E as High */
        GPIOA->BSRR = 0x00009000U;
        
        USART4->TDR = (uint8_t) (character & 0x000000FFU);
    }
}

void MCU_Transmit_UART(void)
{
    int8_t return_value;
    uint8_t character;
    
    return_value = MCU_Queue_Pop(&serial_queue, &character);
    
    if (return_value == MCU_QUEUE_SUCCESS)
    {
        USART2->TDR = 0x000000FFU & ((uint32_t) character);
    }
}

uint8_t MCU_Modify_SYSCLK_to_MSI(uint8_t range)
{
    /* 000: range 0 around 65.536 kHz */
    /* 001: range 1 around 131.072 kHz */
    /* 010: range 2 around 262.144 kHz */
    /* 011: range 3 around 524.288 kHz */
    /* 100: range 4 around 1.048 MHz */
    /* 101: range 5 around 2.097 MHz (reset value) */
    /* 110: range 6 around 4.194 MHz */
    
    uint8_t return_value;
    uint32_t value;
    
    if (range > 0x06U)
    {
        return_value = MCU_MODIFY_SYSCLK_FAILURE;
    }
    else
    {
        switch (range)
        {
        case 1:
            value = 0x00002000U;
            break;
        case 2:
            value = 0x00004000U;
            break;
        case 3:
            value = 0x00006000U;
            break;
        case 4:
            value = 0x00008000U;
            break;
        case 5:
            value = 0x0000A000U;
            break;
        case 6:
            value = 0x0000C000U;
            break;
        default:
            value = 0x00000000U;
            break;
        }
        /* MSI Clock range configure          clr 0x0000E000 */
        /* ------------------------------------------------- */
        /* Clear                                  0x0000E000 */
        /* Mask of Clear                          0xFFFF1FFF */
        /* ------------------------------------------------- */
        /* Set                                         value */
        RCC->ICSCR = (RCC->ICSCR & 0xFFFF1FFFU) | value;
        
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
        RCC->CFGR = (RCC->CFGR & 0xFFFF400FU) | 0x00000000U;
        
        /* MSI Clock Enable configure         clr 0x00000100 */
        /* ------------------------------------------------- */
        /* Mask of Clear                          0xFFFFFEFF */
        /* ------------------------------------------------- */
        /*     MSI Active                     set 0x00000100 */
        /* ------------------------------------------------- */
        /* Set                                    0x00000000 */
        RCC->CR = (RCC->CR & 0xFFFFFEFFU) | 0x00000100U;
        
        /* Wait unit MSI is ready */
        while((RCC->CR & RCC_CR_MSIRDY) == 0U)
        {
            ;
        }
        
        /* SW System Clock configure          clr 0x00000003 */
        /* Mask of Clear                          0xFFFFFFFC */
        /* ------------------------------------------------- */
        /*     SW System Clock to MSI         set 0x00000001 */
        RCC->CFGR = (RCC->CFGR & 0xFFFFFFFCU) | 0x00000001U;
        
        /* Wait unit PLL is running as the System Clock */
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI)
        {
            ;
        }
        return_value = MCU_MODIFY_SYSCLK_SUCCESS;
    }
    
    return return_value;
}


uint8_t MCU_Modify_SYSCLK_to_HSI(uint8_t range)
{
    /* 000: range 0 around 4 MHz */
    /* 001: range 1 around 16 MHz */
    
    uint8_t return_value;
    uint32_t value;
    
    if (range > 0x01U)
    {
        return_value = MCU_MODIFY_SYSCLK_FAILURE;
    }
    else
    {
        switch (range)
        {
        case 1:
            value = 0x00000000U;
            break;
        default:
            value = 0x00000008U;
            break;
        }
        /* MSI Clock range configure          clr 0x0000E000 */
        /* ------------------------------------------------- */
        /* Clear                                  0x0000E000 */
        /* Mask of Clear                          0xFFFF1FFF */
        /* ------------------------------------------------- */
        /*     MSI range 6 around 4.194 MHz   set 0x0000C000 */
        /* ------------------------------------------------- */
        /* Set                                    0x0000C000 */
        RCC->ICSCR = (RCC->ICSCR & 0xFFFF1FFFU) | 0x0000A000U;
        
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
        RCC->CFGR = (RCC->CFGR & 0xFFFF400FU) | 0x00000000U;
        
        /* MSI Clock Enable configure         clr 0x00000100 */
        /* ------------------------------------------------- */
        /* Mask of Clear                          0xFFFFFEFF */
        /* ------------------------------------------------- */
        /*     MSI Active                     set 0x00000100 */
        /* ------------------------------------------------- */
        /* Set                                    0x00000000 */
        RCC->CR = (RCC->CR & 0xFFFFFEFFU) | 0x00000100U;
        
        /* Wait unit MSI is ready */
        while((RCC->CR & RCC_CR_MSIRDY) == 0U)
        {
            ;
        }
        
        /* SW System Clock configure          clr 0x00000003 */
        /* Mask of Clear                          0xFFFFFFFC */
        /* ------------------------------------------------- */
        /*     SW System Clock to MSI         set 0x00000001 */
        RCC->CFGR = (RCC->CFGR & 0xFFFFFFFCU) | 0x00000001U;
        
        /* Wait unit PLL is running as the System Clock */
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI)
        {
            ;
        }
        return_value = MCU_MODIFY_SYSCLK_SUCCESS;

        /* HSI Output Enable configure        clr 0x00000020 */
        /* HSI Divider configure              clr 0x00000008 */
        /* HSI Forced On configure            clr 0x00000002 */
        /* HSI Clock Enable configure         clr 0x00000001 */
        /* ------------------------------------------------- */
        /* Clear                                  0x0000002B */
        /* Mask of Clear                          0xFFFFFFD4 */
        /* ------------------------------------------------- */
        /*     HSI Output Disabled            set 0x00000000 */
        /*     HSI divider                    set      value */
        /*     HSI not forced on              set 0x00000000 */
        /*     HSI Active                     set 0x00000001 */
        /* ------------------------------------------------- */
        /* Set                            value | 0x00000001 */
        RCC->CR = (RCC->CR & 0xFFFFFFD4U) | (0x00000001U | value);
        
        /* SW System Clock configure          clr 0x00000003 */
        /* Mask of Clear                          0xFFFFFFFC */
        /* ------------------------------------------------- */
        /*     SW System Clock to HSI         set 0x00000001 */
        /* ------------------------------------------------- */
        /* Set                                    0x00000001 */
        RCC->CFGR = (RCC->CFGR & 0xFFFFFFFCU) | 0x00000001U;
        
        /* Wait until HSI16 is stable */
        while((RCC->CR & RCC_CR_HSIRDY) == 0U)
        {
            ;
        }
    }
    
    return return_value;
}
