#include "OS.H"

#define BUFFER_SIZE NUM_BLOCKS*128;
uint8_t modifiedHeader[60];
//uint8_t* acdFileBuffA = acdFileData;
//uint8_t* acdFileBuffB = acdFileData+BUFFER_SIZE;
#define MAIN_BUFFA_LOCK	0x1
#define MAIN_BUFFB_LOCK	0x2
#define INT_BUFFA_LOCK	0x4
#define INT_BUFFB_LOCK	0x8
//uint8_t lock=INT_BUFFA_LOCK;
#pragma interrupt uartInterrupt ipl7 vector 24
/*void uartInterrupt() {
	static int iByte;
	if (lock & INT_BUFFA_LOCK) {
		if (!lock){}
	} else {
	}
}*/
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
	if (!failCode) failCode=kpInitialize();
//	if (!failCode) failCode=mpInitialize();//No more network.
	return failCode;//Return no error.
}

/*	run
 *
 *	This function will run until the device is ready to be shut down.
 */
void osRun() {
//	kpTest();
//	acdTest();
//	uartTest();
//	soundCheck();
//	mpTest();//No more network.
	acdWarmUpAD();
	state current=sIdle;
	char meh;
	unsigned int iWord;
	uint32_t data32;
	uint8_t* dataByteArray=(uint8_t*)(&data32);
	acdBuildFileHeader(
		acdFileHeader,NUM_BLOCKS,NUM_CHANNELS,SAMPLE_FREQUENCY
	);
	acdModifyHeader(acdFileHeader,modifiedHeader);
	while(kpLastChar=='\0');
	while(1) {
//		U1STACLR = _U1STA_UTXEN_MASK |//Disable Transmitter
//			_U1STA_OERR_MASK;//Clear overflow.
//		U1STASET = _U1STA_URXEN_MASK;//Enable Receiver
		LATESET = 0x04;//PORT E (-): Un-indicate sync.
		LATECLR = 0x01;//PORT E (-): Indicate playing mode.
		LATESET = 0x08;//PORT E (-): Un-Indicate recording mode.
		current=sIdle;
//		acdStartPlaying();
		while(kpLastChar=='1') {
			osPlayMode(&current);
		}
//		U1STACLR = _U1STA_URXEN_MASK |//Disable Receiver
//			_U1STA_OERR_MASK;//Clear Overflow
//		U1STASET = _U1STA_UTXEN_MASK;//Enable Transmitter
		LATESET = 0x04;//PORT E (-): Un-indicate sync.
		LATECLR = 0x08;//PORT E (-): Indicate recording mode.
		LATESET = 0x01;//PORT E (-): Un-Indicate playing mode.
		current=sIdle;
		while(kpLastChar=='2') {
			osRecordMode(&current);
		}
		acdCommandWrite(
				ACD_MODE_ADDRESS,
				ACD_MODE_DEFAULT_MASK |
				ACD_MODE_RESET_MASK
		);
		acdWarmUpAD();
	}
}

void osPlayMode(state* current) {
	static int syncTry;
	static int iByte;
	switch(*current) {
		case sIdle:
			if (uartRXReady()) {
				protocol piece=(protocol)uartRXRead();
				if (uartProcessQuery(piece) == pReady) {
					uartRespond(pYes);
					*current=sSyncing;
					syncTry=0;
				}
			}
			break;
		case sSyncing:
			if (uartRXReady()) {
				protocol piece=(protocol)uartRXRead();
				if (uartProcessSync(piece)) {
					*current=sReceiving;
					LATECLR = 0x04;//PORT E (-): Indicate sync.
					iByte=0;
				}
			}
			break;
		case sReceiving:
			if (uartRXReady()) {
				acdFileData[iByte]=uartRXRead();
				++iByte;
				if (iByte>=NUM_BLOCKS*256) {
					*current=sPlaying;
					LATESET = 0x04;//PORT E (-): Un-indicate sync.
				}
			}
			break;
		case sPlaying:
			acdSendFileHeader(modifiedHeader);
			acdPlayFile(acdFileData,NUM_BLOCKS);
			*current=sIdle;
			break;
		default:*current=sIdle;
	}
}

void osRecordMode(state* current) {
	static int syncTry;
	static int iByte;
	switch(*current) {
		case sIdle:
			*current=sRecording;
			break;
		case sRecording:
			acdStartRecording();
			acdReadFile(acdFileData);
			uartQuery(pReady);
			*current=sSyncing;
			break;
		case sSyncing:
			if (uartRXReady()) {
				protocol piece=(protocol)uartRXRead();
				if (uartProcessResponse(piece)) {
					*current=sSending;
					uartSync();
					LATECLR = 0x04;//PORT E (-): Indicate sync.
					iByte=0;
				}
			}
			break;
		case sSending:
			if (!uartTXFull()) {
				uartTXWrite(acdFileData[iByte]);
				++iByte;
				if (iByte>=NUM_BLOCKS*256) {
					*current=sIdle;
					LATESET = 0x04;//PORT E (-): Un-indicate sync.
				}
			}
			break;
		default:*current=sIdle;
	}
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
	mpShutdown();
}

