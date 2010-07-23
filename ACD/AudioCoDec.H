#ifndef ACD_H
#define ACD_H
/*	A note about the ACD.  It interperets values as raw voltage scales in a 2's 
 *	compliment system with 0x7FFF being full positive and 0x8000 being full scale 
 *	negative.
 */

//#define VS1053B
#define VS1033D

#include <p32xxxx.h>
#include "../SPI/SPI.H"

int acdInitialize();
int acdInitInterface();
int acdInitDevice();
void acdTest();

void acdReset();
inline int acdGetDREQ();
inline void acdPollDREQ();

void acdCommandWrite(uint8_t address, uint16_t value);
uint16_t acdCommandRead(uint8_t address);
uint32_t acdDataTransfer(uint32_t data);
void acdBasicTests();
void acdSineTest(uint8_t n);
void acdSineTest2(uint16_t left, uint16_t right);
void acdSCITest(uint8_t n);

void acdApplyPCMPatch();
void acdStartRecording();
void buildFileHeader(uint8_t* header, int n, uint16_t C,uint32_t Fs);
void readFile(uint8_t* fileStart);
void acdStartPlaying();
void sendFileHeader(uint8_t* header);
void playFile(uint8_t* data,unsigned int blocks);
void acdFileTests();

#define PORTE_ACDRESET_MASK		0x0002
#define PORTE_ACDDREQ_MASK		0x0200
#define PORTD_ACDXCS_MASK		0x0200
#define PORTD_ACDXDCS_MASK		0x0100
#define NUM_BLOCKS			  0x01
#define NUM_CHANNELS			  0x01
#define SAMPLE_FREQUENCY		  8000U

#define ACD_READ			  0x03
#define ACD_WRITE			  0x02
#define ACD_MODE_ADDRESS		  0x00
#define ACD_MODE_DEFAULT_MASK		0x4800
#define ACD_MODE_DIFF_MASK		0x0001
#define ACD_MODE_LAYER12_MASK		0x0002
#define ACD_MODE_RESET_MASK		0x0004
#define ACD_MODE_CANCEL_MASK		0x0008
#define ACD_MODE_EARSPEAKER_LO_MASK	0x0010
#define ACD_MODE_TESTS_MASK		0x0020
#define ACD_MODE_STREAM_MASK		0x0040
#define ACD_MODE_EARSPEAKER_HI_MASK	0x0080
#define ACD_MODE_DACT_MASK		0x0100
#define ACD_MODE_SDIORD_MASK		0x0200
#define ACD_MODE_SDISHARE_MASK		0x0400
#define ACD_MODE_SDINEW_MASK		0x0800
#define ACD_MODE_ADPCM_MASK		0x1000
#define ACD_MODE_WRONG_MASK		0x2000
#define ACD_MODE_LINE1_MASK		0x4000
#define ACD_MODE_CLK_RANGE_MASK		0x8000
#define ACD_STATUS_ADDRESS		  0x01
#define ACD_BASS_ADDRESS		  0x02
#define ACD_CLOCKF_ADDRESS		  0x03
#define ACD_DECODE_TIME_ADDRESS		  0x04
#define ACD_AUDATA_ADDRESS		  0x05
#define ACD_WRAM_ADDRESS		  0x06
#define ACD_WRAMADDR_ADDRESS		  0x07
#define ACD_HDAT0_ADDRESS		  0x08
#define ACD_HDAT1_ADDRESS		  0x09
#define ACD_AIADDR_ADDRESS		  0x0A
#define ACD_VOL_ADDRESS			  0x0B
#define ACD_AICTRL0_ADDRESS		  0x0C
#define ACD_AICTRL1_ADDRESS		  0x0D
#define ACD_AICTRL2_ADDRESS		  0x0E
#define ACD_AICTRL3_ADDRESS		  0x0F

#endif
