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
 * Filename:     FlightComputer.h                                              *
 *                                                                         *
 ***************************************************************************/

#ifndef APRSBEACON_H
#define APRSBEACON_H

//FIXME: remove once this is rolled into the armlib
#include "GPSNmea.h"
#include "ToneGenerator.h"
/**
 *  @defgroup application Flight Computer Application
 *
 *  @{
 */

/**
 *   Flight Computer main application class.
 */
class FlightComputer
{
public:
    FlightComputer();
    
    void Run();

    static FlightComputer *GetInstance();
    
    ToneGenerator testTone;

    int16_t AscentRate();

    /// variables used to count the elapsed time
    volatile uint16_t msElapsed;
    volatile uint16_t ms100Elapsed;
    volatile uint16_t sElapsed;

    /// flag indicating 100ms is up
    volatile bool_t timer100msFlag;

    /// flag indicating 1s is up
    volatile bool_t timer1sFlag;

    /// flag indicating 10s is up
    volatile bool_t timer10sFlag;

    /// GPDData object indicating the predicted landing position
    GPSData landingPrediction;

private:
    void ScheduleMessage();
    void SystemTimerTick();
    void StatusPacket(const GPSData *gps, char *text);
    //FAT16 * flash;
    //FAT16::fat16_file_struct * handle;
    
    /// Pointer to GPS engine control object.
    GPSNmea *gps;
    
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

    int32_t altitudeList_[10];
};

/** @} */

#endif  // #ifndef APRSBEACON_H
