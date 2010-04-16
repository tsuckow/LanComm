#ifndef OS_H
#define OS_H
#include <p32xxxx.h>
#include <sys/types.h>
#include "../UART/UART.H"
#include "../SPI/SPI.H"
#include "../MP/MatchPort.H"
#include "../LCD/LCD.h"
#include "../KP/KeyPad.H"
#include "../ACD/AudioCoDec.H"

/*	initialize
 *
 *	Initializes everything.  Returns 0 on success.  Returns an error code on failure.
 */
int osInitialize();

/*	run
 *
 *	This function will run until the device is ready to be shut down.
 */
void osRun();

/*	shutdown
 *
 *	This function will perform any necessary shutdown operations and then exit.
 */
void osShutdown();

#endif
