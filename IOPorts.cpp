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
 *               (c) Copyright, 2001-2008, Michael Gray                    *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 * Filename:     IOPorts.cpp                                               *
 *                                                                         *
 ***************************************************************************/

#include "main.h"

/// Port 0, Bit 10 - Radio PTT (Push To Transmit).
const uint32_t IO_P0_RADIO_PTT = (1 << 10);

/// Port 0, Bit 16 - Test Point 2.
const uint32_t IO_P0_TEST_POINT_2 = (1 << 16);

/// Port 0, Bit 21 - Digital Output 2.
const uint32_t IO_P1_OUT2 = (1 << 21);

/// Port 0, Bit 22 - Digital Output 3.
const uint32_t IO_P1_OUT3 = (1 << 22);

/// Port 0, Bit 28 - Red chip in bi-color LED.
const uint32_t IO_P0_LED_RED = (1 << 28);

/// Port 0, Bit 29 - Green chip in bi-color LED.
const uint32_t IO_P0_LED_GREEN = (1 << 29);

/// Port 0, Bit 30 - Test Point 3.
const uint32_t IO_P0_TEST_POINT_3 = (1 << 30);

/// Port 1, Bit 16 - Radio power control line.
const uint32_t IO_P1_RADIO_PWR = (1 << 16);

/// Port 1, Bit 19 - Digital Output 1.
const uint32_t IO_P1_OUT1 = (1 << 19);

/// Port 1, Bit 24 - Radio frequency selection control line.
const uint32_t IO_P1_RADIO_FREQ = (1 << 24);


/**
 * Configure the GPIO ports as required for analog and digital I/O.
 */
void IOPorts::Enable()
{
    // Enable high speed GPIO on ports 0 and 1.
    SCS = 0x03;

    FIO0CLR = IO_P0_RADIO_PTT | IO_P0_LED_RED | IO_P0_LED_GREEN | IO_P0_TEST_POINT_2 | IO_P0_TEST_POINT_3;
    FIO0DIR |= (IO_P0_RADIO_PTT | IO_P0_LED_RED | IO_P0_LED_GREEN | IO_P0_TEST_POINT_2 | IO_P0_TEST_POINT_3);

    FIO1CLR = IO_P1_RADIO_PWR | IO_P1_OUT1 | IO_P1_RADIO_FREQ;
    FIO1DIR |= (IO_P1_RADIO_PWR | IO_P1_OUT1 | IO_P1_RADIO_FREQ);
}

/**
 * Set the control port 1 open drain output.
 *
 * @param state true to ground output; otherwise false
 */
void IOPorts::Port1(bool_t state)
{
    if (state)
        FIO1SET = IO_P1_OUT1;
    else
        FIO1CLR = IO_P1_OUT1;
}

/**
 * Set the control port 2 open drain output.
 *
 * @param state true to ground output; otherwise false
 */
void IOPorts::Port2(bool_t state)
{
    if (state)
        FIO1SET = IO_P1_OUT2;
    else
        FIO1CLR = IO_P1_OUT2;
}

/**
 * Set the control port 3 open drain output.
 *
 * @param state true to ground output; otherwise false
 */
void IOPorts::Port3(bool_t state)
{
    if (state)
        FIO1SET = IO_P1_OUT3;
    else
        FIO1CLR = IO_P1_OUT3;
}

/**
 * Set the state of the radio power control terminal 2 on the Radio connector.
 *
 * @param state true to enable power; otherwise false
 */
void IOPorts::RadioPower (bool_t state)
{
    if (state)
        FIO1SET = IO_P1_RADIO_PWR;
    else
        FIO1CLR = IO_P1_RADIO_PWR;
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
 * Set the state of the radio frequency selection terminal 5 on the Radio connector.
 *
 * @param state true to pull LOW; otherwise false
 */
void IOPorts::RadioFreq (bool_t state)
{
    if (state)
        FIO1SET = IO_P1_RADIO_FREQ;
    else
        FIO1CLR = IO_P1_RADIO_FREQ;
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

/**
 * Set the state of test point 2.
 *
 * @param state true to set HIGH; otherwise false
 */
void IOPorts::TestPoint2 (bool_t state)
{
    if (state)
        FIO0SET = IO_P0_TEST_POINT_2;
    else
        FIO0CLR = IO_P0_TEST_POINT_2;
}


