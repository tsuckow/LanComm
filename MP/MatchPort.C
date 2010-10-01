#include "MatchPort.H"
#include "../UART/UART.H"
//#include "../SPI/SPI.H"
//#include "../OS/OS.H"
//#include "../LCD/LCD.h"
//#include "../KP/KeyPad.H"
//#include "../ACD/AudioCoDec.H"

#pragma interrupt uartInterrupt ipl7 vector 24
int strIndex=0;
int strReceived=0;
char str[255];
void uartInterrupt() {
	static char prev;
	char curr;
	IFS0CLR = //Interrupt flags(-): Clear UART1 interrupts
		_IFS0_U1RXIF_MASK;	//Receive interrupt
//	uint8_t trash;
	U1STACLR = //Disable transmitter.
		_U1STA_UTXEN_MASK;
	U1STACLR = _U1STA_OERR_MASK;
	if (curr == 0x21 && prev == 0x3F) {
		prev = curr;
		curr = uartRXRead();
		if (curr == 0x3F) {
			strReceived=1;
			str[strIndex-1]=0;
		}
	} else {
		prev = curr;
		curr = uartRXRead();
	}
	if (strReceived==0) {
		str[strIndex] = curr;
		++strIndex;
	}
//	++strIndex;
//	IEC0CLR = //Interrupt enable(-): Disable UART1 interrupts
//		_IEC0_U1RXIE_MASK;	//Receive interrupt
}

int mpInitialize() {
	mpReset();
	return 0;
}
void mpTest() {
	int count;
	IEC0SET = //Interrupt enable(+): Enable UART1 interrupts
		_IEC0_U1RXIE_MASK;	//Receive interrupt
	strIndex=0;

	while(strIndex==0) {
		for(count=0;count<0x80000;count++);
		uartTXPollWrite('x');
	}
	while(!strReceived) count++;
	LATECLR = 0x8;
	strReceived=0;
	strIndex=0;
	U1STASET = //Enable transmitter.
		_U1STA_UTXEN_MASK;
	for(count=0;count<0x8000000;count++);
	uartTXPollWrite(0x0A);
	for(count=0;count<0x8000000;count++);
	uartTXPollWrite(0x0D);
	while(!strReceived);
	LATESET = 0x8;
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
