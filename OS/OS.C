#include "OS.H"

/*	initialize
 *
 *	Initializes everything.  Returns 0 on success.  Returns an error code on
 *	failure.
 */
int osInitialize() {
	LATESET = 0xF;//PORT E(+): Turn off our 4 LEDs.
	TRISECLR = 0xF;//TRIS E(-): Configure the 4 LEDs for output.
	ODCECLR = 0xF;//ODC E(-): Congifure the 4 LEDs for non-open drain.
	int failCode;
	failCode=spiInitClockBus();
	if (!failCode) failCode=acdInitialize();
	if (!failCode) failCode=uartInitialize();
	return failCode;//Return no error.
}

/*	run
 *
 *	This function will run until the device is ready to be shut down.
 */
void osRun() {
	acdTest();
//	uartTest();
//	soundCheck();
	while(1);
}


/*	soundCheck
 *
 *	This function this function will constantly pipe sound from a master 
 *	device to a slave device.
 */
void soundCheck() {
	int i=0;
#ifdef MASTER
	acdStartRecording();
	while(1) {
		LATECLR = 0x04;//PORT E (-): Indicate ready for sync.
		LATESET = 0x08;//PORT E (-): Un-Indicate synced.
		uint8_t ready=0;
		while(!ready) {
			uartTXPollWrite((uint8_t)pQuery);
			uartTXPollWrite((uint8_t)pReady);
//			while(uartRXReady()) {
			while(!ready) {
				if (uartRXPollRead() != pResponse)
					continue;
				if (uartRXPollRead() != pReady)
					continue;
				if (uartRXPollRead() != pYes)
					continue;
				ready=1;
				break;
			}
		}
		LATECLR = 0x08;//PORT E (-): Indicate synced.
		LATESET = 0x04;//PORT E (-): Un-Indicate ready for sync.
		
		uint32_t data32;
		while (1) {
			unsigned int ready;
			uint16_t data;
			do {
				ready=acdCommandRead(ACD_HDAT1_ADDRESS);
			} while (ready == 0);
			data = acdCommandRead(ACD_HDAT0_ADDRESS);
			(&data32)[3] = (uint8_t)(data>>8);
			(&data32)[2] = (uint8_t)(data);

			do {
				ready=acdCommandRead(ACD_HDAT1_ADDRESS);
			} while (ready == 0);
			data = acdCommandRead(ACD_HDAT0_ADDRESS);
			(&data32)[1] = (uint8_t)(data>>8);
			(&data32)[0] = (uint8_t)(data);
			uartTXPollWrite(((uint8_t*)(&data32))[0]);
			uartTXPollWrite(((uint8_t*)(&data32))[1]);
			uartTXPollWrite(((uint8_t*)(&data32))[2]);
			uartTXPollWrite(((uint8_t*)(&data32))[3]);
			LATEINV = 0x04;//PORT E (~): Blink the white light.
		}
		
		acdReadFile(acdFileData);
	}
#endif
#ifdef SLAVE
	uint8_t modifiedHeader[60];
	acdBuildFileHeader(acdFileHeader,NUM_BLOCKS,NUM_CHANNELS, SAMPLE_FREQUENCY);
	acdModifyHeader(acdFileHeader,modifiedHeader);
	acdStartPlaying();
	acdSendFileHeader(modifiedHeader);
	while(1) {
		LATECLR = 0x04;//PORT E (-): Indicate ready for sync.
		LATESET = 0x08;//PORT E (-): Un-Indicate synced.
		uint8_t ready=0;
		while(!ready) {
			while(uartRXReady()) {
				if (uartRXPollRead() != pQuery)
					continue;
				if (uartRXPollRead() != pReady)
					continue;
				ready=1;
				break;
			}
		}
		uartTXPollWrite((uint8_t)pResponse);
		uartTXPollWrite((uint8_t)pReady);
		uartTXPollWrite((uint8_t)pYes);
		
		LATECLR = 0x08;//PORT E (-): Indicate synced.
		LATESET = 0x04;//PORT E (-): Un-Indicate ready for sync.
		
		uint32_t data32;
		while (1) {
			(uint8_t*)(&data32)[0]=uartRXPollRead();
			(uint8_t*)(&data32)[1]=uartRXPollRead();
			(uint8_t*)(&data32)[2]=uartRXPollRead();
			(uint8_t*)(&data32)[3]=uartRXPollRead();
			acdDataTransfer(data32);
			LATEINV = 0x01;//PORT E (~): Blink the ???? light.
		}
		
		acdPlayFile(acdFileData,NUM_BLOCKS);
	}
#endif
}

/*	shutdown
 *
 *	This function will perform any necessary shutdown operations and then
 *	exit.
 */
void osShutdown() {
	acdShutdown();
	uartShutdown();
}

