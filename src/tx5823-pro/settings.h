/*
 * Setings file by Shea Ivey

The MIT License (MIT)

Copyright (c) 2015 Shea Ivey

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef settings_h
#define settings_h


//#define USE_FLIP_SCREEN
#define USE_BOOT_LOGO


// Choose which vTX module you are using.
// see below for details of why this is necessary
#define TX5823
//#define TX5823N
//#define FX758

// this will be displayed on the screensaver.
// Up to 10 letters
#define CALL_SIGN "CALL SIGN"

#define spiDataPin 10
#define slaveSelectPin 11
#define spiClockPin 12

#define bindSwitch 3
#define led 13

#define CHANNEL_BAND_SIZE 8
#define CHANNEL_MIN_INDEX 0
#define CHANNEL_MAX_INDEX 39

#define CHANNEL_MAX 39
#define CHANNEL_MIN 0

#define EEPROM_ADR_TUNE 0

#define EEPROM_ADR_CALLSIGN 20


#define STATE_SCREEN_TRANSMITTING 0
#define STATE_BIND_MODE 1
#define STATE_BIND_MODE_WAITING 2
#define STATE_BIND_MODE_RECEIVED 3
#define STATE_BIND_MODE_FAILED 4


#ifdef TX5823
    // Year 2012
    // 3.3v
    // SPI Mod Required
    // the TX5823-120228 needs atleast 1.5 seconds before sending the channel via SPI
    #define VTX_POWER_ON_DELAY 1500
#endif
#ifdef TX5823N
    // Year 2015
    // 3.3v
    // SPI Mod Required
    // the TX5823N_150116 needs atleast 0.1 seconds before sending the channel via SPI
    #define VTX_POWER_ON_DELAY 100
#endif
#ifdef FX758
    // Year 2014
    // 5.5v
    // SPI Mod NOT Required
    // the FX758-141125 needs atleast 1.5 seconds before sending the channel via SPI
    #define VTX_POWER_ON_DELAY 100
#endif

#endif // file_defined
