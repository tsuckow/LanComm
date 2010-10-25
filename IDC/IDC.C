#include "IDC.H"
#include <plib.h>
typedef enum state {
	sError,sIdle,sConnectIn,sConnectOut
} state;

Connection connection;
idcStatus currIdcStatus=isIdle;

#pragma interrupt uartInterrupt ipl7 vector 24
void uartInterrupt() {
	static state cState=sIdle;
	int trash;
	if (IFS0 & _IFS0_U1EIF_MASK) {	//Error interrupt
		cState = sError;
		LATESET = 0xF;//Turn off all the LEDs.
		if (U1STA & _U1STA_OERR_MASK) {
			LATECLR = 0x4;//Turn on White LED.
		}
		if (U1STA & _U1STA_FERR_MASK) {
			LATECLR = 0x8;//Turn on Green LED.
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
}
				break;
				case sConnectOut:
					if (U1RXREG==pbConfirmConnect) {
						currIdcStatus=isIdle;
						cState = sIdle;
						connection.status=csConnected;
						connection.role=crMaster;
						LATECLR = 0x4;//Turn on the white light.
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
		U1TXREG = pbRequestConnect;
		cState = sConnectOut;
	break;
	case isDisconnecting:
		U1TXREG = pbDisconnect;
		connection.status=csDisconnected;
		connection.role=crNull;
		currIdcStatus=isIdle;
		LATESET = 0x4;//Turn off the white light.
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
