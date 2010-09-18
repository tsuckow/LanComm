#ifndef LCD_H
#define LCD_H

#include "../OS/OS.H"

int lcdInitialize();
void lcdTest();
void lciShutdown();
void lcdInitPins();
void lcdSendCommAddr();
void lcdSendData();
void lcdSendString();
void lcdConfig();

#endif
