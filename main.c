#include <p32xxxx.h>
#include <sys/types.h>
#pragma config POSCMOD=XT, FNOSC=PRIPLL, FPLLIDIV=DIV_2, FPLLMUL=MUL_20,FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_1, FWDTEN=OFF, CP=OFF, BWP=OFF

#include "UART/UART.H"
#include "SPI/SPI.H"
#include "OS/OS.H"
#include "MP/MatchPort.H"
#include "LCD/LCD.H"
#include "KP/KeyPad.H"
#include "ACD/AudioCoDec.H"
int main() {
	INTEnableSystemMultiVectoredInt();//Turn on multi vectored interrupts.
	if (osInitialize()) {
		LATECLR = 0xF;//PORT E(-): Turn on our 4 LEDs.
		TRISECLR = 0xF;//TRIS E(-): Configure the 4 LEDs for output.
		ODCECLR = 0xF;//ODC E(-): Congifure the 4 LEDs for non-open drain.
	} else {
		osRun();
		osShutdown();
	}
	return 0;
}

