#include <p32xxxx.h>
#include <sys/types.h>
#pragma config POSCMOD=XT, FNOSC=PRIPLL, FPLLIDIV=DIV_1, FPLLMUL=MUL_20,FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_1, FWDTEN=OFF, CP=OFF, BWP=OFF


#include "../SPI/SPI.H"
#include "../OS/OS.H"
#include "AudioCoDec.H"

uint16_t gData;

#define CLK_DIV 0x0007


int main() {
	int count;
	uint16_t rData;
	TRISECLR = 0x000D;//Configure our status LEDs.
	ODCECLR = 0x000D;//Configure the same for non-open drain.
	
	initACDinterface();
	initClockBus();
	resetACD();
	initACDdevice();

	SPI1STATCLR=0x40; // clear the Overflow
	rData = SPI1BUF;
	untilTBempty();
	SPI1BUF=0x0000;
	untilTBempty();
	SPI1BUF=0x0000;
	while(1) {
		untilTBempty();
		SPI1BUF=rData;
		untilRBfull();
		rData = SPI1BUF;
		untilTBempty();
		SPI1BUF=gData;
		untilRBfull();
		gData = SPI1BUF;
	}
	untilRBfull();
	rData = SPI1BUF;

	return 0;
}



