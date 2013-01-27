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
 * Filename:     GPSLassen.h                                               *
 *                                                                         *
 ***************************************************************************/

#ifndef GPSNMEA_H
#define GPSNMEA_H

/**
 *  @defgroup external External Peripherals
 *
 *  @{
 */

/**
 *   Read and process the GPS Binary messages from a generic
 *   GPS engine outputting NMEA strings
 */


class GPSNmea
{
public:
    GPSNmea();

    GPSData *Data();
    bool_t IsDataReady();
    void Update();

    /**
     *   Callback to determine if the there is serial data to process from the GPS engine.
     *
     *    @return true if data is in FIFO; otherwise false
     */
    virtual bool_t HasData();

    /**
     *   Callback to read the oldest serial data from the GPS engine.
     *
     *   @return oldest byte from serial FIFO
     */
    virtual uint8_t ReadData();

    /**
     *   Callback to write serial data to the GPS engine.
     *
     *   @param data 8-bit data to write
     */
    virtual void WriteData(uint8_t data);

    /**
     *   Passes any characters from the GPS to the serial port and vice versa
     */
    void GpsPassthru();

    /**
     *   Get a pointer to the GPSNmea singleton
     */
  	static GPSNmea *GetInstance();


private:
  	static const uint8_t MAX_CMD_LEN = 8;			// maximum command length (NMEA address)
  	static const uint8_t MAX_DATA_LEN = 255;		// maximum data length
  	static const uint8_t MAX_CHAN = 36;				// maximum number of channels
  	static const uint8_t WAYPOINT_ID_LEN = 32;		// waypoint max string len
  	static const uint8_t MAXFIELD = 25;

    /// The maximum length of a binary GPS engine message.
    static const uint32_t GPSBufferSize = 80;

    /// Object that stores the current GPS fix data.
    GPSData data;

	/// Index into gpsBuffer used to store message data.
	uint32_t gpsIndex;

	/// GPS Time of Week in seconds.
	uint32_t timeOfWeek;

	/// Offset in seconds between GPS and UTC time.
	uint32_t utcOffset;

	/// last altitude in ft
	uint32_t lastAltitude;

	/// last timestamp in seconds
	uint32_t lastTimestamp;

	typedef enum
	{
		STARTOFMESSAGE,

		COMMAND,

		DATA,

		CHECKSUM_1,

		CHECKSUM_2
	} GPS_PARSE_STATE_MACHINE;

	typedef enum
	{
		GGA,

		RMC
	} NMEA_PACKET_TYPE;

	/// Buffer to store data as it is read from the GPS engine.
	char gpsBuffer[GPSBufferSize];

	uint8_t calcChecksum;					// Calculated NMEA sentence checksum
	uint8_t receivedChecksum;			   	// Received NMEA sentence checksum (if exists)
	uint16_t index;							// Index used for command and data
	uint8_t commandBuffer[MAX_CMD_LEN];				// NMEA command
	uint8_t dataBuffer[MAX_DATA_LEN];				// NMEA data


    /// Flag that is set when a data set has been parsed.
    bool_t dataReadyFlag;

    GPS_PARSE_STATE_MACHINE gpsParseState;

    NMEA_PACKET_TYPE gpsPacketType;

    void ProcessCommand(uint8_t *pCommand, uint8_t *pData);
    bool GetField(uint8_t *pData, uint8_t *pField, int8_t nFieldNum, int8_t nMaxFieldLen);
    void ProcessGPGGA(uint8_t *pData);
    void ProcessGPRMC(uint8_t *pData);
};

/** @} */

#endif  // #ifndef GPSNMEA_H

