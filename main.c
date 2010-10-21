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
//	while(1);
	INTEnableSystemMultiVectoredInt();//Turn on multi vectored interrupts.
	int failCode;
	SYSTEMConfig(80000000L, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
	if (failCode=osInitialize()) {
		LATESET = 0xF;//PORT E(-): Turn on our 4 LEDs.
		TRISECLR = 0xF;//TRIS E(-): Configure the 4 LEDs for output.
		ODCECLR = 0xF;//ODC E(-): Congifure the 4 LEDs for non-open drain.
		LATECLR = 0xF & (failCode);
	} else {
		osRun();
		osShutdown();
	}
	return 0;
}

