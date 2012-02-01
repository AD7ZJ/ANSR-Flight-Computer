/***************************************************************************
 *                                                                         *
 *                              ESS, Inc.                                  *
 *                                                                         *
 *    ESS, Inc. CONFIDENTIAL AND PROPRIETARY.  This source is the sole     *
 *    property of ESS, Inc.  Reproduction or utilization of this source    *
 *    in whole or in part is forbidden without the written consent of      *
 *    ESS, Inc.                                                            *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 *            (c) Copyright 1997-2008 -- All Rights Reserved               *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 * Filename:     GPSNmea.cpp                                             *
 *                                                                         *
 ***************************************************************************/

#include "main.h"

/// Reserve memory for the singleton object.
static GPSNmea nmeaSingletonObject;

/**
 * Constructor.
 */
GPSNmea::GPSNmea()
{
    this->dataReadyFlag = false;
    this->utcOffset = 0;
    this->timeOfWeek = 0;
    this->gpsParseState = WAITING;
    this->data.navType = 0xff;
}

/**
 * Get a pointer to the Lassen GPS Engine object.
 */
GPSNmea *GPSNmea::GetInstance()
{
	return 	&nmeaSingletonObject;
}

/**
 *   Get a pointer to the GPSData object that contains the latest fix information.
 *
 *   @return pointer to GPSData object
 */
GPSData *GPSNmea::Data()
{
    return &this->data;
}


/**
 *   Determine if new GPS message is ready to process.  This function is a one shot and
 *   typically returns true once a second for each GPS position fix.
 *
 *   @return true if new message available; otherwise false
 */
bool_t GPSNmea::IsDataReady()
{
    if (this->dataReadyFlag)
    {
        this->dataReadyFlag = false;
        return true;
    } //END if

    return false;
}

/**
 *   Read the serial FIFO and process complete GPS messages.
 */
void GPSNmea::Update() {
    uint32_t value;

    // This state machine handles each characters as it is read from the GPS serial port.
    while (HasData()) {
        // Get the character value.
        value = ReadData();
        //this->gpsBuffer[this->gpsIndex++] = value;

        // Parse the NMEA packet
        switch(this->gpsParseState) {
			//case '\n':
			//	if(this->gpsBuffer[this->gpsIndex - 2] == '\r') {
			//		this->gpsBuffer[this->gpsIndex = 0];
			//		UART0::GetInstance()->WriteLine("Begin: %s", gpsBuffer);
			//		this->gpsIndex = 0;
			//	}
			//	break;


			case WAITING:
				switch (value) {
					case '$':
						this->gpsBuffer[this->gpsIndex++] = value;
						gpsParseState = SENTENCE_HEADER;
						break;
					default:
						this->gpsIndex = 0;
						break;
				}
				break;

			case SENTENCE_HEADER:
				this->gpsBuffer[this->gpsIndex++] = value;
				switch (value) {
					case 'A':
						if(this->gpsIndex == 6) {
							// Assume it's a GGA packet
							this->gpsPacketType = GGA;
							this->gpsParseState = CARRIAGE_RET;
						}
						break;
					case 'C':
						if(this->gpsIndex == 6) {
							// Assume it's an RMC packet
							this->gpsPacketType = RMC;
							this->gpsParseState = CARRIAGE_RET;
						}
						break;
					case '\n':
					case '\r':
						// we've missed something, trash the packet and wait for the next one
						this->gpsParseState = WAITING;
						this->gpsIndex = 0;
						break;
				}

				break;

			case CARRIAGE_RET:
				this->gpsBuffer[this->gpsIndex++] = value;
				if(value == '\r') {
					this->gpsParseState = NEWLINE;
				}

				if(value == '\n') {
					// we've missed something, trash the packet and wait for the next one
					this->gpsParseState = WAITING;
					this->gpsIndex = 0;
				}
				break;

			case NEWLINE:
				if(value != '\n') {
					// we've missed something, trash the packet and wait for the next one
					this->gpsParseState = WAITING;
					this->gpsIndex = 0;
				}
				else {
					this->gpsBuffer[this->gpsIndex++] = value;
					UART0::GetInstance()->WriteLine("%s", gpsBuffer);
					if(this->gpsPacketType == GGA)
						ParseGGA();
					else
						ParseRMC();
					this->gpsParseState = WAITING;
					this->gpsIndex = 0;
				}

				break;
        } // END switch
    } // END while
}

void GPSNmea::ParseRMC() {
	char * temp;
	char substr[2];
	temp = strtok(gpsBuffer, ",");
	if(strcmp(temp, "$GPRMC"))
		return;

	// Split out the time field
	temp = strtok(NULL, ",");
	// grab the hours
	strncpy(substr, temp, 2);
	this->data.hours = atoi(substr);
	// grab the minutes
	strncpy(substr, (temp + 2), 2);
	this->data.minutes = atoi(substr);
	// grab the seconds
	strncpy(substr, (temp + 4), 2);
	this->data.seconds = atoi(substr);

	// Status field; ignoring for now
	temp = strtok(NULL, ",");

	// split out the latitude
	temp = strtok(NULL, ",");
	// grab the degree field of the lat
	strncpy(substr, temp, 2);
	this->data.latitude = (10000000 * (int32_t)atoi(substr));
	// grab the minutes field of the latitude
	this->data.latitude += lroundf((10000000 * atof(temp + 2)) / 60);
	// figure out if it's north or south
	temp = strtok(NULL, ",");
	this->data.latitude = (temp[0] == 'N') ? this->data.latitude : -this->data.latitude;

	// split out the longitude
	temp = strtok(NULL, ",");
	// grab the degree field of the long
	strncpy(substr, temp, 3);
	this->data.longitude = (10000000 * (int32_t)atoi(substr));
	// grab the minutes field of the longitude
	this->data.longitude += lroundf((10000000 * atof(temp + 3)) / 60);
	// Zero out the 3rd byte of substr
	substr[2] = 0;

	// figure out if it's east or west
	temp = strtok(NULL, ",");
	this->data.longitude = (temp[0] == 'E') ? this->data.longitude : -this->data.longitude;

	// Groundspeed in knots, converted to cm/s
	temp = strtok(NULL, ",");
	this->data.speed = lroundf(atof(temp) * 51.4444444);

	// heading in degrees, converted to hundreths of a degree
	temp = strtok(NULL, ",");
	this->data.heading = lroundf(atof(temp) * 100);

	// date in ddmmyy
	temp = strtok(NULL, ",");
	// grab the day
	strncpy(substr, temp, 2);
	printf("Date field: %s Day field: %s\n",temp, substr);
	this->data.day = atoi(substr);
	// grab the month
	strncpy(substr, (temp + 2), 2);
	this->data.month = atoi(substr);
	// grab the year
	strncpy(substr, (temp + 4), 2);
	this->data.year = (atoi(substr) + 2000);

	//printf("Parsed time: %d:%d:%d ; Lat: %d Long: %d\n", this->data.hours, this->data.minutes, this->data.seconds, this->data.latitude, this->data.longitude);
	//printf("Groundspeed: %d Heading: %d Date (dd/mm/yyyy): %d/%d/%d\n", this->data.speed, this->data.heading, this->data.day, this->data.month, this->data.year);
}

void GPSNmea::ParseGGA() {
	char * temp;
	char substr[2];
	temp = strtok(gpsBuffer, ",");
	if(strcmp(temp, "$GPGGA"))
		return;

	// Split out the time field
	temp = strtok(NULL, ",");
	// grab the hours
	strncpy(substr, temp, 2);
	this->data.hours = atoi(substr);
	// grab the minutes
	temp = temp + 2;
	strncpy(substr, temp, 2);
	this->data.minutes = atoi(substr);
	// grab the seconds
	temp = temp + 2;
	strncpy(substr, temp, 2);
	this->data.seconds = atoi(substr);

	// split out the latitude
	temp = strtok(NULL, ",");
	// grab the degree field of the lat
	strncpy(substr, temp, 2);
	this->data.latitude = (10000000 * (int32_t)atoi(substr));
	// grab the minutes field of the latitude
	temp = temp + 2;
	this->data.latitude += lroundf((10000000 * atof(temp)) / 60);

	// figure out if it's north or south
	temp = strtok(NULL, ",");
	this->data.latitude = (temp[0] == 'N') ? this->data.latitude : -this->data.latitude;

	// split out the longitude
	temp = strtok(NULL, ",");
	// grab the degree field of the long
	strncpy(substr, temp, 3);
	this->data.longitude = (10000000 * (int32_t)atoi(substr));
	// grab the minutes field of the longitude
	temp = temp + 3;
	this->data.longitude += lroundf((10000000 * atof(temp)) / 60);

	// figure out if it's east or west
	temp = strtok(NULL, ",");
	this->data.longitude = (temp[0] == 'E') ? this->data.longitude : -this->data.longitude;

	// Grab the fix data
	temp = strtok(NULL, ",");
	if(atoi(temp) == 0)
		this->data.fixType = GPSData::NoFix;

	// Number of birds being tracked
	temp = strtok(NULL, ",");
	this->data.trackedSats = atoi(temp);

	// Horizontal dilution of precision (HDOP)
	temp = strtok(NULL, ",");
	this->data.dop = lroundf(10 * atof(temp));

	// Altitude above MSL
	temp = strtok(NULL, ",");
	this->data.altitude = lroundf(100 * atof(temp));

	//printf("Parsed time: %d:%d:%d ; Lat: %d Long: %d\nTracked sats: %d\n", this->data.hours, this->data.minutes, this->data.seconds, this->data.latitude, this->data.longitude, this->data.trackedSats);
	//printf("Dilution of precision: %d Alt: %d m\n", this->data.dop, this->data.altitude / 100);
}
