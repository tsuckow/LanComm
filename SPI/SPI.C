#include "SPI.H"

#pragma interrupt spi1Interrupt ipl7 vector 23

Queue gOutQueue;
QueueItem* gQueueTransmitting;
QueueItem* gQueueProcessing;
QueueItem* gQueueReceiving;
Queue gInQueue;
Ticket gNextTicket=0;
bool gQueueDataAB=true;
//uint8_t gDataA_B;

Ticket spiEnqueue(Data dataA,Data dataB) {
	QueueItem* newItem = (QueueItem*)malloc(sizeof(QueueItem));
	if (newItem) {
		newItem->dataA = dataA;
		newItem->dataB = dataB;
		newItem->ticket = (gNextTicket)?(gNextTicket):(++gNextTicket);
		++gNextTicket;
		newItem->behind=0;
//		asm("di");
		if (gOutQueue.tail) {
			gOutQueue.tail->behind = newItem;
			gOutQueue.tail = newItem;
		} else {
			gOutQueue.tail = newItem;
			gOutQueue.head = newItem;
		}
//		asm("ei");
		return newItem->ticket;
	} else {
		return 0;
	}
}

QueueItem spiDequeue(Ticket ticket) {
	QueueItemStatus status=spiGetQueueItemStatus(ticket);
		while(status != QueueItemStatus_DoesNotExist) {//This loop will block until we know the ticket doesn't exist or the item is ready.
			if (status == QueueItemStatus_WaitingIn) {//If the item is in the input queue.
				QueueItem desiredQueueItem;
				QueueItem* trail = gInQueue.head;//Start searching at the head of the in queue.
				if (trail->ticket == ticket) {//And if it is at the head of the queue...
					desiredQueueItem = *trail;//Set our return value (copy).
					gInQueue.head = trail->behind;//Remove it from the queue.
					if (gInQueue.tail == trail) gInQueue.tail = 0;//If we removed the tail then set it to null.
					free(trail);//Free the original.
				} else {//Or if it wasn't at the head of the queue we're gonna have to search through the queue.
					QueueItem* travel = trail->behind;//Travel will be our index for iterating.
					while(travel->ticket != ticket) {//Search through.
						trail = travel;//Increment trail.
						travel = travel->behind;//Increment travel.
					}
					desiredQueueItem = *travel;//Set our return value (copy).
					trail->behind = travel->behind;//Remove it.
					if (gInQueue.tail == travel) gInQueue.tail = trail;//If we removed the tail then set it to trail.
					free(travel);//Free the original.
				}
				return desiredQueueItem;
			}
			status=spiGetQueueItemStatus(ticket);
		}
		return NULL_ITEM;//The ticket doesn't refference and existing queue item.
}

QueueItemStatus spiGetQueueItemStatus(Ticket ticket) {
	if (ticket) {//If the ticket is not null.
		QueueItem* travel;//Our temporary pointer for searching the queues.
		for (travel=gInQueue.head; travel && travel->ticket != ticket; travel=travel->behind);//Iterate through the in queue searching.
		if (travel) return QueueItemStatus_WaitingIn;//If it was in the input queue return that as the status.
		if (gQueueReceiving && gQueueReceiving->ticket == ticket) return QueueItemStatus_Receiving;//Check if we are receiving it.
		if (gQueueProcessing && gQueueProcessing->ticket == ticket) return QueueItemStatus_Processing;//Check if we are processing it.
		if (gQueueTransmitting && gQueueTransmitting->ticket == ticket) return QueueItemStatus_Transmitting;//Check if we are transmitting it.
		for (travel=gOutQueue.head; travel && travel->ticket != ticket; travel=travel->behind);//Iterate through the out queue searching.
		if (travel) return QueueItemStatus_WaitingOut;//If it was in the output queue return that as the status.
	}
	return QueueItemStatus_DoesNotExist;//If we didn't return a status yet then we didn't find it, so it doesn't exist.
}

void spi1Interrupt() {
	Data trash;
	IFS0CLR =			//Interrupt Flags(-)
		_IFS0_SPI1EIF_MASK |	//Error interrupt
		_IFS0_SPI1TXIF_MASK |	//Transmit interrupt
		_IFS0_SPI1RXIF_MASK;	//Recieve interrupt
	if (SPI1STAT & _SPI1STAT_SPIROV_MASK) {//If we had an overflow.
		SPI1CONCLR= _SPI1CON_ON_MASK; //SPI1 Config(-): Turn it off.
		LATECLR = PORTE_ACDRESET_MASK;//PORT E(-): Activate the reset for the ACD.
		IEC0CLR = 			//Interrupt Enable(-)
			_IEC0_SPI1EIE_MASK |	//Error interrupt
			_IEC0_SPI1TXIE_MASK |	//Transmit interrupt
			_IEC0_SPI1RXIE_MASK;	//Recieve interrupt
	} else {
			if (gQueueDataAB) {//If we are sending an A word.
				if (gQueueReceiving) gQueueReceiving->dataA = SPI1BUF;
				else trash = SPI1BUF;
				if (gQueueTransmitting) SPI1BUF = gQueueTransmitting->dataA;
				else SPI1BUF = (Data)0;
//				else SPI1BUF = (Data)0x3C5A;
			} else {//If we are sending a B word.
				if (gQueueReceiving) {
					gQueueReceiving->dataB = SPI1BUF;
					if (gInQueue.tail) {
						gInQueue.tail->behind=gQueueReceiving;
						gInQueue.tail=gQueueReceiving;
					} else {
						gInQueue.tail=gInQueue.head=gQueueReceiving;
					}
				} else {
					trash = SPI1BUF;
				}
				gQueueReceiving=gQueueProcessing;
				gQueueProcessing=gQueueTransmitting;
				if (gQueueTransmitting) {
					SPI1BUF = gQueueTransmitting->dataB;
				} else {
					SPI1BUF = (Data)0;
//					SPI1BUF = (Data)0x3CA5;
				}
				gQueueTransmitting=gOutQueue.head;
				if (gOutQueue.head) gOutQueue.head = gOutQueue.head->behind;
				if (!gOutQueue.head) gOutQueue.tail = 0;
			}
		gQueueDataAB = !gQueueDataAB;
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
	IPC5CLR =			//Interrupt Priority Control(-).
		_IPC5_SPI1IP_MASK |	//Clear interrupt priority.
		_IPC5_SPI1IS_MASK;	//Clear interrupt sub-priority.
	IPC5SET =				//Interrupt Priority Control(+).
		(7 << _IPC5_SPI1IP_POSITION) |	//Set to priority 7.
		(3 << _IPC5_SPI1IP_POSITION);	//Set to sub-priority 3.
	SPI1STATCLR=_SPI1STAT_SPIROV_MASK;//SPI1 Status(-): Clear recieve overflow.
	SPI1BRG=CLK_DIV >> 3;//SPI1 Baud Rate Generator(=): Set the divider to 1/8th the clock requency.
	SPI1CONSET=			//SPI1 Config(+)
		_SPI1CON_FRMEN_MASK |	//Frame enable
		_SPI1CON_FRMSYNC_MASK |	//Frame sync
		_SPI1CON_FRMPOL_MASK |	//Frame polarity
		_SPI1CON_MODE16_MASK;	//16 Bit mode
	IEC0SET = 			//Interrupt Enable(+)
		_IEC0_SPI1EIE_MASK |	//Error interrupt
		_IEC0_SPI1RXIE_MASK;	//Recieve interrupt
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

	gQueueTransmitting=0;
	gQueueProcessing=0;
	gQueueReceiving=0;

	while(!(SPI1STAT & _SPI1STAT_SPITBE_MASK)) SPI1BUF = (Data)0;

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
