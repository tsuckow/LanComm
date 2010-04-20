#ifndef SPI_H
#define SPI_H

#include <p32xxxx.h>
#include <plib.h>
#include <sys/types.h>
#define CLK_DIV 0x0003
#define PORTE_ACDRESET_MASK	0x000F
typedef enum bool bool;
enum bool {false = 0,true = 1};

typedef uint8_t Ticket;
typedef uint16_t Data;

typedef enum QueueItemStatus QueueItemStatus;
enum QueueItemStatus {
	QueueItemStatus_WaitingOut,
	QueueItemStatus_Transmitting,
	QueueItemStatus_Processing,
	QueueItemStatus_Receiving,
	QueueItemStatus_WaitingIn,
	QueueItemStatus_DoesNotExist
};

typedef struct QueueItem QueueItem;
struct QueueItem {
	Data dataA;
	Data dataB;
	Ticket ticket;
	QueueItem* behind;
} NULL_ITEM;

typedef struct Queue Queue;
struct Queue {
	QueueItem* head;
	QueueItem* tail;
};

Ticket spiEnqueue(Data dataA,Data dataB);
QueueItem spiDequeue(Ticket ticket);
QueueItemStatus spiGetQueueItemStatus(Ticket ticket);


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
