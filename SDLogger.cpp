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
 * Filename:     SDLogger.cpp                                              *
 *                                                                         *
 ***************************************************************************/

#include "main.h"

/*
 * Constructor
 */
SDLogger::SDLogger() {
	// mount the drive

}

// Allocate the static member variables
uint8_t SDLogger::logicalDrive = 0;

/**
 * Sets up the SD card
 *
 * @param fileName Name of the file to open
 * @param mode Which mode to open the file in.  See http://elm-chan.org/fsw/ff/en/open.html for the different mode options
 *
 * @return true if successful, false otherwise
 */
bool_t SDLogger::Enable(const char * fileName, uint8_t mode) {
    if(f_mount(logicalDrive++, &Fatfs) != FR_OK)
        UART0::GetInstance()->WriteLine("Failed to mount SD card");

    res = f_open(&File, fileName, mode);

    if(res != FR_OK) {
        UART0::GetInstance()->WriteLine("Failed to open file on drive 0");
        return false;
    }
    else
        return true;
}

/**
 * Appends data onto the end of the open file
 *
 * @param buffer Pointer to an array of bytes to be written at the end of the file
 * @param byteCount Number of bytes in the array to write
 *
 * @return true if successful, false otherwise
 */
bool_t SDLogger::Append(void * buffer, UINT byteCount) {
    res = f_lseek(&File, f_size(&File));
    res = f_write(&File, buffer, byteCount, &bytesWritten);

    if(res != FR_OK)
        return false;
    else
        return true;
}

/**
 * Forces any data in memory to be written to the disk
 *
 * @return true if successful, false otherwise
 */
bool_t SDLogger::SyncDisk() {
    res = f_sync(&File);
    if(res == FR_OK)
        return true;
    return false;
}

/**
 * Sets the SD card I/O lines high
 */
void SDLogger::LineTestHigh() {
    // set the pins to GPIO
    PINSEL1 &= ~(0x0F << 2);
    // set the pins as outputs
    FIO0DIR |= (1 << 17) | (1 << 18) | (1 << 19) | (1 << 20);
    FIO0SET |= (1 << 17) | (1 << 18) | (1 << 19) | (1 << 20);
}

/**
 * Sets the SD card I/O lines low
 */
void SDLogger::LineTestLow() {
    FIO0CLR |= (1 << 17) | (1 << 18) | (1 << 19) | (1 << 20);
}
