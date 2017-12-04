/***************************************************************************
* File Name: PWFusion_MCP4725_12DAC.cpp
* Processor/Platform: Arduino Uno R3 (tested)
* Development Environment: Arduino 1.6.2
*
* Designed for use with with Playing With Fusion MCP4725 12-bit DAC breakout 
* board: SEN-20001. Also used with SEN-39002 - Lightning Emulator shield
*   ---> http://playingwithfusion.com/
*
* Copyright � 2017 Playing With Fusion, Inc.
* SOFTWARE LICENSE AGREEMENT: This code is released under the MIT License.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
* **************************************************************************
* REVISION HISTORY:
* Author		    Date		Comments
* J. Steinlage		2017Jan16	Original version
*
* Playing With Fusion, Inc. invests time and resources developing open-source
* code. Please support Playing With Fusion and continued open-source
* development by buying products from Playing With Fusion!
*
* **************************************************************************
* ADDITIONAL NOTES:
* This file contains functions to initialize and run an Arduino Uno R3 in
* order to communicate with an MCP4725 12-bit DAC breakout
*   - Write MCP4725 registers to set output voltage ratio
*		(Vout = Vin * REG/4096)
*	- Ability to write in either DAC or EEPROM mode		
*   - can also turn off the DAC, setting Vout to be pulled-down by 1k resistor
***************************************************************************/

#include "SDL_PWFusion_MCP4725_12DAC.h"

PWFusion_MCP4725::PWFusion_MCP4725(uint8_t address)
{
  _i2cadd = address;
}

// begin: set I2C address, start I2C interface
void PWFusion_MCP4725::begin() 
{
  Wire.begin();
}

// setOutput: set 12-bit DAC to ratio of Vin. Range is 0-4095
// writeNVmem: set this to TRUE to save the value to EEPROM, saves value over power cycle
// set powerDown to TRUE to turn off the DAC
void PWFusion_MCP4725::setOutput(uint16_t output, bool writeNVmem, bool powerDown)
{
	// start transmission with I2C address
	Wire.beginTransmission(_i2cadd);
	
	// next check if power down is set
	if(powerDown){
		Wire.write(MCP4725_DAC_OFF);
		Wire.write(0x00);
		Wire.endTransmission();
		return;
	}
	else{
		// next set write command - DAC or EEPROM write
		if(writeNVmem)			// EEPROM Save selected
		{
			Wire.write(MCP4725_EEPROM_SET);
		}
		else					// DAC (non-NV) selected
		{
			Wire.write(MCP4725_DAC_SET);
		}
		// next, write value to DAC registers
		Wire.write((output & 0x0FFF)>>4);	// upper byte of output data
		Wire.write(output & 0x000F);		// lower 4 bits of output data
		Wire.endTransmission();
	}
}
