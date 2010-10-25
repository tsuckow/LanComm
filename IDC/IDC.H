#ifndef IDC_H
#define IDC_H

#include "../UART/UART.H"
#include "Protocol.H"
#define NUM_BLOCKS			  0x10

typedef enum idcStatus {
	isIdle=			0x0,
	isBusy=			0x1,
	isConnecting=		0x2,
	isDisconnecting=	0x3,
	isSendingArray=		0x4,
	isReceivingArray=	0x5
} idcStatus;

extern idcStatus currIdcStatus;
extern Connection connection;

void idcInitializeConnection();

void idcOpenConnection();
void idcCloseConnection();

extern uint8_t* idcArrayBuffA;
extern uint32_t idcArrayBuffASize;
extern uint8_t* idcArrayBuffB;
extern uint32_t idcArrayBuffBSize;
void idcSendByteArray(uint8_t* data,uint32_t size);
//void idcReceiveByteArray(uint8_t* data,uint32_t* size,uint32_t maxSize);

#endif
