#include "MatchPort.H"
#include "../UART/UART.H"
//#include "../SPI/SPI.H"
//#include "../OS/OS.H"
//#include "../LCD/LCD.h"
//#include "../KP/KeyPad.H"
//#include "../ACD/AudioCoDec.H"

int mpInitialize() {
	mpReset();
	return 0;
}
void mpTest() {
	char str[100];
	int index;
//		for(index=0;index<0x80000;index++);
	while(!uartRXReady()) {
		for(index=0;index<0x80000;index++);
		uartTXPollWrite('x');
	}
	U1STACLR = //Disable transmitter and receiver.
		_U1STA_UTXEN_MASK;
	for(index=0;index<100;index++) {
		str[index]=uartRXPollRead();
	}
	LATECLR = 0x8;
	while(1);
}
void mpShutdown() {
}

void mpReset() {
	int count;
	LATECLR = PORTE_MPRESET_MASK;//PORT E (-): Activate the reset.
	for (count=0;count<200000;++count);
	LATESET = PORTE_MPRESET_MASK;//PORT E (+): Deactivate the reset.
}
