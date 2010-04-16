#ifndef SPI_H
#define SPI_H

#include <p32xxxx.h>
#include <plib.h>
#include <sys/types.h>
#define CLK_DIV 0x0001

int spiQueueDataPair(uint16_t outA, uint16_t outB);

/*	initClockBus
 *
 *	Initializes SPI2 in  framed master mode so it constantly clocks.  We will use this 
 *	clock pin as the clock bus for out peripherals.
 */
int spiInitClockBus();

/*	initSPI1
 *
 *	Initializes SPI1 to be used for communication with the ACD.
 */
int spiInitSPI1();

//	spiClearOverflow() -- Clears the recieve overflow of SPI1.
inline void spiClearOverflow();
//	spiBusySPI1 -- Spins polling the busy bit of the SPI1 status register.
void spiBusySPI1();
//	untilTBempty -- Spins polling until the transmit buffer of SPI1 is empty.
inline void spiUntilTBempty();
//	untilRBfull -- Spins polling until the recieve buffer of SPI1 is full.
inline void spiUntilRBfull();

//	spiWrite -- Writes 2 bytes to SPI1.
inline void spiWrite(uint16_t data);
/*	spiWriteBlocked
 *
 *	Writes 2 bytes to SPI1.  Before the write this function will block with a call to 
 *	untilTBempty.
 */
inline void spiWriteSPI1Blocked(uint16_t data);

//	spiRead -- Reads 2 bytes from from SPI1.
inline uint16_t spiRead();
/*	spiReadBlocked
 *
 *	Reads 2 bytes from SPI1.  Before the read this function will block with a call to 
 *	untilRBfull.
 */
inline uint16_t spiReadSPI1BLocked();

#endif
