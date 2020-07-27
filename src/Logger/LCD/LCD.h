#pragma once

#ifndef LCD_H
#define LCD_H

#include "../lib/WROVER_KIT_LCD-master/src/WROVER_KIT_LCD.h"

// Do not include helper.h in LCD.
// will result in circular referencing

// public methods
extern void LcdInit();
extern int LcdDoScroll(int, int);
extern int LcdScrollText(const char *);
extern void setLcdTextColorError();
extern void setLcdTextColorPrimary();

#endif