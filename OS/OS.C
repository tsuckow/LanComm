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
	failCode=0;
	if (!failCode) failCode=acdInitialize();
	if (!failCode) failCode=uartInitialize();
	if (!failCode) failCode=kpInitialize();
	return failCode;//Return error code.
}

/*	run
 *
 *	This function will run until the device is ready to be shut down.
 */
void osRun() {
	acdFile fileA,fileB;
	uint8_t buffB=0;
	static state cState=sIdle;
//	kpTest();
//	acdTest();
//	uartTest();
//	soundCheck();
	idcInitializeConnection();
	acdInitFile(
		&fileA,
		idcArrayBuffA,
		NUM_BLOCKS,
		NUM_CHANNELS,
		SAMPLE_FREQUENCY
	);
	acdInitFile(
		&fileB,
		idcArrayBuffB,
		NUM_BLOCKS,
		NUM_CHANNELS,
		SAMPLE_FREQUENCY
	);
	uint8_t running=1;
	while(running) {
		while(! kpNewChar) {
			if (connection.role==crMaster) {
				if (cState!=sRecording) {
					acdStartRecording();
					cState=sRecording;
				}
				if (buffB) {
					if (currIdcStatus!=isSendingArray) {
						LATECLR = 0x8;//Turn on green LED.
						acdReadFile(&fileB);
						LATESET = 0x8;//Turn off green LED.
						idcSendByteArray(
							fileB.buf,fileB.size
						);
					}
					buffB=0;
				} else {
					if (currIdcStatus!=isSendingArray) {
						LATECLR = 0x8;//Turn on green LED.
						acdReadFile(&fileA);
						LATESET = 0x8;//Turn off green LED.
						idcSendByteArray(
							fileA.buf,fileA.size
						);
					}
					buffB=1;
				}
			} else if (connection.role==crSlave) {
				if (cState!=sPlaying) {
					acdStartPlaying();
					cState=sPlaying;
				}
				if (idcArrayBuffASize) {
					LATECLR = 0x1;//Turn on orange LED.
					acdPlayFile(&fileA);
					LATESET = 0x1;//Turn off orange LED.
					idcArrayBuffASize=0;
				}
				if (idcArrayBuffBSize) {
					LATECLR = 0x1;//Turn on orange LED.
					acdPlayFile(&fileB);
					LATESET = 0x1;//Turn off orange LED.
					idcArrayBuffBSize=0;
				}
			} else {
				if (cState!=sIdle) {
					acdCommandWrite(
							ACD_MODE_ADDRESS,
							ACD_MODE_DEFAULT_MASK |
							ACD_MODE_RESET_MASK
					);
					acdWarmUpAD();
					cState=sIdle;
					//Turn off green and orange LEDs.
					LATESET = 0x9;
				}
			}
		}
		kpNewChar=0;
		switch(kpLastChar) {
			case'\0':
				break;
			case '0':
				break;
			case '1':
				break;
			case '2':
				break;
			case '3':
				break;
			case '4':
				break;
			case '5':
				break;
			case '6':
				break;
			case '7':
				break;
			case '8':
				break;
			case '9':
				break;
			case 'A':
				if (connection.status!=csConnected) {
					while(currIdcStatus);
					idcOpenConnection();
				}
				break;
			case 'B':
				if (connection.status==csConnected) {
					while(currIdcStatus);
					idcCloseConnection();
				}
				break;
			case 'C':
				break;
			case 'D':
				running=0;
				idcCloseConnection();
				break;
			case 'E':
				break;
			case 'F':
				break;
		}
	}
//	mpTest();//No more network.
/*	acdWarmUpAD();
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
//			osPlayMode(&current);
			osPlayMode_();
		}
//		U1STACLR = _U1STA_URXEN_MASK |//Disable Receiver
//			_U1STA_OERR_MASK;//Clear Overflow
//		U1STASET = _U1STA_UTXEN_MASK;//Enable Transmitter
		LATESET = 0x04;//PORT E (-): Un-indicate sync.
		LATECLR = 0x08;//PORT E (-): Indicate recording mode.
		LATESET = 0x01;//PORT E (-): Un-Indicate playing mode.
		current=sIdle;
		while(kpLastChar=='2') {
//			osRecordMode(&current);
			osRecordMode_();
		}
		acdCommandWrite(
				ACD_MODE_ADDRESS,
				ACD_MODE_DEFAULT_MASK |
				ACD_MODE_RESET_MASK
		);
		acdWarmUpAD();
	}/**/
}

void osPlayMode_() {
/*		LATECLR = 0x01;//PORT E (-): Indicate playing mode.
		LATESET = 0x08;//PORT E (-): Un-Indicate recording mode.
		acdStartPlaying();
		acdSendFileHeader(modifiedHeader);
//		acdPlayFile(acdFileData,NUM_BLOCKS);
		uint32_t data32;
		while (1) {
			((uint8_t*)(&data32))[0]=uartRXPollRead();
			((uint8_t*)(&data32))[1]=uartRXPollRead();
			((uint8_t*)(&data32))[2]=uartRXPollRead();
			((uint8_t*)(&data32))[3]=uartRXPollRead();
			acdDataTransfer(data32);
		}
		/**/
}
void osRecordMode_() {
/*		LATECLR = 0x08;//PORT E (-): Indicate recording mode.
		LATESET = 0x01;//PORT E (-): Un-Indicate playing mode.
		acdStartRecording();
//		acdReadFile(acdFileData);
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
		}
		/**/
}
void osPlayMode(state* current) {
/*	static int syncTry;
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
				++syncTry;
//				if (syncTry>5) *current=sIdle;
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
/**/}

void osRecordMode(state* current) {
/*	static int syncTry;
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
			syncTry=0;
			break;
		case sSyncing:
			if (uartRXReady()) {
				protocol piece=(protocol)uartRXRead();
				++syncTry;
//				if (syncTry>5) *current=sIdle;
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
/**/}

/*	soundCheck
 *
 *	This function this function will constantly pipe sound from a master 
 *	device to a slave device.
 */
void soundCheck() {
/*	int i=0;
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
/**/}

/*	shutdown
 *
 *	This function will perform any necessary shutdown operations and then
 *	exit.
 */
void osShutdown() {
	acdShutdown();
	uartShutdown();
	kpShutdown();
	LATESET=0xF;//Turn off all the LEDs.
	LATECLR=0x2;//Turn on the red LED.
}

