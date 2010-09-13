#ifndef UART_H
#define UART_H
#include <sys/types.h>

enum protocol {
	pQuery, pResponse, pReady, pYes, pNo
};

int uartInitialize();
void uartTest();
void uartShutdown();

uint8_t uartTXPoll();
void uartTXWrite(uint8_t data);
void uartTXPollWrite(uint8_t data);

uint8_t uartRXPoll();
uint8_t uartRXRead();
uint8_t uartRXPollRead();
#endif
