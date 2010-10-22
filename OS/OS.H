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

typedef enum state {
	sIdle,sSyncing,sReceiving,sPlaying,sRecording,sSending
} state;
/*	initialize
 *
 *	Initializes everything.  Returns 0 on success.  Returns an error code
 *	on failure.
 */
int osInitialize();

/*	run
 *
 *	This function will run until the device is ready to be shut down.
 */
void osRun();

void osPlayMode(state* current);
void osRecordMode(state* current);

#define MASTER
//#define SLAVE
/*	soundCheck
 *
 *	This function this function will constantly pipe sound from a master 
 *	device to a slave device.
 */
void soundCheck();

/*	shutdown
 *
 *	This function will perform any necessary shutdown operations and then
 *	exit.
 */
void osShutdown();

#endif
