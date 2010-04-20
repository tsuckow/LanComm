#include "AudioCoDec.H"

int acdInitialize() {
	int failCode;
	acdReset();//Reset just for good measure.
	failCode=acdInitInterface();
	if (failCode) return failCode;
//	acdTestCommunication();
	return acdInitDevice();
}
int acdInitInterface() {
	return spiInitSPI1();
}
int acdInitDevice() {
	uint16_t cr1=0;
	uint16_t cr2=0;
	QueueItem test;
//	spiClearOverflow();
//	spiReadBlocked();
//	acdConfigureSLB();
//	acdConfigureSLB();
	int failCode;

	test=spiDequeue(spiEnqueue(0x8918,0x8118));
	cr2 = test.dataA;
	cr1 = test.dataB;

	acdConfigCRA();
	spiReadBlocked();
	spiReadBlocked();
	acdConfigCRB();
	spiReadBlocked();
	spiReadBlocked();
	acdConfigCRC();
	spiReadBlocked();
	spiReadBlocked();
	acdConfigCRF();
	spiReadBlocked();
	spiReadBlocked();
	while(1);
/*	if (
		(failCode=acdConfigCRB()) ||
//		(failCode=acdConfigCRD()) ||
//		(failCode=acdConfigCRE()) ||
		(failCode=acdConfigCRF()) ||
//		(failCode=acdConfigCRG()) ||
//		(failCode=acdConfigCRH()) ||
		(failCode=acdConfigCRC()) ||
		(failCode=acdConfigCRA())
	     ) {
		return failCode;
	}*/
	spiClearOverflow();
	spiReadBlocked();
	spiWrite(			//Read from register a on device 2
			ACD_C |
			ACD_R |
			ACD_device2 |
			ACD_crf
	);
	spiReadBlocked();
	spiWrite(			//Read from register a on device 1
			ACD_C |
			ACD_R |
			ACD_device1 |
			ACD_crf
	);
	spiReadBlocked();
	spiWrite(			//Read from register b on device 2
			ACD_C |
			ACD_R |
			ACD_device2 |
			ACD_crb
	);
	cr2 = spiReadBlocked();
	spiWrite(			//Read from register b on device 1
			ACD_C |
			ACD_R |
			ACD_device1 |
			ACD_crb
	);
	cr1 = spiReadBlocked();
	while(1);
/**/	return 0;//Return no errors.
}

int acdConfigCRA() {
	spiReadBlocked();
	spiWrite(				//Configure mode and device count for device 2.
			ACD_C |			//Send a command
			ACD_device2 |		//Talk to device 2
			ACD_cra |		//Write to control registr a
//			ACD_cra_DATA |		//Configure for data mode.
//			ACD_cra_MM |		//Configure for mixed mode.
			ACD_cra_DLB |		//Configure for digital loopback mode.
			ACD_cra_DC		//Set the device count.
	);
	spiReadBlocked();
	spiWrite(				//Configure mode and device count for device 1.
			ACD_C |			//Send a command
			ACD_device1 |		//Talk to device 1
			ACD_cra |		//Write to control registr a
//			ACD_cra_DATA |		//Configure for data mode.
//			ACD_cra_MM |		//Configure for mixed mode.
			ACD_cra_DLB |		//Configure for digital loopback mode.
			ACD_cra_DC		//Set the device count.
	);
	return 0;//Return no errors.
}

int acdConfigCRB() {
	spiReadBlocked();
	spiWrite(				//Configure clock dividers and rates for device 2.
			ACD_C |			//Send a command
			ACD_device2 |		//Talk to device 2
			ACD_crb |		//Write to control registr b
			ACD_crb_DIR |		//Set the Decimation/Interpolation Rate.
			ACD_crb_SCD		//Set the serial clock divider.
	);
	spiReadBlocked();
	spiWrite(				//Configure clock dividers and rates for device 1.
			ACD_C |			//Send a command
			ACD_device1 |		//Talk to device 1
			ACD_crb |		//Write to control registr b
			ACD_crb_DIR |		//Set the Decimation/Interpolation Rate.
			ACD_crb_SCD		//Set the serial clock divider.
	);
	return 0;//Return no errors.
}

int acdConfigCRC() {
	spiReadBlocked();
	spiWrite(				//Power up device 2.
			ACD_C |			//Send a command
			ACD_device2 |		//Talk to device 2
			ACD_crc |		//Write to control registr c
//			ACD_crc_PU |		//Power up the entire device.
			ACD_crc_PUIA |		//Power up the input amplifier.
			ACD_crc_PUDAC |		//Power up the digital to analogue converter.
			ACD_crc_PUADC |		//Power up the analogue to digital converter.
			ACD_crc_PUREF |		//Power up the reference voltage.
			ACD_crc_RU		//Enable the reference volatage.
	);
	spiReadBlocked();
	spiWrite(				//Power up device 1.
			ACD_C |			//Send a command
			ACD_device1 |		//Talk to device 1
			ACD_crc |		//Write to control registr c
//			ACD_crc_PU |		//Power up the entire device.
			ACD_crc_PUIA |		//Power up the input amplifier.
			ACD_crc_PUDAC |		//Power up the digital to analogue converter.
			ACD_crc_PUADC |		//Power up the analogue to digital converter.
			ACD_crc_PUREF |		//Power up the reference voltage.
			ACD_crc_RU		//Enable the reference volatage.
	);
	return 0;//Return no errors.
}

int acdConfigCRD() {
	return 0;//Return no errors.
}

int acdConfigCRE() {
	return 0;//Return no errors.
}

int acdConfigCRF() {
	spiReadBlocked();
	spiWrite(				//Configure single ended enable for device 2.
			ACD_C |			//Send a command
			ACD_device2 |		//Talk to device 2
			ACD_crf |		//Write to control registr c
//			ACD_crf_ALB_AGTM	//Configure for analog loopback mode
			ACD_crf_SEEN_AGTE	//Enable single ended mode.
	);
	spiReadBlocked();
	spiWrite(				//Configure single ended enable for device 1.
			ACD_C |			//Send a command
			ACD_device1 |		//Talk to device 1
			ACD_crf |		//Write to control registr c
//			ACD_crf_ALB_AGTM	//Configure for analog loopback mode
			ACD_crf_SEEN_AGTE	//Enable single ended mode.
	);
/**/	return 0;//Return no errors.
}

int acdConfigCRG() {
	return 0;//Return no errors.
}

int acdConfigCRH() {
	return 0;//Return no errors.
}

void acdReset() {
	int count;
	LATECLR = PORTE_ACDRESET_MASK;//PORT E(-): Activate the reset for the ACD.
	for (count = 0; count < 0xFFFF; count++);
	LATESET = PORTE_ACDRESET_MASK;//PORT E(+): Deactivate the reset for the ACD.
	for (count = 0; count < 0xFFFF; count++);
//	spiClearOverflow();
//	spiReadBlocked();
}


void acdTestCommunication() {
	spiClearOverflow();
	spiRead();
//	spiWriteBlocked(0xFF3C);//Write a test pattern.
	while(1) {
		spiWriteBlocked(0xFFC3);//Write a test pattern.
		spiReadBlocked();
		spiWriteBlocked(0xFF3C);//Write a test pattern.
		spiReadBlocked();
	}
//	spiReadBlocked();
}

void acdConfigureSLB() {
	spiReadBlocked();
	spiWrite(			//Configure device 2 for serial loopback mode.
			ACD_C |		//Send a command.
			ACD_device2 |	//Talk to device 2
			ACD_cra |	//Write to control register a.
//			ACD_cra_DC |	//Send the device count.
			ACD_cra_SLB	//Set serial loopback mode.
	);
	spiReadBlocked();
	spiWrite(			//Configure device 1 for serial loopback mode.
			ACD_C |		//Send a command.
			ACD_device1 |	//Talk to device 1.
			ACD_cra |	//Write to control register a.
//			ACD_cra_DC |	//Send the device count.
			ACD_cra_SLB	//Set serial loopback mode.
	);
}
