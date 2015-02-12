extern const struct uart_methods uart_fifo_methods;

extern struct uart_ctx uart0;
extern struct uart_ctx uart1;
extern struct uart_ctx uart2;
#ifdef UART3_BASE_PTR
extern struct uart_ctx uart3;
#endif
#ifdef UART4_BASE_PTR
extern struct uart_ctx uart4;
#endif
#ifdef UART5_BASE_PTR
extern struct uart_ctx uart5;
#endif
