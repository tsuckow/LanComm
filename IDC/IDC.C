#include "IDC.H"
#include <plib.h>
typedef enum state {
	sError,sIdle,sConnectIn,sConnectOut,
	sSendingArray,sReceivingArray,sStoppingArray,
	sSendingArraySize,sReceivingArraySize,
	sSendingArrayData,sReceivingArrayData
} state;

Connection connection;
idcStatus currIdcStatus=isIdle;
uint8_t byteArrayA[NUM_BLOCKS*256+60];
uint8_t byteArrayB[NUM_BLOCKS*256+60];
uint8_t* idcArrayBuffA=byteArrayA;
uint32_t idcArrayBuffASize=0;
uint8_t* idcArrayBuffB=byteArrayB;
uint32_t idcArrayBuffBSize=0;
uint8_t* byteArray;
uint32_t byteArrayIndex=0;
uint32_t byteArraySize=0;
uint32_t* byteArrayReturnSize=0;
uint32_t byteArrayMaxSize=NUM_BLOCKS*256+60;

#pragma interrupt uartInterrupt ipl7 vector 24
void uartInterrupt() {
	static state cState=sIdle;
	uint32_t trash;
	if (IFS0 & _IFS0_U1EIF_MASK) {	//Error interrupt
		cState = sError;
		LATESET = 0xF;//Turn off all the LEDs.
		if (U1STA & _U1STA_OERR_MASK) {
			LATECLR = 0x4;//Turn on White LED.
		}
		if (U1STA & _U1STA_FERR_MASK) {
			LATECLR = 0x8;//Turn on Green LED.
		}
		if (U1STA & _U1STA_PERR_MASK) {
			LATECLR = 0xC;//Turn on Green and White LEDs.
		}
		LATECLR = 0x3;//Turn on the Red ond Orange LEDs, turn off ACD.
		while(1);//Spin, we're done.
		IFS0CLR = //Interrupt flags(-): Clear UART1 interrupts
			_IFS0_U1EIF_MASK;	//Error interrupt
	} else if (IFS0 & _IFS0_U1RXIF_MASK) {	//Receive interrupt
		do {
			switch(cState) {
				case sError:trash=U1RXREG;break;
				case sIdle:
					trash=U1RXREG;
switch (trash) {
	case pbRequestConnect:
		if (connection.status!=csConnected) {
			cState=sConnectIn;
			currIdcStatus=isConnecting;
			IFS0SET = _IFS0_U1TXIF_MASK;	//Fire interrupt.
		}
	break;
	case pbDisconnect:
		if (connection.status==csConnected) {
			connection.status=csDisconnected;
			connection.role=crNull;
			currIdcStatus=isIdle;
			LATESET = 0x4;//Turn off the white light.
		}
	break;
	case pbRequestByteArray:
		if (connection.status==csConnected) {
			if (idcArrayBuffASize==0) {
				currIdcStatus=isReceivingArray;
				cState=sReceivingArray;
				byteArray=idcArrayBuffA;
				byteArrayReturnSize=&idcArrayBuffASize;
				IFS0SET = _IFS0_U1TXIF_MASK;//Fire interrupt.
			} else if (idcArrayBuffBSize==0) {
				currIdcStatus=isReceivingArray;
				cState=sReceivingArray;
				byteArray=idcArrayBuffB;
				byteArrayReturnSize=&idcArrayBuffBSize;
				IFS0SET = _IFS0_U1TXIF_MASK;//Fire interrupt.
			} else {
				cState=sStoppingArray;
				IFS0SET = _IFS0_U1TXIF_MASK;//Fire interrupt.
			}
		}
	break;
}
				break;
				case sConnectOut:
					if (U1RXREG==pbConfirmConnect) {
						currIdcStatus=isIdle;
						cState = sIdle;
						connection.status=csConnected;
						connection.role=crMaster;
						//Turn on the white light.
						LATECLR = 0x4;
					}
				break;
				case sSendingArray:
					if (U1RXREG==pbConfirmByteArray) {
						cState=sSendingArraySize;
						//Fire interrupt.
						IFS0SET = _IFS0_U1TXIF_MASK;
					} else if (U1RXREG==pbStopByteArray) {
						cState=sIdle;
						currIdcStatus=isIdle;
					}
				break;
				case sReceivingArraySize:
					if(byteArrayIndex<4) {
						trash=U1RXREG<<
							(24-byteArrayIndex*8);
						byteArraySize|=trash;
						++byteArrayIndex;
					} else {
						if(byteArraySize>byteArrayMaxSize) {
							byteArraySize=
								byteArrayMaxSize;
						}
						*byteArrayReturnSize=byteArraySize;
						if (byteArraySize==0) {
							cState=sIdle;
							currIdcStatus=isIdle;
						} else if (byteArraySize==1) {
							byteArray[0]=U1RXREG;
							cState=sIdle;
							currIdcStatus=isIdle;
						} else {
							byteArray[0]=U1RXREG;
							byteArrayIndex=0;
							cState=sReceivingArrayData;
						}
					}
				break;
				case sReceivingArrayData:
					++byteArrayIndex;
					if(byteArrayIndex<byteArraySize) {
						byteArray[byteArrayIndex]=U1RXREG;
					} else {
						*byteArrayReturnSize=byteArraySize;
						cState=sIdle;
						currIdcStatus=isIdle;
					}
				break;
			}
		} while (U1STA & _U1STA_URXDA_MASK);
		IFS0CLR = //Interrupt flags(-): Clear UART1 interrupts
			_IFS0_U1RXIF_MASK;	//Receive interrupt
	} else if (IFS0 & _IFS0_U1TXIF_MASK) {	//Transmit interrupt
		switch(cState) {
			case sError: break;
			case sIdle:
switch(currIdcStatus) {
	case isConnecting:
		if (connection.status==csConnected) {
			currIdcStatus=isIdle;
		} else {
			U1TXREG = pbRequestConnect;
			cState = sConnectOut;
		}
	break;
	case isDisconnecting:
		if (connection.status==csConnected) {
			U1TXREG = pbDisconnect;
			connection.status=csDisconnected;
			connection.role=crNull;
			currIdcStatus=isIdle;
			LATESET = 0x4;//Turn off the white light.
		} else {
			currIdcStatus=isIdle;
		}
	break;
	case isSendingArray:
		U1TXREG=pbRequestByteArray;
		cState=sSendingArray;
	break;
}
			break;
			case sConnectIn:
				U1TXREG = pbConfirmConnect;
				connection.status=csConnected;
				connection.role=crSlave;
				LATECLR = 0x4;//Turn on the white light.
				cState=sIdle;
				currIdcStatus=isIdle;
			break;
			case sStoppingArray:
				U1TXREG=pbStopByteArray;
				cState=sIdle;
				currIdcStatus=isIdle;
			break;
			case sReceivingArray:
				U1TXREG=pbConfirmByteArray;
				cState=sReceivingArraySize;
				byteArrayIndex=0;
			break;
			case sSendingArraySize:
				cState=sSendingArrayData;
				byteArrayIndex=0;
				U1TXREG=(uint8_t)(byteArraySize>>24);
				U1TXREG=(uint8_t)(byteArraySize>>16);
				U1TXREG=(uint8_t)(byteArraySize>>8);
				U1TXREG=(uint8_t)(byteArraySize>>0);
			break;
			case sSendingArrayData:
				if(byteArrayIndex<byteArraySize) {
					U1TXREG=byteArray[byteArrayIndex];
					++byteArrayIndex;
				} else {
					cState=sIdle;
					currIdcStatus=isIdle;
				}
			break;
		}
		IFS0CLR = //Interrupt flags(-): Clear UART1 interrupts
			_IFS0_U1TXIF_MASK;	//Transmit interrupt
	}
}

void idcInitializeConnection() {
	connection.status=csDisconnected;
	connection.role=crNull;
}

void idcOpenConnection() {
	if(currIdcStatus) return;
	currIdcStatus=isConnecting;
	IFS0SET = _IFS0_U1TXIF_MASK;	//Fire interrupt.
}
void idcCloseConnection() {
	if(currIdcStatus) return;
	currIdcStatus=isDisconnecting;
	IFS0SET = _IFS0_U1TXIF_MASK;	//Fire interrupt.
}

void idcSendByteArray(uint8_t* data,uint32_t size) {
	if(currIdcStatus) return;
	currIdcStatus=isBusy;
	byteArray=data;
	byteArraySize=size;
	currIdcStatus=isSendingArray;
	IFS0SET = _IFS0_U1TXIF_MASK;	//Fire interrupt.
}

/*void idcReceiveByteArray(uint8_t* data,uint32_t* size,uint32_t maxSize) {
	if(currIdcStatus) return;
	currIdcStatus=isBusy;
	byteArray=data;
	byteArrayMaxSize=maxSize;
	currIdcStatus=isReceivingArray;
}/**/

