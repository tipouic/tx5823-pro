/*

tx5823-pro

Description: The tx5823-pro transmitter module is configured via IR by the
parent project rx5808-pro-diversity.

Transmitter Project:
https://github.com/sheaivey/tx5823-pro

Parent Receiver Project:
https://github.com/sheaivey/rx5808-pro-diversity

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

#include <avr/pgmspace.h>
#include <EEPROM.h>

#include "settings.h"

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <SPI.h>

#include "screens.h"
screens drawScreen;

// Channels to sent to the SPI registers
const uint16_t channelTable[] PROGMEM = {
  // Channel 1 - 8
    0x7981,    0x758D,    0x7199,    0x6DA5,    0x69B1,    0x65BD,    0x6209,    0x5E15, // Band A
    0x5F9D,    0x6338,    0x6713,    0x6AAE,    0x6E89,    0x7224,    0x75BF,    0x799A, // Band B
    0x5A21,    0x562D,    0x5239,    0x4E85,    0x7D35,    0x8129,    0x851D,    0x8911, // Band E
    0x610C,    0x6500,    0x68B4,    0x6CA8,    0x709C,    0x7490,    0x7884,    0x7C38, // Band F / Airwave
    0x510A,    0x5827,    0x5F84,    0x66A1,    0x6DBE,    0x751B,    0x7C38,    0x8395  // Band C / Immersion Raceband
};

// Channels with their Mhz Values
const uint16_t channelFreqTable[] PROGMEM = {
  // Channel 1 - 8
  5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // Band A
  5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // Band B
  5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // Band E
  5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // Band F / Airwave
  5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917  // Band C / Immersion Raceband
};

// do coding as simple hex value to save memory.
const uint8_t channelNames[] PROGMEM = {
  // Channel 1 - 8
  0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, // Band A
  0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, // Band B
  0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, // Band E
  0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, // Band F / Airwave
  0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8  // Band C / Immersion Raceband
};

bool channel_sent = false;
char call_sign[11];
uint8_t channelIndex = 0;
uint8_t state = 255; // force redraw
unsigned long timeout = 0;
unsigned long receive_timeout = 0;
// SETUP ----------------------------------------------------------------------------
void setup()
{
    // IO INIT
    // initialize digital pin 13 LED as an output.
    pinMode(led, OUTPUT); // status pin for TV mode errors
    digitalWrite(led, HIGH);
    // Bind Switch
    pinMode(bindSwitch, INPUT);
    digitalWrite(bindSwitch, LOW);

    // SPI pins for RX control
    pinMode (slaveSelectPin, OUTPUT);
    pinMode (spiDataPin, OUTPUT);
	pinMode (spiClockPin, OUTPUT);

    // use values only of EEprom is not 255 = unsaved
    uint8_t eeprom_check = EEPROM.read(EEPROM_ADR_TUNE);
    if(eeprom_check == 255) // unused
    {
        // save 8 bit
        EEPROM.write(EEPROM_ADR_TUNE,CHANNEL_MIN_INDEX);

        // save default call sign
        strcpy(call_sign, CALL_SIGN); // load callsign
        for(uint8_t i = 0;i<sizeof(call_sign);i++) {
            EEPROM.write(EEPROM_ADR_CALLSIGN+i,call_sign[i]);
        }

    }

    // read last setting from eeprom
    channelIndex=EEPROM.read(EEPROM_ADR_TUNE);

    // load saved call sign
    for(uint8_t i = 0;i<sizeof(call_sign);i++) {
        call_sign[i] = EEPROM.read(EEPROM_ADR_CALLSIGN+i);
    }



    delay(100); // give some time for screen to power on.


    // Init Display
    if (drawScreen.begin(call_sign) > 0) {
        // on Error flicker LED
        while (true) { // stay in ERROR for ever
            digitalWrite(led, !digitalRead(led));
            delay(100);
        }
    }


    // set channel on boot
    channel_sent = true;
    set_5823_freq(channelIndex);
#ifdef USE_BOOT_LOGO
    // display logo for 3 seconds regaurdless of which module being used.
    delay(3000-VTX_POWER_ON_DELAY);
#endif

    Serial.begin(9600);

    // Setup Done - LED OFF
    digitalWrite(led, LOW);
}

// LOOP ----------------------------------------------------------------------------
void loop()
{
    static uint8_t last_state = state;
    bool forceRedraw = false; // force a full screen redraw
    if(digitalRead(bindSwitch) == HIGH) {
        state = (state == STATE_SCREEN_TRANSMITTING || state == 255) ? STATE_BIND_MODE : state;
    }
    else {
        state = STATE_SCREEN_TRANSMITTING;
    }

    if(state != last_state) {
        forceRedraw = true;
    }


    if(state == STATE_SCREEN_TRANSMITTING) {
        digitalWrite(led,(millis() %2000 > 1000)); // blink LED slow in transmit mode
        if(millis() % 500 == 0 || forceRedraw) {
            drawScreen.screenSaver(pgm_read_byte_near(channelNames + channelIndex), pgm_read_word_near(channelFreqTable + channelIndex), call_sign, forceRedraw);
        }
        if(!channel_sent) {
            channel_sent = true;
            delay(150);
            // tell transmitter to be on the correct channel.
            set_5823_freq(channelIndex);
        }
    }

    if(state >= STATE_BIND_MODE && state <= STATE_BIND_MODE_FAILED) {
        channel_sent = false;
        // watch for incoming payload
        if(hasReceivedPayload()) {
            forceRedraw = true;
            state = STATE_BIND_MODE_RECEIVED;
            timeout = millis()+3000;
            digitalWrite(led, HIGH); // Stay solid indecating a payload was received
        }
        if(timeout < millis()) {
            state = STATE_BIND_MODE; // return to flashing bind mode.
            digitalWrite(led, (millis() % 250 > 125)); // blink LED fast in bind mode
        }
        if(millis() % 125 == 0 || forceRedraw) {
            drawScreen.bindMode(state, pgm_read_byte_near(channelNames + channelIndex), pgm_read_word_near(channelFreqTable + channelIndex), call_sign, forceRedraw);
        }
    }
    last_state = state;
}

bool hasReceivedPayload() {
    if(Serial.available() > 0) {
        receive_timeout = millis()+30;
        delay(5);
        // received payload should look something like this ... "STX(byte channel)(char[10] call sign)ETX(byte check sum)"
        uint8_t c;
        bool found_end = false,found_start = false;
        uint8_t i = 0;
        uint8_t check_sum_temp = 0, check_sum = 0;
        char call_sign_temp[11] = "";
        uint8_t channelIndex_temp = 0;
        uint8_t total_bytes_read = 0;
        do{
            if(Serial.available() > 1) { // make sure there is atleast 1 byte to read
                c = Serial.read();
                total_bytes_read++;
                if(c == 2 && !found_start) { // Start of Text STX start of payload
                    found_start = true;
                    check_sum_temp += c;
                    check_sum_temp += channelIndex_temp = Serial.read(); // read channel byte

                    if(channelIndex_temp >= 40 || channelIndex_temp < 0) {
                        break;
                    }
                    total_bytes_read = 2;
                    continue;
                }
                if(c == 3 && found_start) { // End of Text ETX end of payload
                    check_sum = (uint8_t) Serial.read();
                    total_bytes_read++;
                    found_end = true;
                    break;
                }
                if(found_start) { // start reiving call sign chars.
                    check_sum_temp += call_sign_temp[i++] = (char)c;
                }
            }
            else {
                delay(5); // give some time for more bytes to flow in.
            }
        }
        while(millis() < receive_timeout && i <= 10);
        call_sign_temp[i] = '\0';

        receive_timeout = millis()+10;
        while(Serial.available() > 0 && millis() < receive_timeout){Serial.read();Serial.flush();delay(1);}; // clear teh remaining buffer

        if(check_sum == (uint8_t) check_sum_temp && total_bytes_read > 2 && found_start && found_end) { // save valid channelIndex and call_sign
            channelIndex = channelIndex_temp;
            strcpy(call_sign, call_sign_temp);

            // save 8 bit
            EEPROM.write(EEPROM_ADR_TUNE,channelIndex);

            // save call sign
            for(uint8_t i = 0;i<sizeof(call_sign);i++) {
                EEPROM.write(EEPROM_ADR_CALLSIGN+i,call_sign[i]);
            }
            return true; // sucessfull payload
        }
    }
    return false;
}

/*
 *  update the frequency registers on TX5823
 */
void set_5823_freq(uint8_t freq)
{
    uint32_t channelData;

    // some modules need a delay bfore we send the commands
    delay(VTX_POWER_ON_DELAY);

    // read in the channel information from the table, and add 0x00 04 00 00 to it
    channelData = (pgm_read_word_near(channelTable + freq)) + 0x00040000;

    // Setting the R-Counter is not necessary every time but doesn't hurt
    spi_write(0x00, 0x0190);  // default value, provides 40kHz frequency resolution
    spi_write(0x01, channelData);   // write N and A counter-dividers (from channel table)

}


/*
 * execute a complete SPI write operation
 */
void spi_write(uint8_t addr, uint32_t data)
{
    uint32_t output;

    //              FIRST  -  -  -  -  -  -  -  -  -  -  -  LAST
    // Data Format: A0  A1  A2  A3  R/W  D0  D1  D2 ... D18  D19
    output = ((data << 5) | 0x10) + (addr & 0x0f);

    // start SPI transaction
    digitalWrite(spiClockPin, LOW);
    delayMicroseconds(1);
    digitalWrite(slaveSelectPin, LOW);
    delayMicroseconds(1);

    // clock out data LSB first
    for (uint8_t i = 25; i > 0; i--) {
        if (output & 0x01) {
              digitalWrite(spiDataPin, HIGH);  // output 1
        }
        else {
              digitalWrite(spiDataPin, LOW);   // output 0
        }
        delayMicroseconds(1);
        digitalWrite(spiClockPin, HIGH);   // data latched by 5823 here
        delayMicroseconds(1);
        digitalWrite(spiClockPin, LOW);    // clock ends low
        output >>= 1;                      // shift output data
        delayMicroseconds(1);
    }

    // terminate SPI transaction and set data low
    digitalWrite(slaveSelectPin, HIGH);
    delayMicroseconds(1);
    digitalWrite(spiDataPin, LOW);
    delayMicroseconds(1);
}
