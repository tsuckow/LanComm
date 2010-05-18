#include <p32xxxx.h>
#include <plib.h>

//FUCK THIS TIMING SHIT IN THE FUCKING ASS, MOTHER FUCKER

#pragma config POSCMOD=XT, FNOSC=PRIPLL, FPLLIDIV=DIV_2, FPLLMUL=MUL_20, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_1, FWDTEN=OFF, CP=OFF, BWP=OFF

//I AM GOING TO BREAK SOMETHING

int  LCDConfiguration();
int  TMRConfiguration();
void TMRRST(int num);
int  LCDInitialization();
int  LCDWrite(int value);

int main()
{
	LCDConfiguration();
	TMRConfiguration();
	LCDInitialization();

	LCDWrite(0x53);		//S
	LCDWrite(0x75);		//u
	LCDWrite(0x63);		//c
	LCDWrite(0x6B);		//k

	LCDWrite(0x20);		//' '

	LCDWrite(0x69);		//i
	LCDWrite(0x74);		//t
	LCDWrite(0x21);		//!

	return 0;
}

int LCDConfiguration()
{
	TRISACLR = 0xC0FF;
	ODCACLR = 0xC0FF;
	TRISECLR = 0x0001;
	ODCECLR = 0x1;

	LATACLR = 0xC0FF;
		 
	return 0;
}

int TMRConfiguration()
{
	T2CONSET = 0x8;
	TMR2 = 0x0;
	PR2 = 0x432380;
	T2CONSET = 0x8000;

	return 0;
} 

int LCDInitialization()
{
	//Wait = 55ms
  	while((IFS0 & 0x1000) != 0x1000){}

	//Write = 00111000 (Primary Init)
	LATESET = 0x1;
	LATASET = 0x38;
	TMRRST(3);
	while((IFS0 & 0x1000) != 0x1000){}
	LATECLR = 0x1;
	TMRRST(4);
	while((IFS0 & 0x1000) != 0x1000){}

	//Wait = 4.1ms
	TMRRST(0);
	while((IFS0 & 0x1000) != 0x1000){}
	
	//Write = 00110000 (Primary Init)
	LATESET = 0x1;
	//LATASET = 0x38;
	TMRRST(3);
	while((IFS0 & 0x1000) != 0x1000){}
	LATECLR = 0x1;
	TMRRST(4);
	while((IFS0 & 0x1000) != 0x1000){}

	//Wait = 100us
	TMRRST(1);
	while((IFS0 & 0x1000) != 0x1000){}

	//Write = 00110000 (Primary Init)
	LATESET = 0x1;
	//LATASET = 0x38;
	TMRRST(3);
	while((IFS0 & 0x1000) != 0x1000){}
	LATECLR = 0x1;
	TMRRST(4);
	while((IFS0 & 0x1000) != 0x1000){}

	//Write = 00111000 (Function Set)
	LATESET = 0x1;
	//LATASET = 0x38;
	TMRRST(3);
	while((IFS0 & 0x1000) != 0x1000){}
	LATECLR = 0x1;
	TMRRST(4);
	while((IFS0 & 0x1000) != 0x1000){}

	//Write = 00001110	(Display On/Off)
	LATESET = 0x1;
	LATACLR = 0xFF;
	LATASET = 0x0E;
	TMRRST(3);
	while((IFS0 & 0x1000) != 0x1000){}
	LATECLR = 0x1;
	TMRRST(4);
	while((IFS0 & 0x1000) != 0x1000){}

	//Write = 00000001	(Clear Display)
	LATESET = 0x1;
	LATACLR = 0xFF;
	LATASET = 0x01;
	TMRRST(3);
	while((IFS0 & 0x1000) != 0x1000){}
	LATECLR = 0x1;
	TMRRST(7);
	while((IFS0 & 0x1000) != 0x1000){}

	//Write = 00000110	(Entry Mode Set)
	LATESET = 0x1;
	LATACLR = 0xFF;
	LATASET = 0x06;
	TMRRST(3);
	while((IFS0 & 0x1000) != 0x1000){}
	LATECLR = 0x1;
	TMRRST(4);
	while((IFS0 & 0x1000) != 0x1000){}

	T2CONCLR = 0x8000;

	return 0;
}

void TMRRST(int num)
{
	T2CONCLR = 0x8000;
	
	if(num == 0)			//4.1ms     (Only used in initialization).
	{PR2SET = 0x50140;}
	else if(num == 1)		//100us     (Only used in initialization).
	{PR2SET = 0x1F40;}
	else if(num == 2)		//...not actually used, left over.
	{PR2SET = 0x1860;}
	else if(num == 3)		//230ns     (Tpw min for E).
	{PR2SET = 0x13;}
	else if(num == 4)		//100.27us   (Tc (how long before asserting E again) + 100us (length of most ops)).
	{PR2SET = 0x1F56;}
	else if(num == 5)		//40ns      (Tsp1 (Time from RS/RW assertion till E assertion)).
	{PR2SET = 0x4;}
	else if(num == 6)		//10ns	    (Thd1 (Time to hold RS/RW after lowering E).
	{PR2SET = 0x1;}
	else					//4.10027ms (Tc (how long before asserting E again) + 4.1ms (length of clear and return)).
	{PR2SET = 0x50156;}
	
	T2CONSET = 0x8000;
} 

int LCDWrite(int value)
{
	LATASET = 0x4000;
	TMRRST(5);
	while((IFS0 & 0x1000) != 0x1000){}
	LATESET = 0x1;
	LATACLR = 0xFF;
	LATASET = value;
	TMRRST(3);
	while((IFS0 & 0x1000) != 0x1000){}
	LATECLR = 0x1;
	TMRRST(6);
	while((IFS0 & 0x1000) != 0x1000){}
	LATACLR = 0x4000;
	TMRRST(4);
	while((IFS0 & 0x1000) != 0x1000){}
	
	return 0;
}
