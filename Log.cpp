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
 *  Get a pointer to the system logging object.
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
    this->gps = GPSNmea::GetInstance()->Data();
    this->burstDetect = false;
}

/**
 * Log the system booted event.
 */
void Log::SystemBooted()
{
    //FIXME: Log a 'system booted' message to the syslog on the SD card here
}

/**
 * Log the current GPS fix data set.
 * 
 * @param gps pointer to GPSData object
 */
void Log::GPSFix (const GPSData *gps)
{
    //FIXME: Log a GGA and RMC string to the SD card here
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
 * Send the contents of the wind data log to the serial port
 */
void Log::Dump()
{
    UART0::GetInstance()->WriteLine ("Wind Log Contents");
    
    // Find the last entry in the table.
    for (int i = 0; i < NAV_BLOCKCOUNT && windData[i].coord.alt != 0; i++) {
        UART0::GetInstance()->WriteLine("alt:%l dist:%f heading:%f", windData[i].coord.alt, windData[i].course.dist, windData[i].course.head);
    }
}

/**
 *   Write the contents of the wind data field out to the SD card
 */
void Log::Flush()
{
    // We only need to write if there is data.
    if (this->index != 0) 
    {
        windLogger.WriteToFile((void *)windData, sizeof(windData));
    } // END if
}

/**
 * Erase the contents of the wind data table
 */
void Log::Erase()
{
    // zero all the entries in the wind data table
    for (int i = 0; i < NAV_BLOCKCOUNT; i++) {
        windData[i].timeStamp = 0;
        windData[i].timeInterval = 0;

        windData[i].course.dist = 0;
        windData[i].course.head = 0;
        windData[i].course.trackError = 0;

        windData[i].coord.lat = 0;
        windData[i].coord.lon = 0;
        windData[i].coord.alt = 0;
    } // END for

    maxAltitude = 0;
    burstDetect = false;
}

/**
 * Enable the logging system for operation.
 */
void Log::Enable()
{
    windLogger.Enable("wind.bin", FA_OPEN_ALWAYS | FA_WRITE);
    
    // Read the archived wind data into the array
    windLogger.ReadFile((void *)windData);

    this->index = 0;    
}

/**
 * Called once per second to update the wind data table.  After burst, call
 * to update the landing prediction.
 *
 * @return True if successful computing a prediction, false otherwise
 */
bool_t Log::UpdateWindTable() {
	uint16_t i;
    int32_t alt;
    float segmentRate, distance;
    COORD positionEstimate, next;

    // Track the maximum altitude.
    if (gps->AltitudeFeet() > maxAltitude)
        maxAltitude = gps->AltitudeFeet();

    // Calculate the landing after burst.
    if (burstDetect) {
		alt = gps->AltitudeFeet();

        // Find the last entry in the table.
        for (i = 0; i < NAV_BLOCKCOUNT && alt > windData[i].coord.alt; ++i);

        if (i < 2)
            return false;

        --i;
        
		// start out at our current location
        NavSetDegFCoord (((float)gps->latitude / 10000000), ((float)gps->longitude / 10000000), &positionEstimate);

        while (i != 0) {
            segmentRate = ((float) (alt - windData[i].coord.alt)) / NavDescentRate((float) windData[i].coord.alt);
            distance = windData[i].course.dist * segmentRate / (float) windData[i].timeInterval;

            NavDistRadial (&positionEstimate, &next, distance, windData[i].course.head);
            positionEstimate = next;

            alt = windData[i].coord.alt;
            --i;
        }

        this->landingZone = positionEstimate;

        return true;
    } // END if

    // Set a flag to indicate we have a burst condition.
    if (gps->AltitudeFeet() + 500 < maxAltitude) {
        burstDetect = true;
        return true;
    }


    // Find the last entry in the table.
    for (i = 0; i < NAV_BLOCKCOUNT && windData[i].coord.alt != 0; ++i);

    if (i == NAV_BLOCKCOUNT)
        return false;

    this->index = i;

    // Calculate the wind speed for this altitude interval.
    if (gps->AltitudeFeet() > windData[i - 1].coord.alt + NAV_INTERVAL) {
        // Save the current position as the end point for this segment.
        NavSetDegFCoord(((float)gps->latitude / 10000000), ((float)gps->longitude / 10000000), &windData[i].coord);
        windData[i].coord.alt = gps->AltitudeFeet();
        windData[i].timeStamp = gps->hours * 3600 + gps->minutes * 60 + gps->seconds;

        // Calcualte the course and heading from the last element in this segment.
        NavCourse (&windData[i - 1].coord, &windData[i].coord, &windData[i].course);

        // Calculate the time interval for this segment.
        windData[i].timeInterval = (uint16_t) (windData[i].timeStamp - windData[i - 1].timeStamp);

        // save the ascent rate
        NewAscentRate(((gps->AltitudeFeet() - windData[i - 1].coord.alt) * 60) / windData[i].timeInterval);

        //FIXME write out the interval
        UART0::GetInstance()->WriteLine("%f radians in %d seconds\r\n", windData[i].course.dist, windData[i].timeInterval);
    }
    return false;
}

bool_t Log::PredictLanding(GPSData * landingPrediction) {
    // convert coordinates to degrees
    NavRadToDeg (&landingZone);

    // set the current time
    landingPrediction->hours = gps->hours;
    landingPrediction->minutes = gps->minutes;
    landingPrediction->seconds = gps->seconds;

    landingPrediction->latitude = (int32_t)(landingZone.lat * 10000000);
    landingPrediction->longitude = (int32_t)(landingZone.lon * 10000000);
    landingPrediction->altitude = 0;

    return true;
}

/**
 * Add the current ascent rate to the ascent rate list.  This permits low pass filtering
 * later on.
 *
 * @param rawRate New value for the ascent rate in ft/min
 */
void Log::NewAscentRate(int32_t rawRate) {
    for(int i = (ASCENT_RATE_LENGTH - 1); i > 0; i--)
        // shift the array one left
        ascentRates_[i] = ascentRates_[i - 1];

    // add the new entry to the beginning of the array
    ascentRates_[0] = rawRate;
}

/**
 * Returns the filtered ascent rate
 *
 * @return Ascent rate in ft/min
 */
int32_t Log::FilteredAscentRate() {
    // average gain in ft/min
    int32_t sumAltGains = 0;
    for (int i = 0; i < ASCENT_RATE_LENGTH; i++) {
        sumAltGains += ascentRates_[i];
    }
    return sumAltGains / ASCENT_RATE_LENGTH;
}


/**
 *    Calculate the distance and heading from <b>coord1</b> to <b>coord2</b>.  The
 *    coordinate values must be in radians.  The result saved in <b>course</b> is
 *    in units of radians.
 *
 *    @param coord1 start location
 *    @param coord2 end location
 *    @param course distance and heading
 */
void Log::NavCourse (COORD *coord1, COORD *coord2, COURSE *course)
{
    float d, lat1, lon1, lat2, lon2, angle;

    lat1 = coord1->lat;
    lon1 = coord1->lon;

    lat2 = coord2->lat;
    lon2 = coord2->lon;

    d = 2 * asin(sqrt( sin((lat1-lat2)/2)*sin((lat1-lat2)/2) + cos(lat1)*cos(lat2)*sin((lon1-lon2)/2)*sin((lon1-lon2)/2) ));
    course->dist = d;

    if (d < 0.00000027) {
        course->head = 0;
        return;
    }

    if (sin(lon2-lon1) < 0) {
        angle = (sin(lat2)-sin(lat1)*cos(d)) / (sin(d)*cos(lat1));

        if (angle < -1.0)
            angle = -1.0;

        if (angle > 1.0)
            angle = 1.0;

        course->head = acos(angle);
    } else {
        angle = (sin(lat2)-sin(lat1)*cos(d)) / (sin(d)*cos(lat1));

        if (angle < -1.0)
            angle = -1.0;

        if (angle > 1.0)
            angle = 1.0;

        course->head = 2*PI - acos( angle );
    }
}

/**
 *   Add the vector with a length <b>d</b> and true course <b>tc</b> to the
 *   lat/long <b>current</b> and save the result in <b>next</b>.
 *
 *   @param current lat/lon coordinates
 *   @param next lat/lon after vector added
 *   @param d distance in radians
 *   @param tc true course in radians
 */
void Log::NavDistRadial (COORD *current, COORD *next, float d, float tc)
{
    next->lat = asin(sin(current->lat) * cos(d) + cos(current->lat) * sin(d) * cos(tc));
    next->lon = current->lon - asin(sin(tc) * sin(d) / cos(next->lat));
}

/**
 * Initialize the wind data once the balloon has launched
 */
void Log::InitWindLog()
{
    this->Erase();
    this->burstDetect = false;

    NavSetDegFCoord(((float)gps->latitude / 10000000), ((float)gps->longitude / 10000000), &windData[0].coord);
    windData[0].coord.alt = gps->AltitudeFeet();
    windData[0].timeStamp = gps->hours * 3600 + gps->minutes * 60 + gps->seconds;
}

/**
 *   Convert coordinates from radians to decimal degrees.
 *
 *   @param coord pointer to coordinate pair
 */
void Log::NavRadToDeg (COORD *coord)
{
    coord->lat *= 180.0 / PI;
    coord->lon *= -180.0 / PI;
}

/**
 *   Convert coordinates (lat, lon) to radians and store in coord.
 *
 *   @param lat in degrees where north is positive
 *   @param lon in degrees where east is positive
 *   @param coord coordinate pair in radians
 */
void Log::NavSetDegFCoord (float lat, float lon, COORD *coord)
{
    coord->lat = lat * PI / 180.0;
    coord->lon = -lon * PI / 180.0;
}

/**
 *   Use a 4th order polynomial to calculate the descent rate in feet/second.
 *
 *   @param alt altitude in feet
 *
 *   @return descent in feet per second
 */
float Log::NavDescentRate (float alt)
{
    return -1E-18*alt*alt*alt*alt + 3E-13*alt*alt*alt - 8E-09*alt*alt + 0.0004*alt + 13.522;  // 4th order ANSR-13

//    return -5E-18*alt*alt*alt*alt + 9E-13*alt*alt*alt - 4E-08*alt*alt + 0.0010*alt + 9.3197; // 4th order ANSR-9
//    return -1E-18*alt*alt*alt*alt + 4E-13*alt*alt*alt - 2E-08*alt*alt + 0.0012*alt + 6.0886;  // 4th order ANSR-8
}
