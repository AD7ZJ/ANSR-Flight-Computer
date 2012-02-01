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

bool_t SDLogger::Enable(const char * fileName, uint8_t mode) {
	if(f_mount(0, &Fatfs) != FR_OK)
	    	UART0::GetInstance()->WriteLine("Failed to mount SD card");

	DSTATUS stat = disk_initialize(0);
	res = f_open(&File, fileName, mode);
	if(res != FR_OK) {
		UART0::GetInstance()->WriteLine("Failed to open file on drive 0");
		return false;
	}
	else
		return true;

}

bool_t SDLogger::Append(char * buffer, UINT byteCount) {
	res = f_lseek(&File, f_size(&File));
	res = f_write(&File, buffer, byteCount, &bytesWritten);

	if(res != FR_OK)
		return false;
	else
		return true;
}

bool_t SDLogger::fSync() {
	res = f_sync(&File);
	if(res == FR_OK)
		return true;
	return false;
}

