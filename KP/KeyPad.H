#ifndef KP_H
#define KP_H

#include "../OS/OS.H"

//The timeout in micro seconds.
#define TIMEOUT		4000
//The rowscan delay in micro seconds.
#define ROWSCAN		100
#define ROW_PORT	0xF
#define COL1_PORT	0x10
#define COL2_PORT	0x1
#define COL3_PORT	0x2
#define COL4_PORT	0x4
#define COL_PORT	0x17
#define COL_CN		0x5C

int kpInitialize();
void kpTest();
void kpShutdown();

extern volatile char kpChar;
extern volatile char kpLastChar;
#endif
