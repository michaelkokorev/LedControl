/*
 *    LedControlX.cpp - A library for controling Leds with a MAX7219/MAX7221
 *    Copyright (c) 2007 Eberhard Fahle
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
 

#include "LedControlX.h"

#define OP_HORIZONTAL	0
#define OP_VERTICAL		1

LedControlX::LedControlX(int dataPin, int clkPin, int csPin, int numDevices)
:LedControl(dataPin, clkPin, csPin, numDevices){
}

int LedControlX::getMaxWidth() {
	return (maxDevices * 8);
}


void LedControlX::clearAll() {
    int offset;

	for(int addr = 0; addr < maxDevices; addr ++) {
		offset = addr * 8;
		for(int i = 0; i < 8; i++) {
			status[offset + i] = 0;
			spiTransfer(addr, i + 1, status[offset + i]);
		}
	}
}

void LedControlX::setLedEx(int posX, int posY, boolean state) {

   if(posX < 0 || posX >= maxDevices * 8) return;

    if(posY < 0 || posY > 8) return;

    byte val = B10000000 >> posY;

    if(state)
		status[posX] = status[posX] | val;
    else {
		val = ~val;
		status[posX] = status[posX] & val;
    }

	int offset = (posX / 8) * 2;
    int maxbytes = maxDevices * 2;

    for(int i = 0; i < maxbytes; i++) spidata[i] = (byte)0;

    spidata[offset + 1] = (posX % 8) + 1;
    spidata[offset] = status[posX];

    digitalWrite(SPI_CS, LOW);

		for(int i = maxbytes; i > 0; i--) shiftOut(SPI_MOSI, SPI_CLK, MSBFIRST, spidata[i - 1]);

    digitalWrite(SPI_CS, HIGH);
}
		
void LedControlX::setRowEx(byte posX, byte value) {
    if(posX < 0 || posX >= maxDevices * 8) return;

    status[posX] = value;

	int offset = (posX / 8) * 2;
    int maxbytes = maxDevices * 2;

    for(int i = 0; i < maxbytes; i++) spidata[i] = (byte)0;

    spidata[offset + 1] = (posX % 8) + 1;
    spidata[offset] = status[posX];

    digitalWrite(SPI_CS, LOW);

		for(int i = maxbytes; i > 0; i--) shiftOut(SPI_MOSI, SPI_CLK, MSBFIRST, spidata[i - 1]);

    digitalWrite(SPI_CS, HIGH);
}

void LedControlX::printChar(unsigned char sym, int posX, int posY, const unsigned char *Font) {
	int		offset, charOffset;
	int		maxbytes = maxDevices * 2;

	if ((sym >= pgm_read_byte_near(Font + 2)) && (sym <= (pgm_read_byte_near(Font + 2) + pgm_read_byte_near(Font + 3)))) {

		charOffset = (sym - pgm_read_byte_near(Font + 2)) * 8 + 23;

		for(int row = 0; row < 8; row++, posX++) {
			if((posX < 0) || (posX >= maxDevices * 8)) continue;
			
			status[posX] = 0;

			for(int col = 0; col < 8; col++) status[posX] |= ((pgm_read_byte_near(Font + charOffset + col) >> (7 - row)) & 0x01) << col;

			if((posY > -8) && (posY < 8)) {

				if(posY > 0) 
					status[posX] = status[posX] >> posY;
				else
					status[posX] = status[posX] << (-posY);

				offset = (posX / 8) * 2;

				for(int i = 0; i < maxbytes; i++) spidata[i] = (byte)0;

				spidata[offset + 1] = (posX % 8) + 1;
				spidata[offset] = status[posX];

				digitalWrite(SPI_CS, LOW);

					for(int i = maxbytes; i > 0; i--) shiftOut(SPI_MOSI, SPI_CLK, MSBFIRST, spidata[i - 1]);

				digitalWrite(SPI_CS, HIGH);
			}
		}
	}
}

void LedControlX::printStr(unsigned char *s, int posX, int posY, const unsigned char *Font) {
	int p = posX;
	int width = pgm_read_byte(Font);

	while (*s) {
		printChar(*s, p, posY, Font);
		s++;
		p += width + 1;
    }
}

// печатаем строку
void LedControlX::printString(String s, int posX, int posY, const unsigned char *Font) {
	unsigned char buf[s.length() + 1];
	for (unsigned char i = 0; i < s.length(); i ++) {
		buf[i] = s[i];
	}

	buf[s.length()] = '\0';  
	printStr(buf, posX, posY, Font);
} 

// печатаем строку
void LedControlX::buildString(String s, int posX, int posY, int Orientation, const unsigned char *Font) {
	int		charOffset;
	byte	statusData, temp;
	int		width = pgm_read_byte_near(Font);
	int		height = pgm_read_byte_near(Font + 1);

	for(int k = 0; k < s.length(); k ++) {
		if ((s[k] >= pgm_read_byte_near(Font + 2)) && (s[k] <= (pgm_read_byte_near(Font + 2) + pgm_read_byte_near(Font + 3)))) {

			charOffset = (s[k] - pgm_read_byte_near(Font + 2)) * 8 + 23;

			for(int row = 0; row < width; row++) {
				if(Orientation == OP_HORIZONTAL) {
					if(((posX + k * width + row) < 0) || ((posX + k * width + row) >= maxDevices * 8)) continue;
			
					statusData = 0;

					for(int col = 0; col < 8; col++) statusData |= ((pgm_read_byte_near(Font + charOffset + col) >> (width - 1 - row)) & 0x01) << col;

					if((posY > -height) && (posY < height)) {

						if(posY > 0) 
							statusData = statusData >> posY;
						else
							statusData = statusData << (-posY);

						status[posX + k * width + row] = statusData;
					}
				}
				else if (Orientation == OP_VERTICAL){
					if(((posX + row) < 0) || ((posX + row) >= maxDevices * 8)) continue;
			
					statusData = 0;

					for(int col = 0; col < 8; col++) statusData |= ((pgm_read_byte_near(Font + charOffset + col) >> (width - 1 - row)) & 0x01) << col;

					temp = status[posX + row];

					if(((posY + k * height) > -height) && ((posY + k * height) < height)) {

						if((posY + k * height) > 0) {
							statusData = statusData >> (posY + k * height); 
							temp &= ~(0xff >> (posY + k * height));
						}
						else {
							statusData = statusData << (-(posY + k * height)); 
							temp &= ~(0xff << (-(posY + k * height)));
						}

						status[posX + row] = statusData | temp;
					}
				}
			}
		}
	}
}

// печатаем строку
void LedControlX::buildChar(char c, int posX, int posY, int Orientation, const unsigned char *Font) {
	int		charOffset;
	byte	statusData, temp;
	int		width = pgm_read_byte_near(Font);
	int		height = pgm_read_byte_near(Font + 1);

	if ((c >= pgm_read_byte_near(Font + 2)) && (c <= (pgm_read_byte_near(Font + 2) + pgm_read_byte_near(Font + 3)))) {

		charOffset = (c - pgm_read_byte_near(Font + 2)) * 8 + 23;

		for (int row = 0; row < width; row++) {
			if (Orientation == OP_HORIZONTAL) {
				if (((posX + row) < 0) || ((posX + row) >= maxDevices * 8)) continue;

				statusData = 0;

				for (int col = 0; col < 8; col++) statusData |= ((pgm_read_byte_near(Font + charOffset + col) >> (width - 1 - row)) & 0x01) << col;

				if ((posY > -height) && (posY < height)) {

					if (posY > 0)
						statusData = statusData >> posY;
					else
						statusData = statusData << (-posY);

					status[posX + row] = statusData;
				}
			}
			else if (Orientation == OP_VERTICAL) {
				if (((posX + row) < 0) || ((posX + row) >= maxDevices * 8)) continue;

				statusData = 0;
				for (int col = 0; col < 8; col++) statusData |= ((pgm_read_byte_near(Font + charOffset + col) >> (width - 1 - row)) & 0x01) << col;
				temp = status[posX + row];
				if ((posY > -height) && (posY < height)) {
					if (posY > 0) {
						statusData = statusData >> posY;
						temp &= ~(0xff >> posY);
					}
					else {
						statusData = statusData << (-posY);
						temp &= ~(0xff << (-posY));
					}
					status[posX + row] = statusData | temp;
				}
			}
		}
	}
}

void LedControlX::Update(bool Inverse) {
	int		maxbytes = maxDevices * 2;
	int		offset;

	for(int i = 0; i < maxbytes; i++) spidata[i] = (byte)0;

	for(int m = 0; m < 8; m ++) {
		for(int l = 0; l < maxDevices; l ++) {
			offset = l * 2;

			spidata[offset + 1] = m + 1;
			spidata[offset] = (Inverse) ? !status[l * 8 + m] : status[l * 8 + m];
		}

		digitalWrite(SPI_CS, LOW);

			for(int i = maxbytes; i > 0; i--) shiftOut(SPI_MOSI, SPI_CLK, MSBFIRST, spidata[i - 1]);

		digitalWrite(SPI_CS, HIGH);
	}
}