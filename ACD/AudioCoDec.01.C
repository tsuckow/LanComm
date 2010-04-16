#include "AudioCoDec.H"

int acdInitialize() {
	int failCode=acdInitInterface();
	if (failCode) return failCode;
	acdReset();//Reset just for good measure.
	acdTestCommunication();
	return acdInitDevice();
}
int acdInitInterface() {
	return spiInitSPI1();
}
int acdInitDevice() {
	spiClearOverflow();
	spiRead();
	int failCode;
	if (
		(failCode=acdConfigCRB()) ||
		(failCode=acdConfigCRD()) ||
		(failCode=acdConfigCRE()) ||
		(failCode=acdConfigCRF()) ||
		(failCode=acdConfigCRG()) ||
		(failCode=acdConfigCRH()) ||
		(failCode=acdConfigCRC()) ||
		(failCode=acdConfigCRA())
	     ) {
		return failCode;
	}
	return 0;//Return no errors.
}

int acdConfigCRA() {
	spiWriteBlocked(			//Configure mode and device count for device 2.
			ACD_C |			//Send a command
			ACD_device2 |		//Talk to device 2
			ACD_cra |		//Write to control registr a
			ACD_cra_DATA |		//Configure for data mode.
			ACD_cra_MM |		//Configure for mixed mode.
			ACD_cra_DC		//Set the device count.
	);
	spiWriteBlocked(			//Configure mode and device count for device 1.
			ACD_C |			//Send a command
			ACD_device1 |		//Talk to device 1
			ACD_cra |		//Write to control registr a
			ACD_cra_DATA |		//Configure for data mode.
			ACD_cra_MM |		//Configure for mixed mode.
			ACD_cra_DC		//Set the device count.
	);
	spiReadBlocked();
	spiReadBlocked();
	return 0;//Return no errors.
}

int acdConfigCRB() {
	spiWriteBlocked(			//Configure clock dividers and rates for device 2.
			ACD_C |			//Send a command
			ACD_device2 |		//Talk to device 2
			ACD_crb |		//Write to control registr b
			ACD_crb_DIR |		//Set the Decimation/Interpolation Rate.
			ACD_crb_SCD		//Set the serial clock divider.
	);
	spiWriteBlocked(			//Configure clock dividers and rates for device 1.
			ACD_C |			//Send a command
			ACD_device1 |		//Talk to device 1
			ACD_crb |		//Write to control registr b
			ACD_crb_DIR |		//Set the Decimation/Interpolation Rate.
			ACD_crb_SCD		//Set the serial clock divider.
	);
	spiReadBlocked();
	spiReadBlocked();
	return 0;//Return no errors.
}

int acdConfigCRC() {
	spiWriteBlocked(			//Power up device 2.
			ACD_C |			//Send a command
			ACD_device2 |		//Talk to device 2
			ACD_crc |		//Write to control registr c
			ACD_crc_PU |		//Power up the entire device.
			ACD_crc_PUIA |		//Power up the input amplifier.
			ACD_crc_PUDAC |		//Power up the digital to analogue converter.
			ACD_crc_PUADC |		//Power up the analogue to digital converter.
			ACD_crc_PUREF |		//Power up the reference voltage.
			ACD_crc_RU		//Enable the reference volatage.
	);
	spiWriteBlocked(			//Power up device 1.
			ACD_C |			//Send a command
			ACD_device1 |		//Talk to device 1
			ACD_crc |		//Write to control registr c
			ACD_crc_PU |		//Power up the entire device.
			ACD_crc_PUIA |		//Power up the input amplifier.
			ACD_crc_PUDAC |		//Power up the digital to analogue converter.
			ACD_crc_PUADC |		//Power up the analogue to digital converter.
			ACD_crc_PUREF |		//Power up the reference voltage.
			ACD_crc_RU		//Enable the reference volatage.
	);
	spiReadBlocked();
	spiReadBlocked();
	return 0;//Return no errors.
}

int acdConfigCRD() {
	return 0;//Return no errors.
}

int acdConfigCRE() {
	return 0;//Return no errors.
}

int acdConfigCRF() {
	spiWriteBlocked(			//Configure single ended enable for device 2.
			ACD_C |			//Send a command
			ACD_device2 |		//Talk to device 2
			ACD_crf |		//Write to control registr c
			ACD_crf_SEEN_AGTE	//Enable single ended mode.
	);
	spiWriteBlocked(			//Configure single ended enable for device 1.
			ACD_C |			//Send a command
			ACD_device1 |		//Talk to device 1
			ACD_crf |		//Write to control registr c
			ACD_crf_SEEN_AGTE	//Enable single ended mode.
	);
	spiReadBlocked();
	spiReadBlocked();
	return 0;//Return no errors.
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
}


void acdTestCommunication() {
	spiClearOverflow();
	spiRead();
	spiWriteBlocked(0xFF3C);//Write a test pattern.
	while(1) {
		spiWriteBlocked(0xFFC3);//Write a test pattern.
		spiReadBlocked();
		spiWriteBlocked(0xFF3C);//Write a test pattern.
		spiReadBlocked();
	}
	spiReadBlocked();
}

void acdConfigureSLB() {
	uint16_t rData;
	spiWriteBlocked(		//Configure device 2 for serial loopback mode.
			ACD_C |		//Send a command.
			ACD_device2 |	//Talk to device 2
			ACD_cra |	//Write to control register a.
			ACD_cra_DC |	//Send the device count.
			ACD_cra_SLB	//Set serial loopback mode.
	);
	spiReadBlocked();
	spiWriteBlocked(		//Configure device 1 for serial loopback mode.
			ACD_C |		//Send a command.
			ACD_device1 |	//Talk to device 1.
			ACD_cra |	//Write to control register a.
			ACD_cra_DC |	//Send the device count.
			ACD_cra_SLB	//Set serial loopback mode.
	);
	spiReadBlocked();
}
