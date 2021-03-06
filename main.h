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
 *               (c) Copyright, 2011, ANSR                                 *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 * Filename:     main.h                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef MAIN_H
#define MAIN_H

/** 
 * @mainpage Flight Computer Application
 *
 * @section overview_sec Overview
 *
 * Flight Computer Balloon Tracking application.
 *
 * @section copyright Copyright
 *
 *   <b>(c) Copyright, 2001-2012, ANSR</b><br>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.<br>
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.<br>
 *
 */

#include "armlib.h"
#include "AFSK.h"
#include "Log.h"
#include "Engineering.h"
#include "FlightComputer.h"
#include "IOPorts.h"
#include "SDLogger.h"
#include "GPSNmea.h"
#include "ToneGenerator.h"
#include "Repeater.h"

#include <string.h>
#include <stdint.h>

extern "C" {
	#include "fatfs/ff.h"
	#include "fatfs/diskio.h"
}

#endif  // #ifndef MAIN_H


