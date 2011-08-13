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
 * Filename:     Log.h                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef LOG_H
#define LOG_H

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
    
    static Log *GetInstance();        
    
private:
    /// Number of bytes to buffer before writing to flash memory.
    static const uint32_t WriteBufferSize = 40;

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
    
    /// Last used address in flash memory.
    uint32_t address;
    
    /// Current index into log buffer.
    uint32_t index;
    
    /// Temporary buffer that holds data before it is written to flash device.
    uint8_t buffer[WriteBufferSize];
    
    /// Object to control flash memory chip.
    M25P80 *m25p80;
};

/** @} */

#endif  // #ifndef LOG_H
