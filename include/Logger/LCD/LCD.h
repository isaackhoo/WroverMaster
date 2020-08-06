#pragma once

#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include "../lib/WROVER_KIT_LCD-master/src/WROVER_KIT_LCD.h"

namespace LCD
{
    extern bool init();
    extern void display(String);
    extern void displayError(String);
}; // namespace LCD

#endif