#include "AudioCoDec.H"

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
//	acdBasicTests();
	acdFileTests();
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
//	acdSineTest(0xFE);//Extremely Loud.  Be warned.
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
	LATDCLR = PORTD_ACDXCS_MASK;//PORT D (+): Activate command ship select.
	spiWriteBlocked(
			ACD_WRITE << 24 |
			address << 16 |
			value
	);
	spiReadBlocked();
	LATDSET = PORTD_ACDXCS_MASK;//PORT D (-): Deactivate command ship select.
	for (count=0;count<0xF;++count);
}
uint16_t acdCommandRead(uint8_t address) {
	int count;
	acdPollDREQ();
	LATDCLR = PORTD_ACDXCS_MASK;//PORT D (+): Activate command ship select.
	spiWriteBlocked(
			ACD_READ << 24 |
			address << 16 |
			0
	);
	uint16_t value = (uint16_t)spiReadBlocked();
	LATDSET = PORTD_ACDXCS_MASK;//PORT D (-): Deactivate command ship select.
	for (count=0;count<0xF;++count);
	return value;
}
uint32_t acdDataTransfer(uint32_t data) {
	int count;
	acdPollDREQ();
	LATDCLR = PORTD_ACDXDCS_MASK;//PORT D (+): Activate data ship select.
	spiWriteBlocked(data);
	data = spiReadBlocked();
	LATDSET = PORTD_ACDXDCS_MASK;//PORT D (-): Deactivate data ship select.
	for (count=0;count<0xF;++count);
	return data;
}
//Extremely Loud. Be warned.
void acdSineTest(uint8_t n) {
	int count;
	uint32_t trash;
	LATDCLR = PORTD_ACDXDCS_MASK;//PORT D (+): Activate data ship select.
	spiWriteBlocked(0x53EF6E00 | n);
	spiWriteBlocked(0x00000000);
	trash = spiReadBlocked();
	trash = spiReadBlocked();
	LATDSET = PORTD_ACDXDCS_MASK;//PORT D (-): Deactivate data ship select.
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
	LATDCLR = PORTD_ACDXDCS_MASK;//PORT D (+): Activate data ship select.
	spiWriteBlocked(0x5370EE00 | regAddress+48);
	spiWriteBlocked(0x00000000);
	spiReadBlocked();
	spiReadBlocked();
	LATDSET = PORTD_ACDXDCS_MASK;//PORT D (-): Deactivate data ship select.
	for (count=0;count<0xF;++count);
}
void acdFileTests() {
}

