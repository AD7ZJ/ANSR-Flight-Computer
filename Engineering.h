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
 * Filename:     Engineering.h                                             *
 *                                                                         *
 ***************************************************************************/

#ifndef ENGINEERING_H
#define ENGINEERING_H

//FIXME: remove once this is rolled into the armlib
#include "GPSNmea.h"

/**
 *  @defgroup application APRS Beacon Application
 *
 *  @{
 */

/**
 * Provides a simple text based menu and command system for engineering integration
 * and test of the hardware.
 */
class Engineering
{
public:
    Engineering();

    void Enable();
    void ProcessCommand();

    static Engineering *GetInstance();

private:
    void MICEncoderTestCase (GPSData *gps, uint32_t testCase);

    /// Pointer to control port object.
    UART0 *uart;

    /// Pointer to GPS engine control object.
    GPSNmea *gps;

    /// Flag that indicates the radio is powered on.
    bool_t radioPower;

    /// Flag that indicates the PTT (Push To Transmit) is active.
    bool_t radioPTT;

    /// Flag that indicates the primary or secondary frequency are selected.
    bool_t radioFreq;

    /// Flag that indicates the status of port1
    bool_t port1;

    /// Flag that indicates the status of port2
    bool_t port2;

    /// Flag that indicates the status of audio switch 1
    bool_t audiosw1;

    /// Flag that indicates the status of audio switch 2
    bool_t audiosw2;

    /// Flag that indicates the status of audio switch 3
    bool_t audiosw3;

    void Menu();
};

/** @} */



#endif  // #ifndef ENGINEERING_H
