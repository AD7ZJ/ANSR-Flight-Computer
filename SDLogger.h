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
 *               (c) Copyright, 2011, Elijah Brown                         *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 * Filename:     SDLogger.h                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef SDLOGGER_H
#define SDLOGGER_H

// ChaN FatFS includes. Move to armlib.h when this module rolled into the armlib
extern "C" {
	#include "fatfs/ff.h"
	#include "fatfs/diskio.h"
}

/**
 *  @defgroup library Generic Library Functions
 *
 *  @{
 */

/**
 * Provides basic logging functions to work with the SD card
 */
class SDLogger
{
public:
	SDLogger();

    void Enable(const char * fileName, uint8_t mode);
    void Write();
    void Read();
    void Append(char * buffer, UINT byteCount);

private:
    void MICEncoderTestCase (GPSData *gps, uint32_t testCase);

    /// File system object
    FATFS Fatfs;

    /// File object
    FIL File;

    /// Enumerated type defining the different file system operation results
    FRESULT res;

    /// Number of bytes successfully written or received
    UINT numBytes;


    char buffer[512];
    UINT bytesWritten;
};

/** @} */


#endif  // #ifndef SDLOGGER_H
