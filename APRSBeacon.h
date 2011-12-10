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
 * Filename:     APRSBeacon.h                                              *
 *                                                                         *
 ***************************************************************************/

#ifndef APRSBEACON_H
#define APRSBEACON_H

/**
 *  @defgroup application APRS Beacon Application
 *
 *  @{
 */

/**
 *   HF-APRS main application class.
 */
class APRSBeacon
{
public:
    APRSBeacon();
    
    void Run();

    static APRSBeacon *GetInstance();
    
private:
    void ScheduleMessage();
    void StatusPacket(const GPSData *gps, char *text);
    //FAT16 * flash;
    //FAT16::fat16_file_struct * handle;
    
    /// Pointer to GPS engine control object.
    GPSLassen *gps;
    
    /// Pointer to the engineering control object.
    Engineering *eng;
    
    /// Pointer to AX.25 message generation object.
    AFSK *afsk;
    
    /// Peak payload altitude in feet.
    int32_t peakAltitude;
    
    /// Time since Epoch the GPS first reported a 3D fix.
    uint32_t startGPSTime;
    
    /// System timer value to turn off GPS status tick LED.
    uint32_t statusLEDOffTick;

    uint32_t millisecondTimer;
};

/** @} */

#endif  // #ifndef APRSBEACON_H
