#ifndef UART_H
#define UART_H
#include <sys/types.h>

#define BAUD_RATE 512000
#define MASTER
typedef enum protocol {
	pNull=0,pSync,pQuery, pResponse, pReady, pYes, pNo
} protocol;

int uartInitialize();
void uartTest();
void uartShutdown();
void uartQuery(protocol piece);
void uartRespond(protocol piece);
void uartSync();
protocol uartProcessQuery(protocol piece);
protocol uartProcessResponse(protocol piece);
uint8_t uartProcessSync(protocol piece);

uint8_t uartTXFull();
void uartTXWrite(uint8_t data);
void uartTXPollWrite(uint8_t data);

uint8_t uartRXReady();
uint8_t uartRXRead();
uint8_t uartRXPollRead();
#endif
