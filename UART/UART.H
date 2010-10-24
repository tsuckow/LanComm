#ifndef UART_H
#define UART_H
#include <sys/types.h>

#define BAUD_RATE 512000

int uartInitialize();
void uartTest();
void uartShutdown();

#endif
