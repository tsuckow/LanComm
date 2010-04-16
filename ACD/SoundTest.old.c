#include <p32xxxx.h>
#pragma config POSCMOD=XT, FNOSC=PRIPLL, FPLLIDIV=DIV_2, FPLLMUL=MUL_18,FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_1, FWDTEN=OFF, CP=OFF, BWP=OFF

int main() {
	int rData,count;
	TRISESET = 0x0040;//Configure the button for input.
	TRISECLR = 0x000F;//Configure our status LEDs and the ACD enable.
	ODCECLR = 0x000F;//Configure the same for non-open drain.
	LATECLR = 0x0002;//Clear the enable for the ACD (note: ACD = Audio CoDec).
	TRISFCLR = 0x0002;//Configure the ACD reset for output.
	ODCFCLR = 0x0002;//Make it non-open drain for operation with our ACD.
	PMCONCLR = 0x8000;//Turn off the PM module so it doesn't conflict with pin 10.
	IC2CONCLR = 0x8000;//Turn off Input Capture 2 so that it doesn't conflict with pin 69.
	IC3CONCLR = 0x8000;//Turn off Input Capture 3 so that it doesn't conflict with pin 70.
	
//	TRISGCLR = 0x40;//Make a test pin output.
//	ODCGCLR = 0x40;//Make it non open drain.
//	while(1) LATGINV = 0x40;

	IEC1CLR = 0xE0;//Disable interrupts for SPI 2.
	IFS1CLR = 0xE0;//Clears any existing event.
	SPI2CONCLR = 0xFFFFFFFF;//Nuke the config regester and turn off SPI 2.
for (count = 0; count < 0xFFF; count++);//Delay, because we should.
	rData = SPI2BUF;//Clear the recieve buffer.
	SPI2STATCLR = 0x0040;//Clear the overflow bit.
	SPI2CONSET = 0xB00001A0;//Configure SPI2 so that we can use it's clock.
for (count = 0; count < 0xFFF; count++);
	SPI2CONSET = 0x8000;//Turn on the SPI2 to act as a clock for our peripherals.

	while(1) {
//		SPI2BUF = 0x5555;
	}

	TRISDSET = 0x0600;//Configure the SPI input pins.
	TRISCSET = 0x0010;//Configure the SPI input pins.

	IEC0CLR = 0x03800000;//Disable interrupts for SPI1.
	IFS0CLR = 0x03800000;//Clears any existing event.
	SPI1CONCLR = 0xFFFFFFFF;//Stop and reset SPI1.
for (count = 0; count < 0xFFF; count++);
	rData = SPI1BUF;//Clear the receive buffer.
	SPI1STATCLR = 0x0040;//Clear the overflow bit.
	//SPI1CONSET = 0xE0000480;//Configure SPI1 for operation with our ACD.
	SPI1CONSET = 0xE0000580;
for (count = 0; count < 0xFFF; count++);
	



	LATFCLR = 0x0002;//Activate the reset.
	for (count = 0; count < 0xFFF; count++);
	LATFSET = 0x0002;//De-activate the ACD reset.

	
	LATESET = 0x0002;//Set the enable for the ACD.
	SPI1CONSET = 0x8000;//Turn on the SPI1 for operation with our ACD.
	SPI1BUF = 0x8020;//Set the device count to 2 for device 0 in the ACD.
	SPI1BUF = 0x8820;//Set the device count to 2 for device 1 in the ACD.
	while(1) {
		if (SPI1STAT & 0x0100) {
			LATECLR = 0x000F;//Shit just hit the fan.
		}
		if (SPI1STAT & 0x0040) SPI1STATCLR = 0x0040;
		SPI1BUF = 0xB802;
		SPI1BUF = 0xB802;
		if (SPI1BUF) {//If button is not pressed...
			LATESET = 0x0008;//Turn off LED for play mode.
			LATECLR = 0x0004;//Turn on LED for record mode.
		} else {//Else if button is pressed...
			LATESET = 0x0004;//Turn off LED for record mode.
			LATECLR = 0x0008;//Turn on LED for play mode.
		}
		if (SPI1BUF) {
			LATECLR = 0x0001;
		} else {
			LATESET = 0x0001;
		}
	}
	
	return 0;
}
