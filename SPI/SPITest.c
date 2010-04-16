#include <p32xxxx.h>
#include <sys/types.h>
#pragma config POSCMOD=XT, FNOSC=PRIPLL, FPLLIDIV=DIV_1, FPLLMUL=MUL_20,FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_1, FWDTEN=OFF, CP=OFF, BWP=OFF

#include "UART/UART.H"
#include "SPI/SPI.H"
#include "OS/OS.H"
#include "MP/MatchPort.H"
#include "LCD/LCD.h"
#include "KP/KeyPad.H"
#include "ACD/AudioCoDec.H"

#include <p32xxxx.h>

int main() {
/*
	The following code example will initialize the SPI1 in Slave mode.
	It assumes that none of the SPI1 input pins are shared with an analog input. If so, the
	AD1PCFG and corresponding TRIS registers have to be properly configured.
*/
	int rData;
	IEC0CLR=0x03800000; // disable all interrupts
	SPI1CON = 0; // Stops and resets the SPI1.
	rData=SPI1BUF; // clears the receive buffer
	IFS0CLR=0x03800000; // clear any existing event
	SPI1STATCLR=0x40; // clear the Overflow
	SPI1CONSET=0xE0000480;// Framed mode, 
//	SPI1CONSET=0xC0000000;// Framed mode, 
	SPI1CONSET=0x8000; // SPI ON, 8 bits transfer, Slave mode
	// from now on, the device is ready to receive and transmit data
	
/*
	The following code example will initialize the SPI1 in Master mode.
	It assumes that none of the SPI1 input pins are shared with an analog input. If so, the
	AD1PCFG and corresponding TRIS registers have to be properly configured.
*/
//	int rData;
	IEC1CLR = 0xE0;//Disable interrupts for SPI 2.
	SPI2CON = 0; // Stops and resets the SPI1.
	rData=SPI2BUF; // clears the receive buffer
	IFS1CLR = 0xE0;//Clears any existing event.
	SPI2BRG=0x20; // use FPB/4 clock frequency
	SPI2STATCLR=0x40; // clear the Overflow
	SPI2CONSET=0xA00004A0; // Framed mode, 8 bits transfer, SMP=1, Master mode
//	SPI2CONSET=0x80000220; // Framed mode, 8 bits transfer, SMP=1, Master mode
	SPI2CONSET=0x8000; // SPI ON
	// from now on, the device is ready to transmit and receive data
	SPI2BUF='A'; // transmit an A character
	
//	SPI1BUF=0x4321;
//	while(1);
	while(1) {
//		rData = SPI1BUF + 1;
//		SPI1BUF = rData;
		SPI1BUF=0x8551;
//		rData = SPI2BUF + 1;
//		SPI2BUF = rData;
		SPI2BUF = ~SPI2BUF;
	}
	return 0;
}
