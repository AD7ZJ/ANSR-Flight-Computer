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
 * Filename:     IOPorts.h                                                 *
 *                                                                         *
 ***************************************************************************/

#ifndef IOPORTS_H
#define IOPORTS_H

/**
 *  @defgroup application APRS Beacon Application
 *
 *  @{
 */

/**
 *   Manage the discrete I/O control ports.
 */
class IOPorts
{
public:
    /// Bi-color status LED.
    typedef enum
    {
        /// Turn on only the red LED.
        LEDRed,
        
        /// TUrn on only the green LED.
        LEDGreen,
        
        /// Turn on the red and green LEDs to generate a yellow color.
        LEDYellow
    } STATUS_LED;

    static void Enable();
    static void Port1(bool_t state);
    static void Port2(bool_t state);
    static void AudioSW1(bool_t state);
    static void AudioSW2(bool_t state);
    static void AudioSW3(bool_t state);
    static void RadioFreq (bool_t state);
    static void RadioPTT (bool_t state);
    static void StatusLED (STATUS_LED led, bool_t state);
    static bool_t GetCarrierDet();
};

/** @} */

#endif  // #ifndef IOPORTS_H
