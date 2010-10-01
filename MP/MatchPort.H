#ifndef MP_H
#define MP_H
#include <p32xxxx.h>

#define PORTE_MPRESET_MASK		0x0004

int mpInitialize();
void mpTest();
void mpShutdown();

void mpReset();

#endif
