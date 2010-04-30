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
	while (!failCode)
		failCode=acdInitialize();
	return failCode;//Return no error.
}

/*	run
 *
 *	This function will run until the device is ready to be shut down.
 */
void osRun() {
	uint16_t acd1Data;
	uint16_t acd2Data;
	int16_t raw;
	spiClearOverflow();
	spiReadBlocked();
//	acd2Data = spiReadBlocked();
//	spiWrite(0x3FFF);//Write full positive to the codec.
//	acd1Data = spiReadBlocked();
//	spiWriteBlocked(0x4000);//Write full negative to the codec.
/*	while(1) {
		acd2Data = spiReadBlocked();
		spiWrite(acd2Data);
		acd1Data = spiReadBlocked();
		spiWrite(acd1Data);
	}/**/
/*	while(1) {
		spiReadBlocked();
		spiWrite(0x7FFF);
		spiReadBlocked();
		spiWrite(0x7FFF);
		spiReadBlocked();
		spiWrite(0x8000);
		spiReadBlocked();
		spiWrite(0x8000);
	}/**/
/*	while(1) {
		for(raw = -16384; raw < 16383; raw+=100) {
//			spiWriteBlocked(raw);//Write full positive to the codec.
//			spiReadBlocked();
			spiWriteBlocked(raw);//Write full positive to the codec.
			spiReadBlocked();
		}
		for(raw = 16383; raw > -16384; raw-=100) {
//			spiWriteBlocked(raw);//Write full positive to the codec.
//			spiReadBlocked();
			spiWriteBlocked(raw);//Write full positive to the codec.
			spiReadBlocked();
		}
	}/**/
	while(1);
}

/*	shutdown
 *
 *	This function will perform any necessary shutdown operations and then exit.
 */
void osShutdown() {
}

