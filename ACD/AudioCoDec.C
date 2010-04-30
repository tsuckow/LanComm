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
	QueueItem CRA;
	CRA.dataA =	ACD_C |			//Send a command
			ACD_device2 |		//Talk to device 2
			ACD_cra |		//Write to control register a
			ACD_cra_DATA |		//Configure for data mode.
			ACD_cra_MM |		//Configure for mixed mode.
			ACD_cra_DC;		//Set the device count.
	CRA.dataB =	ACD_C |			//Send a command
			ACD_device1 |		//Talk to device 1
			ACD_cra |		//Write to control register a
			ACD_cra_DATA |		//Configure for data mode.
			ACD_cra_MM |		//Configure for mixed mode.
			ACD_cra_DC;		//Set the device count.
	QueueItem CRB;
	CRB.dataA =	ACD_C |			//Send a command
			ACD_device2 |		//Talk to device 2
			ACD_crb |		//Write to control registr b
			ACD_crb_DIR |		//Set the Decimation/Interpolation Rate.
			ACD_crb_SCD;		//Set the serial clock divider.
	CRB.dataB =	ACD_C |			//Send a command
			ACD_device1 |		//Talk to device 1
			ACD_crb |		//Write to control registr b
			ACD_crb_DIR |		//Set the Decimation/Interpolation Rate.
			ACD_crb_SCD;		//Set the serial clock divider.
	QueueItem CRC;
	CRC.dataA =	ACD_C |			//Send a command
			ACD_device2 |		//Talk to device 2
			ACD_crc |		//Write to control registr c
//			ACD_crc_PU |		//Power up the entire device.
			ACD_crc_PUIA |		//Power up the input amplifier.
			ACD_crc_PUDAC |		//Power up the digital to analogue converter.
			ACD_crc_PUADC |		//Power up the analogue to digital converter.
			ACD_crc_PUREF |		//Power up the reference voltage.
			ACD_crc_RU;		//Enable the reference volatage.
	CRC.dataB =	ACD_C |			//Send a command
			ACD_device1 |		//Talk to device 1
			ACD_crc |		//Write to control registr c
//			ACD_crc_PU |		//Power up the entire device.
			ACD_crc_PUIA |		//Power up the input amplifier.
			ACD_crc_PUDAC |		//Power up the digital to analogue converter.
			ACD_crc_PUADC |		//Power up the analogue to digital converter.
			ACD_crc_PUREF |		//Power up the reference voltage.
			ACD_crc_RU;		//Enable the reference volatage.
	QueueItem CRF;
	CRF.dataA =	ACD_C |			//Send a command
			ACD_device2 |		//Talk to device 2
			ACD_crf |		//Write to control registr c
//			ACD_crf_SEEN_AGTE |	//Enable single ended mode.
			ACD_crf_ALB_AGTM;	//Configure analogue loopback mode.
	CRF.dataB =	ACD_C |			//Send a command
			ACD_device1 |		//Talk to device 1
			ACD_crf |		//Write to control registr c
//			ACD_crf_SEEN_AGTE	//Enable single ended mode.
			ACD_crf_ALB_AGTM;	//Configure analogue loopback mode.
	QueueItem temp;
	QueueItem soundTest[12];
	int failCode;

//	test=spiDequeue(spiEnqueue(0x8918,0x8118));
//	cr2 = test.dataA;
//	cr1 = test.dataB;
	CRA.ticket = spiEnqueue(CRA.dataA,CRA.dataB);//Configure mode and device count.
	CRB.ticket = spiEnqueue(CRB.dataA,CRB.dataB);//Configure clock dividers and rates for.
	CRC.ticket = spiEnqueue(CRC.dataA,CRC.dataB);//Power up devices.
	CRF.ticket = spiEnqueue(CRF.dataA,CRC.dataB);//Enable single ended mode.
	temp = spiDequeue(CRA.ticket);//Get the response from reading register A.
	temp = spiDequeue(CRB.ticket);//Get the response from reading register B.
	temp = spiDequeue(CRC.ticket);//Get the response from reading register C.
	temp = spiDequeue(CRF.ticket);//Get the response from reading register F.

	CRA.ticket = spiEnqueue(		//Read mode and device count.
			ACD_C |			//Send a command
			ACD_R |			//Read the register
			ACD_cra |		//Read register A.
			ACD_device2,		//Talk to device 2
			ACD_C |			//Send a command
			ACD_R |			//Read the register
			ACD_cra |		//Read register A.
			ACD_device1		//Talk to device 1
	);
	CRB.ticket = spiEnqueue(		//Read clocks and rates.
			ACD_C |			//Send a command
			ACD_R |			//Read the register
			ACD_crb |		//Read register A.
			ACD_device2,		//Talk to device 2
			ACD_C |			//Send a command
			ACD_R |			//Read the register
			ACD_crb |		//Read register A.
			ACD_device1		//Talk to device 1
	);
	CRC.ticket = spiEnqueue(		//Read power settings.
			ACD_C |			//Send a command
			ACD_R |			//Read the register
			ACD_crc |		//Read register A.
			ACD_device2,		//Talk to device 2
			ACD_C |			//Send a command
			ACD_R |			//Read the register
			ACD_crc |		//Read register A.
			ACD_device1		//Talk to device 1
	);
	CRF.ticket = spiEnqueue(		//Read sindle ended/anlog gain tap configurations.
			ACD_C |			//Send a command
			ACD_R |			//Read the register
			ACD_crf |		//Read register A.
			ACD_device2,		//Talk to device 2
			ACD_C |			//Send a command
			ACD_R |			//Read the register
			ACD_crf |		//Read register A.
			ACD_device1		//Talk to device 1
	);
	temp = spiDequeue(CRF.ticket);//Get the response from configuring register F.
	if ((0x00FF)&(CRF.dataA) != (0x00FF)&(temp.dataA) || (0x00FF)&(CRF.dataB) != (0x00FF)&(temp.dataB)) return 1;
	temp = spiDequeue(CRC.ticket);//Get the response from configuring register C.
	if ((0x00FF)&(CRC.dataA) != (0x00FF)&(temp.dataA) || (0x00FF)&(CRC.dataB) != (0x00FF)&(temp.dataB)) return 2;
	temp = spiDequeue(CRB.ticket);//Get the response from configuring register B.
	if ((0x00FF)&(CRB.dataA) != (0x00FF)&(temp.dataA) || (0x00FF)&(CRB.dataB) != (0x00FF)&(temp.dataB)) return 3;
	temp = spiDequeue(CRA.ticket);//Get the response from configuring register A.
	if ((0x00FF)&(CRA.dataA) != (0x00FF)&(temp.dataA) || (0x00FF)&(CRA.dataB) != (0x00FF)&(temp.dataB)) return 4;
//	switchToDMA();
//	while(1);
	return 0;//Return no errors.
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
//	TMR23 for waiting
	IEC0CLR = _IEC0_T2IE_MASK;//Interrupt Enable (-): Disable interrupts for timer 2.
	IEC0CLR = _IEC0_T3IE_MASK;//Interrupt Enable (-): Disable interrupts for timer 2.
	IFS0CLR = _IFS0_T2IF_MASK;//Interrupt Flags (-): Clear flags for timer 2.
	IFS0CLR = _IFS0_T3IF_MASK;//Interrupt Flags (-): Clear flags for timer 2.
	IPC2CLR = _IPC2_T2IP_MASK | _IPC2_T2IS_MASK;//Interrupt Priority(-): Clear the interrupt priority for timer 2.
	IPC3CLR = _IPC3_T3IP_MASK | _IPC3_T3IS_MASK;//Interrupt Priority(-): Clear the interrupt priority for timer 2.
//	T2CON = _T2CON_TCKPS0_MASK | _T2CON_TCKPS1_MASK | _T2CON_TCKPS2_MASK;//Timer 2 Config(=): Turn off and set prescaler of 256.
	T2CON = _T2CON_T32_MASK;//Timer 2 Config(=): Turn off and set for 32 bit mode.
	TMR2 = (uint32_t)0;//Clear timer.

	LATECLR = PORTE_ACDRESET_MASK;//PORT E(-): Activate the reset for the ACD.

	T2CONCLR = _T2CON_ON_MASK;//Timer 2 Config(-): Turn off the timer.
	IFS0CLR = _IFS0_T3IF_MASK;//Interrupt Flags (-): Clear flags for timer 2
	TMR2 = (uint32_t)0;//Clear timer.
	PR2 = 80 * 2*(CLK_DIV+1) * 5;//Period Register 2(=): Set the period to the number of cycles we want.
	T2CONSET = _T2CON_ON_MASK;//Timer 2 Config(+): Turn on the timer.
	while(!(IFS0 & _IFS0_T3IF_MASK));
	T2CONCLR = _T2CON_ON_MASK;//Timer 2 Config(-): Turn off the timer.
	
	LATESET = PORTE_ACDRESET_MASK;//PORT E(+): Deactivate the reset for the ACD.

	T2CONCLR = _T2CON_ON_MASK;//Timer 2 Config(-): Turn off the timer.
	IFS0CLR = _IFS0_T3IF_MASK;//Interrupt Flags (-): Clear flags for timer 2
	TMR2 = (uint32_t)0;//Clear timer.
	PR2 = 80 * 2*(CLK_DIV+1) * 5;//Period Register 2(=): Set the period to the number of cycles we want.
	T2CONSET = _T2CON_ON_MASK;//Timer 2 Config(+): Turn on the timer.
	while(!(IFS0 & _IFS0_T3IF_MASK));
	T2CONCLR = _T2CON_ON_MASK;//Timer 2 Config(-): Turn off the timer.

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
