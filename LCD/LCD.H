#ifndef LCD_H
#define LCD_H

#include "../OS/OS.H"

void lcd_init_pins();
void lcd_send_comm_addr();
void lcd_send_data();
void lcd_send_string();
void lcd_config();

#endif
