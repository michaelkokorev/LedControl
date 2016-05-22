/*
 *    LedControlX.h - A library for controling Leds with a MAX7219/MAX7221
 *    Copyright (c) 2016 Eberhard Fahle
 * 
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 * 
 *    This permission notice shall be included in all copies or 
 *    substantial portions of the Software.
 * 
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef LedControlX_h
#define LedControlX_h

#include <avr/pgmspace.h>

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
  
#include "LedControl.h"

class LedControlX : public LedControl{
 public:
    /* 
     * Create a new controler 
     * Params :
     * dataPin		pin on the Arduino where data gets shifted out
     * clockPin		pin for the clock
     * csPin		pin for selecting the device 
     * numDevices	maximum number of devices that can be controled
     */
    LedControlX(int dataPin, int clkPin, int csPin, int numDevices=1);

    /* 
     * Switch all Leds on the displayes off. 
     */
	void clearAll() ;

    /* 
     * Set the status of a single Led.
     * Params :
     * posX	the position X of the Leds array 
     * posY	the position Y of the Leds array
     * state	If true the led is switched on, 
     *		if false it is switched off
     */
	void setLedEx(int posX, int posY, boolean state);

    /* 
     * Set all 8 Led's in a row to a new state
     * Params:
     * posX	position
     * value	each bit set to 1 will light up the
     *		corresponding Led.
     */
	void setRowEx(byte posX, byte value);

	void printChar(unsigned char sym, int posX, int posY, const unsigned char *Font);
	void printStr(unsigned char *s, int posX, int posY, const unsigned char *Font);
	void printString(String s, int posX, int posY, const unsigned char *Font);
	int	 getMaxWidth();
	void buildString(String s, int posX, int posY, int Orientation, const unsigned char *Font);
	void buildChar(char c, int posX, int posY, int Orientation, const unsigned char *Font);
	void Update(bool Inverse = false);
 };

#endif	//LedControlX.h



