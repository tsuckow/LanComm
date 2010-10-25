#include "SPI.H"

/*	initSPI1
 *
 *	Initializes SPI1 to be used for communication with the ACD.
 */
int spiInitSPI1() {
/*
	The following code example will initialize the SPI1 in Slave mode.
	It assumes that none of the SPI1 input pins are shared with an analog
	input. If so, the AD1PCFG and corresponding TRIS registers have to be
	properly configured.
*/
	uint32_t trash;
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
//	IPC5SET =				//Interrupt Priority Control(+).
//		(7 << _IPC5_SPI1IP_POSITION) |	//Set to priority 7.
//		(3 << _IPC5_SPI1IP_POSITION);	//Set to sub-priority 3.
	SPI1STATCLR=_SPI1STAT_SPIROV_MASK;//SPI1 Status(-): Clear recieve overflow.
	//SPI1 Baud Rate Generator(=): Set the divider to 1/46th the clock
	//frequency.
	SPI1BRG= 0x22;
	SPI1CONSET=			//SPI1 Config(+)
		_SPI1CON_MODE32_MASK |	//32 Bit mode
		_SPI1CON_CKE_MASK |	//Set the clock edge to rising edge.
		_SPI1CON_MSTEN_MASK;	//Master mode
//	IEC0SET = 			//Interrupt Enable(+)
//		_IEC0_SPI1EIE_MASK |	//Error interrupt
//		_IEC0_SPI1RXIE_MASK;	//Recieve interrupt
	SPI1CONSET= _SPI1CON_ON_MASK; //SPI1 Config(+): Turn it on.
	// from now on, the device is ready to receive and transmit data
	
//	while(!(SPI1STAT & _SPI1STAT_SPITBE_MASK)) SPI1BUF = (Data)0;

	return 0;//Return no errors.
}

//	spiClearOverflow() -- Clears the recieve overflow of SPI1.
inline void spiClearOverflow() {
	/*SPI1 Status(-): Clear recieve overflow.*/
	SPI1STATCLR=_SPI1STAT_SPIROV_MASK;
}
//	spiBusySPI1 -- Spins polling the busy bit of the SPI1 status register.
inline void spiBusySPI1() {while(SPI1STAT & _SPI1STAT_SPIBUSY_MASK);}

//	spiUntilTBempty -- Spins polling until the transmit buffer of SPI1 is
//	empty.
inline void spiUntilTBempty() {
	/*Wait for the transmit buffer to be empty.*/
	while(!(SPI1STAT & _SPI1STAT_SPITBE_MASK));
}
//	spiUntilRBfull -- Spins polling until the recieve buffer of SPI1 is
//	full.
inline void spiUntilRBfull() {
	/*Wait for the recieve buffer to be full.*/
	while(!(SPI1STAT & _SPI1STAT_SPIRBF_MASK));
}

//	spiWrite -- Writes 2 bytes to SPI1.
inline void spiWrite(uint32_t data) {SPI1BUF=data;}
/*	spiWriteBlocked
 *
 *	Writes 2 bytes to SPI1.  Before the write this function will block with
 *	a call to spiUntilTBempty.
 */
inline void spiWriteBlocked(uint32_t data) {
	spiUntilTBempty();
	spiWrite(data);
}

//	spiRead -- Reads 2 bytes from from SPI1.
inline uint32_t spiRead() {return SPI1BUF;}
/*	spiReadBlocked
 *
 *	Reads 2 bytes from SPI1.  Before the read this function will block with
 *	a call to spiUntilRBfull.
 */
inline uint32_t spiReadBlocked() {
	spiUntilRBfull();
	return spiRead();
}
