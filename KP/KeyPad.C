#include "KeyPad.H"

#pragma interrupt kpPinChangeInterrupt ipl5 vector 26
#pragma interrupt kpTimeoutInterrupt ipl5 vector 12
#pragma interrupt kpRowScan ipl5 vector 16

volatile char kpChar='\0';
volatile char kpLastChar='\0';
volatile uint8_t kpNewChar=0;
volatile uint8_t row=0;
int rows[] = {0x1,0x2,0x4,0x8};
volatile uint8_t colPins;

void kpPinChangeInterrupt() {
	IFS1CLR = _IFS1_CNIF_MASK;//Clear change notice flags.
	colPins = PORTB & COL_PORT;//Clear the change notice.
	TMR2=0;//Timer 2 (=): Clear the value of timer 2.
	T4CONCLR = _T4CON_ON_MASK;
	IFS0CLR = _IFS0_T4IF_MASK;//Interrupt Flags (-): Clear timer 4 flags.
}

void kpTimeoutInterrupt() {
	IFS0CLR = _IFS0_T3IF_MASK;//Interrupt Flags (-): Clear timer 23 flags.
	if (kpChar != '\0') {
		kpLastChar = kpChar;
		kpNewChar = 1;
	}
	uint8_t col=0xF0;
	if (!(colPins & COL1_PORT)) {
		col=0x0;
	} else if (!(colPins & COL2_PORT)) {
		col=0x4;
	} else if (!(colPins & COL3_PORT)) {
		col=0x8;
	} else if (!(colPins & COL4_PORT)) {
		col=0xC;
	}
	switch(row | col) {
		case 0x0: kpChar='1';break;
		case 0x1: kpChar='4';break;
		case 0x2: kpChar='7';break;
		case 0x3: kpChar='D';break;
		case 0x4: kpChar='2';break;
		case 0x5: kpChar='5';break;
		case 0x6: kpChar='8';break;
		case 0x7: kpChar='0';break;
		case 0x8: kpChar='3';break;
		case 0x9: kpChar='6';break;
		case 0xA: kpChar='9';break;
		case 0xB: kpChar='E';break;
		case 0xC: kpChar='A';break;
		case 0xD: kpChar='B';break;
		case 0xE: kpChar='C';break;
		case 0xF: kpChar='F';break;
		default: kpChar='\0';
			 TMR4=0;
			 T4CONSET = _T4CON_ON_MASK;
	}
}

void kpRowScan() {
	IFS0CLR = _IFS0_T4IF_MASK;//Interrupt Flags (-): Clear timer 4 flags.
//	LATASET=rows[row];
	LATASET=ROW_PORT;
//	row=(row+1)%4;
//	row+=1;
//	if(row>3) row=0;
//	LATACLR=rows[row];
//	LATESET = 0xC;
//	LATECLR = (row & 0x3) << 2;
	switch(row) {
		case 0:LATACLR=0x2;row=1;break;
		case 1:LATACLR=0x4;row=2;break;
		case 2:LATACLR=0x8;row=3;break;
		case 3:LATACLR=0x1;row=0;break;
		default:LATACLR=0x0;row=0;
	}
}

void kpTest() {
//	LATECLR = 0x04;//PORT E (-): Indicate progress.
	while(1) {
		LATECLR=0x1;
		while(kpChar=='\0');
		LATESET=0x1;
		while(kpChar!='\0');
//		LATECLR=0x1;
//		DDPCONbits.JTAGEN = 1;
//		while(1);
	}
}

int kpInitialize() {
	//Turn off jtag, because we don't need it (and it's in the way).
	DDPCONbits.JTAGEN = 0;
	//INITIALIZE THE TIMEOUT TIMER
	T2CON = 0;//Timer 23 Config (=): Turn off timer 23 and clear the config.
	IEC0CLR = _IEC0_T3IE_MASK;//Interrupt Enable (-): Disable timer 23.
	IFS0CLR = _IFS0_T3IF_MASK;//Interrupt Flags (-): Clear timer 23 flags.
	IPC3CLR = _IPC3_T3IP_MASK | _IPC3_T3IS_MASK;//Clear interrupt priority.
	IPC3SET =	//Set interrupt priority.
		(5 << _IPC3_T3IP_POSITION) |
		(1 << _IPC3_T3IS_POSITION);
	IEC0SET = _IEC0_T3IE_MASK;//Interrupt Enable (+): Enable timer 23.
	T2CONSET = 	//Timer 23 Config (+)
		_T2CON_T32_MASK;	//32 bit mode.
	TMR2=0;//Timer 23 (=): Clear the value of timer 23.
	PR2=(uint32_t)(80*TIMEOUT);//Timer 23 (=): Set the period.
	
	//INITIALIZE THE ROWSCAN TIMER
	T4CON = 0;//Timer 4 Config (=): Turn off timer 4 and clear the config.
	IEC0CLR = _IEC0_T4IE_MASK;//Interrupt Enable (-): Disable timer 4.
	IFS0CLR = _IFS0_T4IF_MASK;//Interrupt Flags (-): Clear timer 4 flags.
	IPC4CLR = _IPC4_T4IP_MASK | _IPC4_T4IS_MASK;//Clear interrupt priority.
	IPC4SET =	//Set interrupt priority.
		(5 << _IPC4_T4IP_POSITION) |
		(3 << _IPC4_T4IS_POSITION);
	IEC0SET = _IEC0_T4IE_MASK;//Interrupt Enable (+): Enable timer 4.
	TMR4=0;//Timer 4 (=): Clear the value of timer 4.
	PR4=(uint16_t)(80*ROWSCAN);//Timer 4 (=): Set the period.
	
	//INITIALIZE PINS
	LATASET = ROW_PORT;//Set the row pins high.
	TRISACLR = ROW_PORT;//Set the row pins to output.
//	ODCASET = ROW_PORT;//Set the row pins to open drain.
//	TRISCSET = COL1_PORT;//Set COL1 to input;
	TRISBSET = COL_PORT;//Set the other COL pins to input.
	//INITIALIZE THE CHANGE NOTICE
	CNCONCLR = _CNCON_ON_MASK;//Turn off the change notice.
	IEC1CLR = _IEC1_CNIE_MASK;//Turn off change notice interrupts.
	IFS1CLR = _IFS1_CNIF_MASK;//Clear change notice flags.
	AD1PCFGSET = COL_PORT;//Set the COL pins for digital.
//	CNPUESET = COL_CN;//Set the COL pins to have a weak pull up.
	CNEN = COL_CN;//Enable the change notice for the COL pins.
	IPC6CLR = _IPC6_CNIP_MASK | _IPC6_CNIS_MASK;//Clear interrupt priority.
	IPC6SET =	//Set interrupt priority.
		(5 << _IPC6_CNIP_POSITION) |
		(2 << _IPC6_CNIS_POSITION);
	IEC1SET = _IEC1_CNIE_MASK;//Turn on change notice interrupts.
	//TURN ON CHANGE NOTICE
	CNCONSET = _CNCON_ON_MASK;//Turn on the change notice.
	//TURN ON THE TIMERS
	T2CONSET = _T2CON_ON_MASK;
	T4CONSET = _T4CON_ON_MASK;
	return 0;
}

void kpShutdown() {
}
