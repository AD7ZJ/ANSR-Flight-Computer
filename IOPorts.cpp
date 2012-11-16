/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the Free Software            *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111 USA    *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 *               (c) Copyright, 2001-2012, ANSR                            *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 * Filename:     IOPorts.cpp                                               *
 *                                                                         *
 ***************************************************************************/

#include "main.h"

/// Port 1, Bit 22 - audio switch 1.
const uint32_t IO_P1_AUDSW1 = (1 << 22);

/// Port 1, Bit 21 - audio switch 2.
const uint32_t IO_P1_AUDSW2 = (1 << 21);

/// Port 1, Bit 20 - audio switch 3.
const uint32_t IO_P1_AUDSW3 = (1 << 20);

/// Port 0, Bit 11 - Radio PTT (Push To Transmit).
const uint32_t IO_P0_RADIO_PTT = (1 << 11);

/// Port 1, Bit 24 - Digital output (open drain 1)
const uint32_t IO_P1_OD_1 = (1 << 24);

/// Port 0, Bit 10 - Digital output (open drain 2)
const uint32_t IO_P0_OD_2 = (1 << 10);

/// Port 1, Bit 23 - Radio frequency selection control line.
const uint32_t IO_P1_FREQCHG = (1 << 23);

/// Port 0, Bit 28 - Red chip in bi-color LED.
const uint32_t IO_P0_LED_RED = (1 << 28);

/// Port 0, Bit 29 - Green chip in bi-color LED.
const uint32_t IO_P0_LED_GREEN = (1 << 29);

/// Port 0, Bit 16 - carrier det in
const uint32_t IO_P0_CARRIERDET = (1 << 16);




/**
 * Configure the GPIO ports as required for analog and digital I/O.
 */
void IOPorts::Enable()
{
    // Enable high speed GPIO on ports 0 and 1.
    SCS = 0x03;

    FIO0CLR = IO_P0_RADIO_PTT | IO_P0_LED_RED | IO_P0_LED_GREEN | IO_P0_OD_2;

    // specify input pins on P0
    FIO0DIR |= (IO_P0_RADIO_PTT | IO_P0_LED_RED | IO_P0_LED_GREEN | IO_P0_OD_2 );
    // specify output pins on P0
    FIO0DIR &= ~(IO_P0_CARRIERDET);

    FIO1CLR = IO_P1_AUDSW1 | IO_P1_AUDSW2 | IO_P1_AUDSW3 | IO_P1_OD_1 | IO_P1_FREQCHG;
    // specify input pins on P1
    FIO1DIR |= (IO_P1_AUDSW1 | IO_P1_AUDSW2 | IO_P1_AUDSW3 | IO_P1_OD_1 | IO_P1_FREQCHG);
}

/**
 * Set the control port 1 open drain output.
 *
 * @param state true to ground output; otherwise false
 */
void IOPorts::Port1(bool_t state)
{
    if (state)
        FIO1SET = IO_P1_OD_1;
    else
        FIO1CLR = IO_P1_OD_1;
}

/**
 * Set the control port 2 open drain output.
 *
 * @param state true to ground output; otherwise false
 */
void IOPorts::Port2(bool_t state)
{
    if (state)
        FIO1SET = IO_P0_OD_2;
    else
        FIO1CLR = IO_P0_OD_2;
}

/**
 * Set audio switch 1
 *
 * @param state true to turn on, false to turn off
 */
void IOPorts::AudioSW1(bool_t state)
{
    if (state)
        FIO1SET = IO_P1_AUDSW1;
    else
        FIO1CLR = IO_P1_AUDSW1;
}

/**
 * Set audio switch 2
 *
 * @param state true to turn on, false to turn off
 */
void IOPorts::AudioSW2(bool_t state)
{
    if (state)
        FIO1SET = IO_P1_AUDSW2;
    else
        FIO1CLR = IO_P1_AUDSW2;
}

/**
 * Set audio switch 3
 *
 * @param state true to turn on, false to turn off
 */
void IOPorts::AudioSW3(bool_t state)
{
    if (state)
        FIO1SET = IO_P1_AUDSW3;
    else
        FIO1CLR = IO_P1_AUDSW3;
}

/**
 * Set the state of the radio PTT (Push-To-Transmit) terminal 6 on the Radio connector.
 *
 * @param state true to pull LOW; otherwise false
 */
void IOPorts::RadioPTT (bool_t state)
{
    if (state)
        FIO0SET = IO_P0_RADIO_PTT;
    else
        FIO0CLR = IO_P0_RADIO_PTT;
}

/**
 * Set the state of the radio frequency selection on the Radio connector.
 *
 * @param state true to pull LOW; otherwise false
 */
void IOPorts::RadioFreq (bool_t state)
{
    if (state)
        FIO1SET = IO_P1_FREQCHG;
    else
        FIO1CLR = IO_P1_FREQCHG;
}

/**
 * Set the Red/Green status LED.
 *
 * @param led enumerated type of the desired LED to control
 * @param state true to turn the LED on; otherwise false
 */
void IOPorts::StatusLED (STATUS_LED led, bool_t state)
{
    if (!state)
    {
        switch (led)
        {
            case LEDRed:
                FIO0SET = IO_P0_LED_RED;
                break;

            case LEDGreen:
                FIO0SET = IO_P0_LED_GREEN;
                break;

            case LEDYellow:
                FIO0SET = IO_P0_LED_RED | IO_P0_LED_GREEN;
                break;
        } // END switch
    } else {
        switch (led)
        {
            case LEDRed:
                FIO0CLR = IO_P0_LED_RED;
                break;

            case LEDGreen:
                FIO0CLR = IO_P0_LED_GREEN;
                break;

            case LEDYellow:
                FIO0CLR = IO_P0_LED_RED | IO_P0_LED_GREEN;
                break;
        } // END switch
    }
}

bool_t IOPorts::GetCarrierDet()
{
    return (FIO0PIN & IO_P0_CARRIERDET) ? true : false;
}



