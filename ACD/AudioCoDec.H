#ifndef ACD_H
#define ACD_H
/*	A note about the ACD.  It interperets values as raw voltage scales in a 2's 
 *	compliment system with 0x7FFF being full positive and 0x8000 being full scale 
 *	negative.
 */

#include <p32xxxx.h>
#include "../SPI/SPI.H"

int acdInitialize();
int acdInitInterface();
int acdInitDevice();

int acdConfigCRA();
int acdConfigCRB();
int acdConfigCRC();
int acdConfigCRD();
int acdConfigCRE();
int acdConfigCRF();
int acdConfigCRG();
int acdConfigCRH();

void acdReset();

//This is a blocking write.
void acdWriteData(uint16_t data);
//this is a blocking read.
uint16_t acdReadData();

void acdTestCommunication();
void acdConfigureSLB();


#define ACD_C			0x8000
#define ACD_R			0x4000
#define ACD_device1		0x0000
#define ACD_device2		0x0800
#define ACD_cra			0x0000
#define ACD_cra_DATA		  0x01
#define ACD_cra_MM		  0x02
#define ACD_cra_DLB		  0x04
#define ACD_cra_SLB		  0x08
#define ACD_cra_DC		  0x10
#define ACD_cra_RESET		  0x80
#define ACD_crb			0x0100
#define ACD_crb_DIR		  0x03
#define ACD_crb_SCD		  0x00
#define ACD_crb_MCD		  0x10
#define ACD_crb_CEE		  0x80
#define ACD_crc			0x0200
#define ACD_crc_PU		  0x01
#define ACD_crc_PUAGT		  0x02
#define ACD_crc_PUIA		  0x04
#define ACD_crc_PUADC		  0x08
#define ACD_crc_PUDAC		  0x10
#define ACD_crc_PUREF		  0x20
#define ACD_crc_RU		  0x40
#define ACD_crd			0x0300
#define ACD_crd_IGS_00		  0x00
#define ACD_crd_IGS_06		  0x01
#define ACD_crd_IGS_12		  0x02
#define ACD_crd_IGS_18		  0x03
#define ACD_crd_IGS_20		  0x04
#define ACD_crd_IGS_26		  0x05
#define ACD_crd_IGS_32		  0x06
#define ACD_crd_IGS_38		  0x07
#define ACD_crd_RMOD		  0x08
#define ACD_crd_OGS_P06		  0x00
#define ACD_crd_OGS_P03		  0x01
#define ACD_crd_OGS_P00		  0x02
#define ACD_crd_OGS_N03		  0x03
#define ACD_crd_OGS_N06		  0x04
#define ACD_crd_OGS_N09		  0x05
#define ACD_crd_OGS_N12		  0x06
#define ACD_crd_OGS_N15		  0x07
#define ACD_crd_MUTE		  0x80
#define ACD_cre			0x0400
#define ACD_cre_DA		  0x00
#define ACD_cre_IBYP		  0x20
#define ACD_cre_DGTE		  0x40
#define ACD_crf			0x0500
#define ACD_crf_AGTC		  0x00
#define ACD_crf_SEEN_AGTE	  0x20
#define ACD_crf_INV		  0x40
#define ACD_crf_ALB_AGTM	  0x80
#define ACD_crg			0x0600
#define ACD_crg_DGTC_L		  0x00
#define ACD_crh			0x0700
#define ACD_crh_DGTC_H		  0x00

#endif
