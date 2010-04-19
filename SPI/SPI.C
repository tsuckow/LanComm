

#include "SPI.H"

Queue gOutQueue;
Queue gInQueue;
QueueItem* gCurrentItem;
Ticket gNextTicket=1;
uint8_t gDataA_B;


Ticket enQueue(Data dataA,Data dataB) {
	QueueItem* newItem = (QueueItem*)malloc(sizeof(QueueItem));
	if (newItem) {
		newItem->dataA = dataA;
		newItem->dataB = dataB;
		newItem->ticket = gNextTicket++;
		gOutQueue->tail->behind = newItem;
		newItem->behind=0;
		gOutQueue->tail = newItem;
		return newItem->ticket;
	} else {
		return 0;
	}
}

QueueItem deQueue(Ticket ticket) {
	QueueItem found = NULL_ITEM;
	if (ticket) {//If ticket is not null...
		uint8_t notDone=1;
		do {//This loop will block until we know the ticket doesn't exist or it has been recieved.
			QueueItem* trail = gInQueue.head;//Start searching at the head of the in queue.
			if (trail) {//If the in queue isn't empty...
				if (trail->ticket == ticket) {//And if it is at the head of the queue...
					found = *trail;//Set our return value (copy).
					gInQueue.head = trail.behind;//Remove it from the queue.
					if (gInQueue.tail == trail) gInQueue.tail = 0;//If we removed the tail then set it to null.
					free(trail);//Free the original.
					notDone=0;//And we're done.
				} else {//Or if it wasn't at the head of the queue we're gonna have to search through the queue.
					QueueItem* travel = trail->behind;//Travel will be our index for iterating.
					while(travel && travel->ticket != ticket) {//Search through.
						trail = travel;//Increment trail.
						travel = travel->behind;//Increment travel.
					}
					if (travel) {//If we found it.
						found = *trail;//Set our return value (copy).
						trail->behind = travel->behind;//Remove it.
						if (gInQueue.tail == travel) gInQueue.tail = trail;//If we removed the tail then set it to trail.
						free(travel);//Free the original.
						notDone=0;//And we're done.
					}
				}
			}
			if (notDone) {//If it wasn't in the input buffer...
				if (gCurrentItem->ticket != ticket) {//If this is not the item currently being recieved.
					for (trail=gOutQueue.head; trail; trail=trail->behind);//Iterate through the out queue searching.
					if (!trail) notDone=0;//If it isn't in the out queue then it doesn't exist and we need to return the NULL_ITEM.
				}
			}
		} while (notDone);
		return found;//If we found it then this will return it, if not this will return invalid.
	} else {//If ticket was null...
		return NULL_ITEM;//Return an invalid value.
	}
}

void spi1Interrupt() {
	if (SPI1STAT & _SPI1STAT_SPIROV_MASK) {//If we had an overflow.
		SPI1CONCLR= _SPI1CON_ON_MASK; //SPI1 Config(-): Turn it off.
		LATECLR = PORTE_ACDRESET_MASK;//PORT E(-): Activate the reset for the ACD.
	} else {
			if (gDataA_B) {//If we are sending an A word.
			} else {//If we are sending a B word.
			}
		gDataA_B = !gDataA_B;
	}
}


/*	initClockBus
 *
 *	Initializes SPI2 in  framed master mode so it constantly clocks.  We will use this 
 *	clock pin as the clock bus for out peripherals.
 */
int spiInitClockBus() {
/*
	The following code example will initialize the SPI2 in Master mode.
	It assumes that none of the SPI2 input pins are shared with an analog input. If so, the
	AD1PCFG and corresponding TRIS registers have to be properly configured.
*/
	uint16_t trash;//A trash value so we can read from the register.
	IEC1CLR =			//Interrupt Enable(-)
		_IEC1_SPI2EIE_MASK |	//Error interrupt
		_IEC1_SPI2TXIE_MASK |	//Transmit interrupt
		_IEC1_SPI2RXIE_MASK;	//Recieve interrupt
	SPI2CON = 0;//SPI2 Config(=): Stops and resets the SPI1.
	trash=SPI2BUF;// clears the receive buffer
	IFS1CLR =			//Interrupt Flags(-)
		_IFS1_SPI2EIF_MASK |	//Error interrupt
		_IFS1_SPI2TXIF_MASK |	//Transmit interrupt
		_IFS1_SPI2RXIF_MASK;	//Recieve interrupt
	SPI2BRG=CLK_DIV;//SPI2 Baud Rate Generator(=): Set the divider to determine the clock frequency.
	SPI2STATCLR=_SPI2STAT_SPIROV_MASK;//SPI2 Status(-): Clear recieve overflow.
	SPI2CONSET=			//SPI2 Config(+)
		_SPI2CON_FRMEN_MASK |	//Frame enable
		_SPI2CON_FRMPOL_MASK |	//Frame polarity
		_SPI2CON_MODE16_MASK |	//16 Bit mode
		_SPI2CON_SSEN_MASK |	//Slave Select enable
		_SPI2CON_MSTEN_MASK;	//Master mode.
	SPI2CONSET= _SPI2CON_ON_MASK; //SPI2 Config(+): Turn it on.
	// from now on, the device is ready to transmit and receive data
	return 0;//Return no error.
}

/*	initSPI1
 *
 *	Initializes SPI1 to be used for communication with the ACD.
 */
int spiInitSPI1() {
/*
	The following code example will initialize the SPI1 in Slave mode.
	It assumes that none of the SPI1 input pins are shared with an analog input. If so, the
	AD1PCFG and corresponding TRIS registers have to be properly configured.
*/
	uint16_t trash;
//	TRISDSET=0x0200;//Tris D(+):Set Frame Sync to an input.
	IEC0CLR = 			//Interrupt Enable(-)
		_IEC0_SPI1EIE_MASK |	//Error interrupt
		_IEC0_SPI1TXIE_MASK |	//Transmit interrupt
		_IEC0_SPI1RXIE_MASK;	//Recieve interrupt
	SPI1CON=0;//SPI1 Config(=): Stops and resets the SPI1.
	trash=SPI1BUF;//Clears the receive buffer
	IFS0CLR =			//Interrupt Flags(-)
		_IFS0_SPI1EIF_MASK |	//Error interrupt
		_IFS0_SPI1TXIF_MASK |	//Transmit interrupt
		_IFS0_SPI1RXIF_MASK;	//Recieve interrupt
	SPI1STATCLR=_SPI1STAT_SPIROV_MASK;//SPI1 Status(-): Clear recieve overflow.
	SPI1BRG=CLK_DIV >> 3;//SPI1 Baud Rate Generator(=): Set the divider to 1/8th the clock requency.
	SPI1CONSET=			//SPI1 Config(+)
		_SPI1CON_FRMEN_MASK |	//Frame enable
		_SPI1CON_FRMSYNC_MASK |	//Frame sync
		_SPI1CON_FRMPOL_MASK |	//Frame polarity
		_SPI1CON_MODE16_MASK;	//16 Bit mode
	SPI1CONSET= _SPI1CON_ON_MASK; //SPI1 Config(+): Turn it on.
	// from now on, the device is ready to receive and transmit data
	
	gOutQueue.head=0;
	gOutQueue.tail=0;
	gInQueue.head=0;
	gInQueue.tail=0;

	NULL_ITEM.dataA=0;
	NULL_ITEM.dataB=0;
	NULL_ITEM.ticket=0;
	NULL_ITEM.behind=0;

	gCurrentTicket;

	return 0;//Return no errors.
}

//	spiClearOverflow() -- Clears the recieve overflow of SPI1.
inline void spiClearOverflow() {SPI1STATCLR=_SPI1STAT_SPIROV_MASK;/*SPI1 Status(-): Clear recieve overflow.*/}
//	spiBusySPI1 -- Spins polling the busy bit of the SPI1 status register.
inline void spiBusySPI1() {while(SPI1STAT & _SPI1STAT_SPIBUSY_MASK);}

//	spiUntilTBempty -- Spins polling until the transmit buffer of SPI1 is empty.
inline void spiUntilTBempty() {while(!(SPI1STAT & _SPI1STAT_SPITBE_MASK));/*Wait for the transmit buffer to be empty.*/}
//	spiUntilRBfull -- Spins polling until the recieve buffer of SPI1 is full.
inline void spiUntilRBfull() {while(!(SPI1STAT & _SPI1STAT_SPIRBF_MASK));/*Wait for the recieve buffer to be full.*/}

//	spiWrite -- Writes 2 bytes to SPI1.
inline void spiWrite(uint16_t data) {SPI1BUF=data;}
/*	spiWriteBlocked
 *
 *	Writes 2 bytes to SPI1.  Before the write this function will block with a call to 
 *	spiUntilTBempty.
 */
inline void spiWriteBlocked(uint16_t data) {
	spiUntilTBempty();
	spiWrite(data);
}

//	spiRead -- Reads 2 bytes from from SPI1.
inline uint16_t spiRead() {return (uint16_t)SPI1BUF;}
/*	spiReadBlocked
 *
 *	Reads 2 bytes from SPI1.  Before the read this function will block with a call to 
 *	spiUntilRBfull.
 */
inline uint16_t spiReadBlocked() {
	spiUntilRBfull();
	return spiRead();
}
