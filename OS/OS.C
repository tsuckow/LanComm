#include "OS.H"

/*	initialize
 *
 *	Initializes everything.  Returns 0 on success.  Returns an error code on failure.
 */
int osInitialize() {
	LATESET = 0xF;//PORT E(+): Turn off our 4 LEDs.
	TRISECLR = 0xF;//TRIS E(-): Configure the 4 LEDs for output.
	ODCECLR = 0xF;//ODC E(-): Congifure the 4 LEDs for non-open drain.
	int failCode;
	failCode=spiInitClockBus();
	if (!failCode) failCode=acdInitialize();
	return failCode;//Return no error.
}

/*	run
 *
 *	This function will run until the device is ready to be shut down.
 */
void osRun() {
	acdTest();
	while(1);
}

/*	shutdown
 *
 *	This function will perform any necessary shutdown operations and then exit.
 */
void osShutdown() {
}

