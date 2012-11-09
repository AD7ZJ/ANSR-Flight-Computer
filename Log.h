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
 * Filename:     Log.h                                                     *
 *                                                                         *
 ***************************************************************************/

#ifndef LOG_H
#define LOG_H

#include "SDLogger.h"
/**
 *  @defgroup application APRS Beacon Application
 *
 *  @{
 */

/**
 *  Manange the flash based data logger.
 */
class Log
{
public:
    Log();
    
    void Dump();
    void Enable();
    void Erase();
    void GPSFix (const GPSData *gps);
    void SystemBooted();
    void TimeStamp (const GPSData *gps);
    void NavLaunch();
    static Log *GetInstance();
    bool_t UpdateWindTable();
    bool_t PredictLanding(GPSData * landingPrediction);

    /// true when burst has been detected
    bool_t burstDetect;

private:
    SDLogger windLogger;

    typedef struct {
        /// lat and long, can be either degrees or radians
        float lat, lon;
        /// altitude in ft
        int32_t alt;
    } COORD;

    typedef struct {
        float dist, head, trackError;
    } COURSE;

    typedef struct {
        uint32_t timeStamp;
        uint16_t timeInterval;
        COURSE course;
        COORD coord;
    } NAV_WINDDATA;

    /// Predicted landing coordinate
    COORD landingZone;

    /// Number of bytes to buffer before writing to flash memory.
    static const uint32_t WriteBufferSize = 40;

    static const float PI = 3.14159;

    /// How much altitude difference in ft before we log another wind speed calculation
    static const uint16_t NAV_INTERVAL = 500;

    /// how many wind speed entries are allowed
    static const uint16_t NAV_BLOCKCOUNT = 200;

    /// array containing the
    NAV_WINDDATA windData[NAV_BLOCKCOUNT];

    GPSData * gps;

    /// Enumerated type that describes the type of record.
    typedef enum
    {
        /// System boot indicator.
        SYSTEM_BOOTED = 0x87,
        
        /// GPS fix record.
        GPS_FIX = 0x96,
        
        /// Time stamp of first 3D fix.
        TIME_STAMP = 0xa5
        
    } LOG_RECORD_TYPE;
    
    void Flush();
    void Type (LOG_RECORD_TYPE type);
    void WriteUint8 (uint8_t data);
    void WriteUint16 (uint16_t data);
    void WriteUint32 (uint32_t data);
    void WriteBlock (const uint8_t *data, uint32_t length);

    void NavCourse (COORD *coord1, COORD *coord2, COURSE *course);
    void NavRadToDeg (COORD *coord);
    void NavSetDegFCoord (float lat, float lon, COORD *coord);
    void NavDistRadial (COORD *current, COORD *next, float d, float tc);
    float NavDescentRate (float alt);
    
    /// Current index into log buffer.
    uint32_t index;
    
    /// Temporary buffer that holds data before it is written to flash device.
    uint8_t buffer[WriteBufferSize];
    
    /// Maximum altitude in flight
    int32_t maxAltitude;
};

/** @} */

#endif  // #ifndef LOG_H
