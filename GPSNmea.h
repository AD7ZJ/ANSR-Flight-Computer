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

  	static GPSNmea *GetInstance();

private:
	void ParseGGA();
	void ParseRMC();

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

	typedef enum
	{
		WAITING,

		SENTENCE_HEADER,

		CARRIAGE_RET,

		NEWLINE
	} GPS_PARSE_STATE_MACHINE;

	typedef enum
	{
		GGA,

		RMC
	} NMEA_PACKET_TYPE;

	/// Buffer to store data as it is read from the GPS engine.
	char gpsBuffer[GPSBufferSize];

    /// Flag that is set when a data set has been parsed.
    bool_t dataReadyFlag;

    GPS_PARSE_STATE_MACHINE gpsParseState;

    NMEA_PACKET_TYPE gpsPacketType;
};

/** @} */

#endif  // #ifndef GPSNMEA_H

