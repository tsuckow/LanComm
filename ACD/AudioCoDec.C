#include "AudioCoDec.H"

//This file will conain a header and 32 audio blocks.
uint8_t file[NUM_BLOCKS*256 + 60];
uint8_t* fileHeader = file;
uint8_t* fileData = file+60;

int acdInitialize() {
	int failCode;
//	acdReset();//Reset just for good measure.
	failCode=acdInitInterface();
	if (failCode) return failCode;
	return acdInitDevice();
}
int acdInitInterface() {
	TRISCSET = 0x10;//Configure sdi for input.
	LATDSET = PORTD_ACDXCS_MASK | PORTD_ACDXDCS_MASK;//PORT D (-): Deactivate command and data chip select.
	TRISDCLR = PORTD_ACDXCS_MASK | PORTD_ACDXDCS_MASK;//PORT D (-): Configure both chip selects for outputs.
	ODCDCLR = PORTD_ACDXCS_MASK | PORTD_ACDXDCS_MASK;//PORT D (-): Configure both chip selects for non-open-drain.
	TRISESET = PORTE_ACDDREQ_MASK;//PORT E (+): Configure DREQ as an input.
	return spiInitSPI1();
}
int acdInitDevice() {
	acdReset();//Reset just for good measure.
	acdPollDREQ();
	return 0;//Return no errors.
}
void acdTest() {
	acdBasicTests();
//	acdFileTests();
}
void acdBasicTests() {
	int count;
	uint16_t mode;
	uint16_t status[2];
	acdCommandWrite(
			ACD_MODE_ADDRESS,	//Write to the mode register.
			ACD_MODE_DEFAULT_MASK |	//Write the default values, because we like these ones.
			ACD_MODE_TESTS_MASK	//Enter testing mode.
	);
//	acdSineTest2(0x4000, 0x4000);
	acdSineTest(0x73);//Extremely Loud.  Be warned.
//	mode = acdCommandRead(ACD_MODE_ADDRESS);
//	acdSCITest(ACD_MODE_ADDRESS);
//	for (count=0;count<0x00FFFFFF;++count);
//	mode = acdCommandRead(ACD_MODE_ADDRESS);
//	mode = acdCommandRead(ACD_HDAT0_ADDRESS);
//	status[0] = acdCommandRead(ACD_STATUS_ADDRESS);
//	for (count=0;count<0x00FFFFFF;++count);
//	status[1] = acdCommandRead(ACD_STATUS_ADDRESS);
}

//Note: After activating this reset we shouldn't do anything until DREQ is high.
void acdReset() {
	int count;
	LATECLR = PORTE_ACDRESET_MASK;//PORT E (-): Activate the reset.
	for (count=0;count<16;++count);
	LATESET = PORTE_ACDRESET_MASK;//PORT E (+): Deactivate the reset.
}
int acdGetDREQ() {
	return PORTE&PORTE_ACDDREQ_MASK;//Get value of DREQ;
}
void acdPollDREQ() {
	while(! acdGetDREQ());
}

void acdCommandWrite(uint8_t address, uint16_t value) {
	int count;
	acdPollDREQ();
	LATDCLR = PORTD_ACDXCS_MASK;//PORT D (+): Activate command chip select.
	spiWriteBlocked(
			ACD_WRITE << 24 |
			address << 16 |
			value
	);
	spiReadBlocked();
	LATDSET = PORTD_ACDXCS_MASK;//PORT D (-): Deactivate command chip select.
	for (count=0;count<0xF;++count);
}
uint16_t acdCommandRead(uint8_t address) {
	int count;
	acdPollDREQ();
	LATDCLR = PORTD_ACDXCS_MASK;//PORT D (+): Activate command chip select.
	spiWriteBlocked(
			ACD_READ << 24 |
			address << 16 |
			0
	);
	uint16_t value = (uint16_t)spiReadBlocked();
	LATDSET = PORTD_ACDXCS_MASK;//PORT D (-): Deactivate command chip select.
	for (count=0;count<0xF;++count);
	return value;
}
uint32_t acdDataTransfer(uint32_t data) {
	int count;
	acdPollDREQ();
	LATDCLR = PORTD_ACDXDCS_MASK;//PORT D (+): Activate data chip select.
	spiWriteBlocked(data);
	data = spiReadBlocked();
	LATDSET = PORTD_ACDXDCS_MASK;//PORT D (-): Deactivate data chip select.
	for (count=0;count<0xF;++count);
	return data;
}
//Extremely Loud. Be warned.
void acdSineTest(uint8_t n) {
	int count;
	uint32_t trash;
	LATDCLR = PORTD_ACDXDCS_MASK;//PORT D (+): Activate data chip select.
	spiWriteBlocked(0x53EF6E00 | n);
	spiWriteBlocked(0x00000000);
	trash = spiReadBlocked();
	trash = spiReadBlocked();
	LATDSET = PORTD_ACDXDCS_MASK;//PORT D (-): Deactivate data chip select.
	for (count=0;count<0xF;++count);
}
void acdSineTest2(uint16_t left, uint16_t right) {
	acdCommandWrite(
			ACD_AICTRL0_ADDRESS,
			left
	);
	acdCommandWrite(
			ACD_AICTRL1_ADDRESS,
			right
	);
	acdCommandWrite(
			ACD_AIADDR_ADDRESS,
			0x4020
	);
}

void acdSCITest(uint8_t regAddress) {
	int count;
	LATDCLR = PORTD_ACDXDCS_MASK;//PORT D (+): Activate data chip select.
	spiWriteBlocked(0x5370EE00 | regAddress+48);
	spiWriteBlocked(0x00000000);
	spiReadBlocked();
	spiReadBlocked();
	LATDSET = PORTD_ACDXDCS_MASK;//PORT D (-): Deactivate data chip select.
	for (count=0;count<0xF;++count);
}
void acdApplyPCMPatch() {
	acdCommandWrite(ACD_WRAMADDR_ADDRESS,0x8010);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x3e12);
	acdCommandWrite(ACD_WRAM_ADDRESS,0xb817);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x3e14);
	acdCommandWrite(ACD_WRAM_ADDRESS,0xf812);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x3e01);
	acdCommandWrite(ACD_WRAM_ADDRESS,0xb811);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x0007);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x9717);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x0020);
	acdCommandWrite(ACD_WRAM_ADDRESS,0xffd2);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x0030);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x11d1);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x3111);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x8024);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x3704);
	acdCommandWrite(ACD_WRAM_ADDRESS,0xc024);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x3b81);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x8024);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x3101);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x8024);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x3b81);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x8024);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x3f04);
	acdCommandWrite(ACD_WRAM_ADDRESS,0xc024);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x2808);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x4800);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x36f1);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x9811);
	acdCommandWrite(ACD_WRAMADDR_ADDRESS,0x8028);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x2a00);
	acdCommandWrite(ACD_WRAM_ADDRESS,0x040e);
}
void acdStartRecording() {
	acdCommandWrite(
			ACD_AICTRL0_ADDRESS,
			SAMPLE_FREQUENCY	//Set the sample rate.
	);
	acdCommandWrite(
			ACD_AICTRL1_ADDRESS,
			0			//Use automatic gain control.
	);
	acdCommandWrite(
			ACD_AICTRL2_ADDRESS,
			0x0400			//Set maximum autogain to 1x.
	);
	acdCommandWrite(
			ACD_AICTRL3_ADDRESS,
			2 |			//Use left channel mono.
			0x0			//Use IMA ADPCM mode.
	);
	acdCommandWrite(
			ACD_MODE_ADDRESS,	//Write to the mode register.
			ACD_MODE_DEFAULT_MASK |	//Write the default values, because we like these ones.
			ACD_MODE_ADPCM_MASK |	//Enter ADPCM mode.
			ACD_MODE_RESET_MASK	//Reset to enter recording mode.
	);
	acdApplyPCMPatch();
}
void buildFileHeader(uint8_t* header, int n, uint16_t C,uint32_t Fs) {
	uint16_t* header16=header;
	uint32_t* header32=header;
	header[0]='R'; header[1]='I'; header[2]='F'; header[3]='F';
	header32[1] = (uint32_t)(n * C * 256 + 52);
	header[8]='W'; header[9]='A'; header[10]='V'; header[11]='E';
	header[12]='f'; header[13]='m'; header[14]='t'; header[15]=' ';
	header32[4]=20;
	header16[10]=0x0011;
	header16[11] = C;
	header32[6] = Fs;
	header32[7] = (uint32_t)(Fs * C * 256 / 505);
	header16[16] = (uint16_t)0x0100 * C;
	header16[17] = (uint16_t)0x0004;
	header16[18] = (uint16_t)0x0002;
	header16[19] = (uint16_t)505;
	header[40]='f'; header[41]='a'; header[42]='c'; header[43]='t';
	header32[11] = (uint32_t)4;
	header32[12] = (uint32_t)(n * 505);
	header[52]='d'; header[53]='a'; header[54]='t'; header[55]='a';
	header32[14] = (uint32_t)(n * C * 256);
}
/*void readFile(uint8_t* dataPointer) {
	unsigned int count;
	for (count = 0; count < NUM_BLOCKS*256;count+=2) {
		unsigned int ready;
		do {
			ready=acdCommandRead(ACD_HDAT1_ADDRESS);
		} while (ready == 0);
		uint16_t data = acdCommandRead(ACD_HDAT0_ADDRESS);
		*dataPointer = (uint8_t)(data >> 8);
		++dataPointer;
		*dataPointer = (uint8_t)(data);
		++dataPointer;
	}
}*/
void readFile(uint8_t* dataPointer) {//This version of the function translates for this system's endien-ness
	unsigned int iWord,iByte;
	for (iWord=0;iWord<NUM_BLOCKS*256/4;++iWord) {
		unsigned int ready;
		uint16_t data;
		do {
			ready=acdCommandRead(ACD_HDAT1_ADDRESS);
		} while (ready == 0);
		data = acdCommandRead(ACD_HDAT0_ADDRESS);
		dataPointer[iWord*4+3] = (uint8_t)(data>>8);
		dataPointer[iWord*4+2] = (uint8_t)(data);
		
		do {
			ready=acdCommandRead(ACD_HDAT1_ADDRESS);
		} while (ready == 0);
		data = acdCommandRead(ACD_HDAT0_ADDRESS);
		dataPointer[iWord*4+1] = (uint8_t)(data>>8);
		dataPointer[iWord*4+0] = (uint8_t)(data);
	}
}
void acdStartPlaying() {
	acdCommandWrite(
			ACD_MODE_ADDRESS,	//Write to the mode register.
			ACD_MODE_DEFAULT_MASK |	//Write the default values, because we like these ones.
			ACD_MODE_ADPCM_MASK	//Enter ADPCM mode.
	);
}
void sendFileHeader(uint8_t* header) {
	uint32_t* header32=header;
	unsigned int count;
	for (count=0;count<15;++count) {
		acdDataTransfer(header32[count]);
	}
}
void playFile(uint8_t* data,unsigned int blocks) {
	uint16_t mode;
	uint16_t status;
	uint16_t hdat0;
	uint16_t hdat1;
	unsigned int count;
	uint32_t* data32=data;
	for (count=0;count<blocks*256;++count) {
//		mode = acdCommandRead(ACD_MODE_ADDRESS);
//		status = acdCommandRead(ACD_STATUS_ADDRESS);
//		hdat0 = acdCommandRead(ACD_HDAT0_ADDRESS);
//		hdat1 = acdCommandRead(ACD_HDAT1_ADDRESS);
		acdDataTransfer(data32[count]);
	}
}
void modifyHeader(uint8_t* original,uint8_t* modified) {//This function mofies the header to compensate for this system's endien-ness.
	unsigned int iWord,iByte;
	for (iWord=0;iWord<15;++iWord) {
		for (iByte=0;iByte<4;++iByte) {
			modified[iWord*4 + 3-iByte] = original[iWord*4 + iByte];
		}
	}
}
void acdFileTests() {
	uint16_t mode;
	uint16_t status[2];
	uint8_t modifiedHeader[60];

//	status[0] = acdCommandRead(ACD_STATUS_ADDRESS);
//	mode = acdCommandRead(ACD_MODE_ADDRESS);
//	status[1] = acdCommandRead(ACD_STATUS_ADDRESS);

	buildFileHeader(fileHeader,NUM_BLOCKS,NUM_CHANNELS, SAMPLE_FREQUENCY);
	modifyHeader(fileHeader,modifiedHeader);
//		acdStartPlaying();
	while(1) {
		acdStartRecording();
		readFile(fileData);
		
		acdCommandWrite(
				ACD_MODE_ADDRESS,	//Write to the mode register.
				ACD_MODE_DEFAULT_MASK |	//Write the default values, because we like these ones.
				ACD_MODE_RESET_MASK	//Reset.
		);
		acdStartPlaying();
		sendFileHeader(modifiedHeader);
		playFile(fileData,NUM_BLOCKS);
	}
}

