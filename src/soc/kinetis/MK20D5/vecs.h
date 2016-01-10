/**
 * This file is sourced multiple times, with different macro expansions
 * for VH() and V().
 *
 * VH() is used to declare an ISR vector with a specified default handler
 *      implementation.
 * V() is used to simply declare an ISR vector which uses the
 *     default default handler.
 */

/* XXX match with header IRQn names */

/* Generic ARM M4 */
V(2, NMI)
V(3, Hard_Fault)
V(4, Mem_Manage_Fault)
V(5, Bus_Fault)
V(6, Usage_Fault)
#ifndef SHORT_ISR
V(11, SVCall)
V(12, DebugMonitor)
V(14, PendSV)
V(15, SysTick)
/* K20P48 specific */
V(16, DMA0)
V(17, DMA1)
V(18, DMA2)
V(19, DMA3)
V(20, DMA_Error)
// V(21, DMA) unknown IRQ
V(22, FTFL)
V(23, FTFL_Collision)
V(24, LVW)
V(25, LLWU)
V(26, WDOG)
V(27, I2C0)
V(28, SPI0)
V(29, I2S0_TX)
V(30, I2S0_RX)
V(31, UART0_LON)
V(32, UART0_RX_TX)
V(33, UART0_Error)
V(34, UART1_RX_TX)
V(35, UART1_Error)
V(36, UART2_RX_TX)
V(37, UART2_Error)
V(38, ADC0)
V(39, CMP0)
V(40, CMP1)
V(41, FTM0)
V(42, FTM1)
V(43, CMT)
V(44, RTC)
V(45, RTC_Seconds)
V(46, PIT0)
V(47, PIT1)
V(48, PIT2)
V(49, PIT3)
V(50, PDB0)
V(51, USB0)
V(52, USBDCD)
V(53, TSI0)
V(54, MCG)
V(55, LPTMR0)
V(56, PORTA)
V(57, PORTB)
V(58, PORTC)
V(59, PORTD)
V(60, PORTE)
V(61, SWI)
#endif
