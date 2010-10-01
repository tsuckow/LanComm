#include "UART.H"
#include <plib.h>
//#include "../SPI/SPI.H"
//#include "../OS/OS.H"
//#include "../MP/MatchPort.H"
//#include "../LCD/LCD.h"
//#include "../KP/KeyPad.H"
//#include "../ACD/AudioCoDec.H"
//#define MASTER
//#define SLAVE

int uartInitialize() {
	TRISDSET = 0x4000;
	TRISDCLR = 0x8000;
	ODCDCLR = 0x8000;
	IEC0CLR = //Interrupt enable(-): Disable UART1 interrupts
		_IEC0_U1EIE_MASK |	//Error interrupt
		_IEC0_U1RXIE_MASK |	//Receive interrupt
		_IEC0_U1TXIE_MASK;	//Transmit interrupt
	IFS0CLR = //Interrupt flags(-): Clear UART1 interrupts
		_IFS0_U1EIF_MASK |	//Error interrupt
		_IFS0_U1RXIF_MASK |	//Receive interrupt
		_IFS0_U1TXIF_MASK;	//Transmit interrupt
	IPC6SET = //Iterrupt priority(+): Set UART1 interrupt priorities.
		_IPC6_U1IS_MASK |	//Sub-priority
		_IPC6_U1IP_MASK;	//Priority
	U1BRG = (uint16_t)(((float)80000000/(float)(16*BAUD_RATE))-0.5);//Set Baud
	U1STA = 0;//Clear status.
	U1MODE = //UART1 mode(=): Set the mode for UART1.
//		_U1MODE_RTSMD_MASK |	//Use simplex instead of flow control.
		_U1MODE_UEN1_MASK;	//Use TX,RX,RTS,and CTS.
	U1STASET = //Enable transmitter and receiver.
		_U1STA_UTXEN_MASK |	//Transmitter
		_U1STA_URXEN_MASK;	//Receiver
	U1MODE = //UART1 mode(=): Set the mode for UART1.
		_U1MODE_ON_MASK;	//On
	return 0;
}
void uartTest() {
	int i=0;
	uint8_t mask;
	while(1) {
#ifdef MASTER
		LATECLR = 0x04;//PORT E (-): Indicate playing mode.
		LATESET = 0x08;//PORT E (-): Un-Indicate recording mode.
		uartTXPollWrite(0x04);
		uartTXPollWrite(0x08);
		for(i=0;i<0x400000;i++);
		LATECLR = 0x08;//PORT E (-): Indicate playing mode.
		LATESET = 0x04;//PORT E (-): Un-Indicate recording mode.
		uartTXPollWrite(0x08);
		uartTXPollWrite(0x04);
		for(i=0;i<0x400000;i++);
#endif
#ifdef SLAVE
		mask = uartRXPollRead();
		LATECLR = mask;
		mask = uartRXPollRead();
		LATESET = mask;
#endif
	}
}
void uartShutdown() {
}

uint16_t uartTXFull() {
	return(U1STA & _U1STA_UTXBF_MASK);
}
void uartTXWrite(uint8_t data) {
	U1TXREG=data;
}
void uartTXPollWrite(uint8_t data) {
	while(uartTXFull());
	uartTXWrite(data);
}

uint8_t uartRXReady() {
	return(U1STA & _U1STA_URXDA_MASK);
}
uint8_t uartRXRead() {
	return(U1RXREG);
}
uint8_t uartRXPollRead() {
	while(!uartRXReady());
	return(uartRXRead());
}
