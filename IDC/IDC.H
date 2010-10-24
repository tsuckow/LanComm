#ifndef IDC_H
#define IDC_H

#include "../UART/UART.H"
#include "Protocol.H"

typedef enum idcStatus {
	isIdle=			0x0,
	isConnecting=		0x2,
	isDisconnecting=	0x3
} idcStatus;

extern idcStatus currIdcStatus;
extern Connection connection;

void idcInitializeConnection();

void idcOpenConnection();
void idcCloseConnection();

#endif
