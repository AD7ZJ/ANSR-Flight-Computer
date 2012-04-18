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
 * Filename:     Log.cpp                                                   *
 *                                                                         *
 ***************************************************************************/

#include "main.h"

/// Reserve memory for singleton object.
static Log logSingletonObject;

/**
 *  Get a pointer to the Radio waveform generation object.
 */
Log *Log::GetInstance()
{
    return  &logSingletonObject;
}

/**
 * Constructor.
 */
Log::Log()
{
    this->m25p80 = M25P80::GetInstance();
    this->address = 0;
}

/**
 * Log the system booted event.
 */
void Log::SystemBooted()
{
    Type (SYSTEM_BOOTED);
    Flush();
}

/**
 * Log the current GPS fix data set.
 * 
 * @param gps pointer to GPSData object
 */
void Log::GPSFix (const GPSData *gps)
{
    Type (GPS_FIX);
    
    WriteUint8 (gps->hours);
    WriteUint8 (gps->minutes);
    WriteUint8 (gps->seconds);
    
    WriteBlock (gps->rawLatitude, sizeof(gps->rawLatitude));
    WriteBlock (gps->rawLongitude, sizeof(gps->rawLongitude));
    WriteBlock (gps->rawAltitude, sizeof(gps->rawAltitude));

    WriteBlock (gps->rawSpeedHeading, sizeof(gps->rawSpeedHeading));
    WriteUint16 (gps->navType);
    WriteUint8 (gps->dop);

    WriteUint8 (gps->trackedSats);
    
    Flush();    
}

/**
 * Log the system time and date.
 */
void Log::TimeStamp (const GPSData *gps)
{
    Type (TIME_STAMP); 
    
    WriteUint8 (gps->month);
    WriteUint8 (gps->day);
    WriteUint16 (gps->year);  
    
    WriteUint8 (gps->hours);
    WriteUint8 (gps->minutes);
    WriteUint8 (gps->seconds);    
}

/**
 * Write an 8-bit value to flash.
 * 
 * @param data 8-bit data value
 */
void Log::WriteUint8 (uint8_t data)
{
    this->buffer[this->index++] = data;
}

/**
 * Write an 16-bit value to flash.
 * 
 * @param data 16-bit data value
 */
void Log::WriteUint16 (uint16_t data)
{
    this->buffer[this->index++] = static_cast <uint8_t> ((data >> 8) & 0xff);
    this->buffer[this->index++] = static_cast <uint8_t> (data & 0xff);    
}

/**
 * Write an 32-bit value to flash.
 * 
 * @param data 32-bit data value
 */
void Log::WriteUint32 (uint32_t data)
{
    this->buffer[this->index++] = static_cast <uint8_t> ((data >> 24) & 0xff);
    this->buffer[this->index++] = static_cast <uint8_t> ((data >> 16) & 0xff);
    this->buffer[this->index++] = static_cast <uint8_t> ((data >> 8) & 0xff);
    this->buffer[this->index++] = static_cast <uint8_t> (data & 0xff);    
}

/**
 * Write a binary block of data to flash.
 * 
 * @param data pointer to data block
 * @param length number of bytes to write
 */
void Log::WriteBlock (const uint8_t *data, uint32_t length)
{
    uint32_t i;
    
    for (i = 0; i < length; ++i)
        this->buffer[this->index++] = static_cast <uint8_t> (data[i]);    
}

/**
 * Send the contents of the log to the serial port.
 */
void Log::Dump()
{
    uint32_t i, zeroCount, address;
    bool_t doneFlag;
    uint8_t data[24];
    char buffer[10];
    
    address = 0;
    doneFlag = false;
    zeroCount = 0;
    
    UART0::GetInstance()->WriteLine ("Log Contents");
    
    while (!doneFlag)
    {
        this->m25p80->ReadBlock (address, data, sizeof(data));
        
        sprintf (buffer, "%08lx ", address);
        UART0::GetInstance()->Write (buffer);
        
        for (i = 0; i < sizeof(data); ++i)
        {
            sprintf (buffer, "%02x ", data[i]);
            UART0::GetInstance()->Write (buffer);
            
            if (data[i] == 0xff)
                ++zeroCount;
            else
                zeroCount = 0;
        } // END for
        
        UART0::GetInstance()->WriteLine ("");
        
        if (UART0::GetInstance()->IsCharReady())
            doneFlag = true;
        
        if (zeroCount > 32)
            doneFlag = true;
        
        address += sizeof(data);
        
    } // END while    
}

/**
 *   Write the contents of the temporary log buffer to the flash device.  If the buffer
 *   is empty, nothing is done.
 */
void Log::Flush()
{
    // We only need to write if there is data.
    if (this->index != 0) 
    {
        this->m25p80->WriteBlock (this->address, this->buffer, this->index);
        this->address += this->index;
        this->index = 0;
    } // END if
}

/**
 *   Start a entry in the data log.
 *
 *   @param type enumeration that describes the type of log entry
 */
void Log::Type (LOG_RECORD_TYPE type)
{
    // Only add the new entry if there is space.
    if (this->address >= 0x100000)
        return;

    // Write the old entry first.
    Flush();

    // Save the type and set the log buffer pointer.
    this->buffer[0] = type;
    this->index = 1;
}

/**
 * Erase the contents of the flash memory log.
 */
void Log::Erase()
{
    this->m25p80->BulkErase();
    
    // Set the write address back to the start.
    this->address = 0x0000;
}

/**
 * Enable the logging system for operation.
 */
void Log::Enable()
{
    uint8_t buffer[8];
    bool_t endFound;

    // Parse through the log to find the end of memory.
    this->address = 0x0000;
    endFound = false;
    
    // Read each logged data block from flash to determine how long it is.
    do 
    {
        // Read the data log entry type.
        this->m25p80->ReadBlock (this->address, buffer, 1);

        // Based on the log entry type, we'll skip over the data contained in the entry.
        switch (buffer[0]) 
        {
            case SYSTEM_BOOTED:
                this->address += 1;
                break;
    
            case GPS_FIX:
                this->address += 24;
                break;
    
            case TIME_STAMP:
                this->address += 7;
                break;

            case 0xff:
                endFound = true;
                break;

            default:
                ++this->address;
        } // END switch
    } while (this->address < 0x100000 && !endFound);

    this->index = 0;    
}
