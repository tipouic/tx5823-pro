/*
 * Screens Class by Shea Ivey

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

#ifndef display_h
#define display_h


#include <avr/pgmspace.h>

class screens
{
    private: // helper functions for screens.
        void reset();

    public:
        screens();
        char begin(const char *call_sign);
        void flip();

        void updateFrequencyInformation(uint8_t channelName, uint16_t channelFrequency, const char *call_sign);

        void updateStatus(const char *status);
        void updateStatus(const char *status, bool invert);
        void updateStatus(const char *status, bool invert, bool blink);

        // BIND MODE
        void bindMode(uint8_t state, uint8_t channelName, uint16_t channelFrequency, const char *call_sign, bool force_redraw);

        // SCREEN SAVER
        void screenSaver(uint8_t channelName, uint16_t channelFrequency, const char *call_sign, bool force_redraw);
        void alternateScreenSaver(uint8_t channelName, uint16_t channelFrequency, const char *call_sign);


};
#endif
