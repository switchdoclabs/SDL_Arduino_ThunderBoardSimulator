/***************************************************************************
* File Name: PWFusion_MCP4725_12DAC.h
* Processor/Platform: Arduino Uno R3 (tested)
* Development Environment: Arduino 1.6.2
*
* Designed for use with with Playing With Fusion MCP4725 12-bit DAC breakout 
* board: SEN-20001. Also used with SEN-39002 - Lightning Emulator shield
*   ---> http://playingwithfusion.com/
*
* Copyright © 2017 Playing With Fusion, Inc.
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
* *************************************************************************/
#ifndef PWFusion_MCP4725_H
#define PWFusion_MCP4725_H

#include "Arduino.h"
#include "Wire.h"

// Commands to write data vs EEPROM (volatile vs nonvolatile)
#define MCP4725_DAC_SET			0x40	/* set DAC, does not last over power cycle */
#define MCP4725_EEPROM_SET		0x60	/* set DAC, setting lasts over power cycle */

#define MCP4725_DAC_OFF			0x10	/* power down setting - 1k pull-down */ 

class PWFusion_MCP4725{
public:	
  PWFusion_MCP4725(uint8_t address);
  void begin();
  void setOutput(uint16_t output, bool writeNVmem, bool powerDown);
  
private:
  uint8_t _i2cadd;
};
#endif // PWFusion_MCP4725_H