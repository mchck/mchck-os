/**
 * This file is sourced multiple times, with different macro expansions
 * for VH() and V().
 *
 * VH() is used to declare an ISR vector with a specified default handler
 *      implementation.
 * V() is used to simply declare an ISR vector which uses the
 *     default default handler.
 */

/* Generic ARM M0+ */
V(2, NMI)
V(3, Hard_Fault)
#ifndef SHORT_ISR
V(11, SVCall)
V(14, PendSV)
V(15, SysTick)
V(16, DMA0)
V(17, DMA1)
V(18, DMA2)
V(19, DMA3)
V(21, FTFA)
V(22, PMC)
V(23, LLWU)
V(24, I2C0)
V(25, I2C1)
V(26, SPI0)
V(27, SPI1)
V(28, LPUART0)
V(29, LPUART1)
V(30, UART2_FLEXIO)
V(31, ADC0)
V(32, CMP0)
V(33, TPM0)
V(34, TPM1)
V(35, TPM2)
V(36, RTC)
V(37, RTC_Seconds)
V(38, PIT)
V(39, I2S0)
V(40, USB0)
V(41, DAC0)
V(44, LPTMR0)
V(46, PORTA)
V(47, PORTC_PORTD)
#endif
