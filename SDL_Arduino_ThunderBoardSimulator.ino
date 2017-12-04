/*****************
   ThunderBoard Lightning Simulator
   SwitchDoc Labs
   Modified for the Thunder Board Project
   Version 1.0
   December 2017
*/






/***************************************************************************
  Original Software from:

  Copyright © 2017 Playing With Fusion, Inc.
  SOFTWARE LICENSE AGREEMENT: This code is released under the MIT License.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
* **************************************************************************/

#define SOFTWAREVERSION "002"

// I2C library - standard Wire lib
#include "Wire.h"
// include Playing With Fusion MCP4725 lib (https://github.com/PlayingWithFusion/PWFusion_MCP4725)
#include "SDL_PWFusion_MCP4725_12DAC.h"

// declare sensor object
#define DAC_ADD 0x62
PWFusion_MCP4725 dac0(DAC_ADD);


#define SINGLESTRIKEMODE 0


// close strikes every 15 seconds
#define FASTTIMEDSTRIKEMODE   1
// close strikes every 5 minutes
#define SLOWTIMEDSTRIKEMODE 2
// random stikes between 30 seconds and 30 minutes
#define RANDOMTIMEDSTRIKEMODE 3

int timedState = SINGLESTRIKEMODE;


void setup()
{
  Serial.begin(115200);
  Serial.println("***********************************************");
  Serial.println("SwitchDoc Labs ThunderBoard Lightning Simulator");
  Serial.println("***********************************************");
  Serial.print("Version ");
  Serial.println(SOFTWAREVERSION);
  Serial.println("***********************************************");

  // set up DAC
  dac0.begin();

  // setup DAC, set DAC to 'off' and pulled low
  // DAC output / write to NV memory / power down DAC, pull to gnd
  dac0.setOutput(0, false, true);

  // give the Arduino time to start up
  delay(100);

  pinMode(9, INPUT_PULLUP);   // FAR PB
  pinMode(8, OUTPUT);         // FAR
  digitalWrite(8, HIGH);
  pinMode(7, INPUT_PULLUP);   // MID PB
  pinMode(6, OUTPUT);         // MID
  digitalWrite(6, HIGH);
  pinMode(5, INPUT_PULLUP);   // CLO PB
  pinMode(4, OUTPUT);         // CLO
  digitalWrite(4, HIGH);

  uint8_t su_tim = 50;
  volatile uint8_t su_x = 4;
  volatile uint8_t su_cnt;
  // startup LED sequence
  for (su_cnt = 0; su_cnt < su_x; su_cnt++) {
    digitalWrite(4, LOW);
    delay(su_tim);
    digitalWrite(4, HIGH);
    digitalWrite(6, LOW);
    delay(su_tim);
    digitalWrite(6, HIGH);
    digitalWrite(8, LOW);
    delay(su_tim);
    digitalWrite(8, HIGH);
    digitalWrite(6, LOW);
    delay(su_tim);
    digitalWrite(6, HIGH);
  }
  digitalWrite(4, LOW);
  delay(su_tim);
  digitalWrite(4, HIGH);
}

// as calibrated, this profile works from around 7cm and 15cm from inductor to inductor
uint16_t out_array[20] = { 103, 73, 52, 37, 27, 20, 15, 11, 9, 7, 6, 5, 4, 4, 4, 3, 3, 3, 3, 3};
volatile uint8_t i, j, j_cnt;



void executeStrike()  //j, i, j_cnt are declared global (not my cup of tea, but hey)
{
  // now run emulation profile
  for (j = 0; j < j_cnt; j++)
  {
    for (i = 0; i < 19; i++)
    {
      dac0.setOutput(out_array[i], false, false);         // set new command value
      delayMicroseconds(30);    // change drive every 30 microseconds
    }
  }
  delay(20); // end by delaying 4 milliseconds

  // turn off DAC
  dac0.setOutput(0, true, true);
  // hold state for one sec - give sensor time to pick up, report signal
  delay(1000);
  // turn off all LEDs
  digitalWrite(4, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(8, HIGH);
}


void blink4(int count)
{
  int i;
  for (i = 0; i < count; i++)
  {
    digitalWrite(4, LOW);
    delay(100);
    digitalWrite(4, HIGH);
    delay(100);
  }

}

void loop()
{
  int lastState;

  randomSeed(analogRead(0));

  //         close             mid              far
  while (digitalRead(5) & digitalRead(7) & digitalRead(9)); // wait for any button push


  // if close and mid are pressed at the same time, we do close lightning strikes every 15 seconds
  // mode is disabled by pressing close and mid again

  // if mid and far are pressed same time, we do close lightning strkes every 5 minutes
  // mode is disabled by pressing mid and far again

  // if all three buttons are pressed, then random strikes from 30 seconds to 30 minutes

  lastState = digitalRead(5) * 4 + digitalRead(7) * 2 + digitalRead(9);
  delay(500);   // wait for things to stabilize

  if (!digitalRead(5) & !digitalRead(7) & digitalRead(9))
  {
    timedState = FASTTIMEDSTRIKEMODE;

    Serial.println("fast Timed Strike Mode Selected - 30 seconds");

  }
  else if (digitalRead(5) & !digitalRead(7) & !digitalRead(9))
  {
    timedState = SLOWTIMEDSTRIKEMODE;

    Serial.println("slow Timed Strike Mode Selected - 5 minutes");

  }
  else if (!digitalRead(5) & !digitalRead(7) & !digitalRead(9))
  {
    timedState = RANDOMTIMEDSTRIKEMODE;

    Serial.println("random Stike Mode Selected - 30 seconds to 30 minutes");

  }
  else
  {

    // if multi buttons not pushed then fall through to one button pushed
    Serial.print("lastState=");
    Serial.print(lastState, HEX);
    Serial.print(" :5 7 9:");
    Serial.print(digitalRead(5));
    Serial.print(digitalRead(7));
    Serial.println(digitalRead(9));



    if (lastState == 0x03) {                                    // close-range strike simulation requested
      j_cnt = 3;
      digitalWrite(4, LOW);
      Serial.println("Close-range Strike Simulated");
    }
    else if (lastState == 0x05) {                               // mid-range strike simulation requested
      j_cnt = 2;
      digitalWrite(6, LOW);
      Serial.println("Mid-range Strike Simulated");
    }
    else {                                                    // far-range strike simulation requested
      j_cnt = 1;
      digitalWrite(8, LOW);
      Serial.println("Far-range Strike Simulated");
    }

  }


  if (timedState == 0)
  {
    // no multi button mode active


    executeStrike();

  }  // end of single button mode
  else
  {

    // we don't do squat until all the buttons are clear
    while (!digitalRead(5) || !digitalRead(7) || !digitalRead(9))
    {

    }


    // now we must execute multi button mode
    // any button press will terminate mode

    long delayTime;

    long lastBlink;

    lastBlink = millis() / 1000;

    Serial.print("timedState =");
    Serial.println(timedState);

    long checkTime;

    int blinkLight;
    switch (timedState)
    {
      case SINGLESTRIKEMODE:

        Serial.println("Single Strike Mode hit - error");

        break;

      case SLOWTIMEDSTRIKEMODE:

        checkTime = millis() + 5 * 60 * 1000;
        blinkLight = 8;

        break;

      case FASTTIMEDSTRIKEMODE:

        checkTime = millis() + 15 * 1000;
        blinkLight = 6;


        break;

      case RANDOMTIMEDSTRIKEMODE:
        blinkLight = 4;
        long randNumber;
        randNumber = random(30 * 1000, 30 * 60 * 1000);

        checkTime = millis() + randNumber;

        break;

      default:

        Serial.println("Default hit - error");
        break;
    }


    while (digitalRead(5) & digitalRead(7) & digitalRead(9))
    {
      // here we implement the multi button mode




      if (millis() > checkTime)  // time to do our stuff
      {

        switch (timedState)
        {
          case SINGLESTRIKEMODE:

            Serial.println("Single Strike Mode in Timed Mode - error");

            break;

          case SLOWTIMEDSTRIKEMODE:

            checkTime = millis() + 5 * 60 * 1000;
            blinkLight = 8;
            j_cnt = 3;
            blink4(3);
            digitalWrite(4, LOW);
            Serial.println("Timed Slow Close-range Strike Simulated");
            executeStrike();

            break;

          case FASTTIMEDSTRIKEMODE:
            blinkLight = 6;

            checkTime = millis() + 15 * 1000;
            j_cnt = 3;
            blink4(6);
            digitalWrite(4, LOW);
            Serial.println("Timed Fast Close-range Strike Simulated");
            executeStrike();
            break;

          case RANDOMTIMEDSTRIKEMODE:
            blinkLight = 4;

            long randNumber;
            randNumber = random(30 * 1000, 30 * 60 * 1000);

            checkTime = millis() + randNumber;
            blink4(9);
            j_cnt = 3;
            digitalWrite(4, LOW);
            Serial.println("Timed Random Close-range Strike Simulated");
            executeStrike();
            break;

          default:

            Serial.println("Default hit - error");
            break;

        }

      }


      // blink which light

      if (lastBlink != millis() / 1000)
      {
        digitalWrite(blinkLight, LOW);
        delay(100);
        lastBlink = millis() / 1000;

      }
      else
      {
        digitalWrite(blinkLight, HIGH);
      }


    }


    timedState = SINGLESTRIKEMODE;

    Serial.println("Timed Multi-strike Mode terminated");

    delay(500);

    // wait until clear

    // we don't do squat until all the buttons are clear
    while (!digitalRead(5) || !digitalRead(7) || !digitalRead(9))
    {

    }
  }

}

