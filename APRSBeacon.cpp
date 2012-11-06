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
 * Filename:     APRSBeacon.cpp                                            *
 *                                                                         *
 ***************************************************************************/

#include "main.h"

bool_t GPSNmea::HasData()
{
    return UART1::GetInstance()->IsCharReady();
}
uint8_t GPSNmea::ReadData()
{
    return UART1::GetInstance()->ReadChar();
}

void GPSNmea::WriteData(uint8_t data)
{
    UART1::GetInstance()->WriteChar (data);
}

void M25P80::IOSetCS(bool_t state)
{
    SPI1::GetInstance()->SetCS (state);
}

uint8_t M25P80::SPIRead()
{
    return SPI1::GetInstance()->Read();
}

void M25P80::SPIWrite(uint8_t data)
{
    SPI1::GetInstance()->Write(data);
}

void CMX867A::IOSetCS(bool_t state)
{
    SPI0::GetInstance()->SetCS(state);
}

uint8_t CMX867A::SPIRead()
{
    return SPI0::GetInstance()->Read();
}

void CMX867A::SPIWrite(uint8_t data)
{
    SPI0::GetInstance()->Write(data);
}

/// Reserve memory for singleton object.
static APRSBeacon APRSBeaconSingletonObject;

/**
 *  Get a pointer to the HF-APRS control object.
 */
APRSBeacon *APRSBeacon::GetInstance()
{
    return  &APRSBeaconSingletonObject;
}

/**
 * Constructor.
 */
APRSBeacon::APRSBeacon()
{
    this->peakAltitude = 0;
    this->startGPSTime = 0;
    this->statusLEDOffTick = 0;

    this->timer100msFlag = false;
    this->timer10sFlag = false;
    this->timer1sFlag = false;
    this->ms100Elapsed = 0;
    this->msElapsed = 0;
    this->sElapsed = 0;
}

/**
 * Generate a plain text status packet.
 *
 * @param gps pointer to current GPS fix object
 * @param text pointer to NULL terminated string with status packet
 */
void APRSBeacon::StatusPacket(const GPSData *gps, char *text)
{
    uint32_t time;
    char buffer[100];

    // Generate the GPRMC message.
    if (this->startGPSTime == 0)
        strcpy (text, ">Balloon -:--:-- ");
    else {
        time = gps->TimeSinceEpoch() - this->startGPSTime;

        sprintf (text, ">Balloon %ld:%02ld:%02ld ", time / 3600, ((time / 60) % 60), time % 60);
    } // END if-else

    sprintf (buffer, "%ld' %ld'pk ", gps->AltitudeFeet(), this->peakAltitude);
    strcat (text, buffer);

    // DOP and number of tracked birds.
    sprintf (buffer, "%u.%01u%cdop %utrk ", gps->dop / 10, gps->dop % 10, (gps->fixType == GPSData::NoFix ? '-' : (gps->fixType == GPSData::Fix2D ? 'h' : 'p')), gps->trackedSats);
    strcat (text, buffer);

    // Internal temperature.
    sprintf (buffer, "%lddegF ", LM92::GetInstance()->ReadTempF() / 10);
    strcat (text, buffer);

    strcat (text, "www.ansr.org\015");
}

/**
 * Generate an APRS packet based on the current GPS time.
 */
void APRSBeacon::ScheduleMessage()
{
    char buffer[150];
    MICEncoder micEncoder;

    IOPorts::RadioPTT(true);
    this->gps->Update();
    IOPorts::RadioPTT(false);

    if (SystemControl::GetTick() > this->statusLEDOffTick)
        IOPorts::StatusLED(IOPorts::LEDGreen, false);

    if (this->gps->IsDataReady())
    {
        // Get a pointer to the GPS data set.
        GPSData *gpsData = this->gps->Data();

        // update the wind data table
        Log::GetInstance()->UpdateWindTable();

        // Process certain elements when we have a 3D fix.
        if (gpsData->fixType == GPSData::Fix3D)
        {
            // Record the peak altitude.
            if (gpsData->AltitudeFeet() > this->peakAltitude)
                this->peakAltitude = gpsData->AltitudeFeet();

            // Record the starting time.
            if (this->startGPSTime == 0)
            {
                this->startGPSTime = gpsData->TimeSinceEpoch();
                //Log::GetInstance()->TimeStamp(gpsData);
            }
        }

        IOPorts::StatusLED(IOPorts::LEDGreen, true);

        if (gpsData->fixType == GPSData::NoFix)
            this->statusLEDOffTick = SystemControl::GetTick() + 800;
        else
            this->statusLEDOffTick = SystemControl::GetTick() + 100;

        switch (gpsData->seconds)
        {
            case 0:
            case 30:
                StatusPacket (this->gps->Data(), buffer);
                this->afsk->Transmit (buffer);
                break;

            case 5:
            case 25:
            case 35:
            case 55:
                micEncoder.Encode (this->gps->Data());
                this->afsk->Transmit (micEncoder.GetInformationField(), micEncoder.GetDestAddress());
                break;
        } // END switch

        // Engineering data set reported
        sprintf (buffer, "%s Time:%02d:%02d:%02d %d/%d/%d\r\n", (gpsData->fixType == GPSData::NoFix ? "no fix" : (gpsData->fixType == GPSData::Fix2D ? "2D fix" : "3D fix")), gpsData->hours, gpsData->minutes, gpsData->seconds, gpsData->month, gpsData->day, gpsData->year);
        UART0::GetInstance()->Write (buffer);

        sprintf (buffer, "Lat:%ld Long:%ld Dop:%d Tracked Sats:%d\r\n", gpsData->latitude, gpsData->longitude, gpsData->dop, gpsData->trackedSats);
        UART0::GetInstance()->Write (buffer);

        sprintf (buffer, "%d deg @ %ld knots ", gpsData->heading, gpsData->SpeedKnots());
        UART0::GetInstance()->Write (buffer);

        sprintf (buffer, "Alt:%ld ft", gpsData->AltitudeFeet());
        UART0::GetInstance()->WriteLine (buffer);
    } // END if
}

void APRSBeacon::SystemTimerTick()
{
    //IOPorts::RadioPTT(true);
    disk_timerproc();
	APRSBeacon::GetInstance()->msElapsed++;
	
	// set 100ms, 1s, and 10s flags used for timing in the main loop
	if(APRSBeacon::GetInstance()->msElapsed >= 100) {
	    APRSBeacon::GetInstance()->ms100Elapsed++;
	    APRSBeacon::GetInstance()->msElapsed = 0;
	    APRSBeacon::GetInstance()->timer100msFlag = true;
	}
	
	if(APRSBeacon::GetInstance()->ms100Elapsed >= 10) {
	    APRSBeacon::GetInstance()->sElapsed++;
	    APRSBeacon::GetInstance()->ms100Elapsed = 0;
	    APRSBeacon::GetInstance()->timer1sFlag = true;
	}

	if(APRSBeacon::GetInstance()->sElapsed >= 10) {
	    APRSBeacon::GetInstance()->sElapsed = 0;
	    APRSBeacon::GetInstance()->timer10sFlag = true;
	}
}


/**
 * Start and run the mission application.
 */
void APRSBeacon::Run()
{
	int32_t tempF;
	char tempBuffer[40];
	UINT numChars;

    // Set the system clock to the minimum speed required for USB operation.
    SystemControl::GetInstance()->Enable(SystemControl::Clock24MHz, SystemControl::Timer1Base);

    // Setup Timer1 to run our system timer tick function on each tick
    Timer1::GetInstance()->SetCallback((void (*)())&APRSBeacon::SystemTimerTick);

    // Set the GPIO to the default state.
    IOPorts::Enable();

    // Turn on both LEDs.
    IOPorts::StatusLED (IOPorts::LEDYellow, true);

    // UART 0 is the console serial port, UART 1 is the GPS engine.
    UART0::GetInstance()->Enable(UART0::BaudRate57600);
    UART1::GetInstance()->Enable(UART1::BaudRate9600, UART1::Control8N1);

    // SPI 0 is the CMX867A MODEM.  SPI 1 is the SD card which is setup by the SDlogger class
    SPI0::GetInstance()->Enable();

    // I2C Bus 0 is the temp sensor
    I2C0::GetInstance()->Enable();

    // Enable and set the output DAC voltage to 0 VDC.
    DAC::GetInstance()->Enable();
    DAC::GetInstance()->Set (0);

    // CMX867A MODEM
    CMX867A::GetInstance()->Enable();

    // Temperature sensor.
    LM92::GetInstance()->Enable();

    // Setup the flash memory for logging.
    Log::GetInstance()->Enable();

    // Enable the RTC
    RTC::GetInstance()->Enable();

    // Get the engineering control object and enable it.
    eng = Engineering::GetInstance();
    eng->Enable();

    // Objects used for the application
    this->afsk = AFSK::GetInstance();
    this->gps = GPSNmea::GetInstance();

    // Turn on the radio and give it time to boot.
    IOPorts::RadioPower(true);
    SystemControl::Sleep(1000);

    // Show a startup message on the serial port.
    UART0::GetInstance()->WriteLine (">ANSR Flight Computer v0.3 booted!");

    // Turn off the LEDs.
    IOPorts::StatusLED (IOPorts::LEDYellow, false);

    // Transmit a startup message.
    this->afsk->Transmit (">ANSR Flight Computer v0.3");

    //Log::GetInstance()->SystemBooted();

    SDLogger gpsLogger;
    gpsLogger.Enable("gps.bin", FA_OPEN_ALWAYS | FA_WRITE);

    char gpsTestBuffer[] = "GPS log works...";

    gpsLogger.Append(gpsTestBuffer, 16);
    gpsLogger.SyncDisk();

    // instantiate the SDLogger class
    SDLogger tempLogger;
    tempLogger.Enable("test.txt", FA_OPEN_EXISTING | FA_WRITE);

	RTCTime * time;


    for (;;)
    {
        // Check the serial port for engineering commands.
        eng->ProcessCommand();

        // Update the waveform state machine as required.
        this->afsk->Update();

        time = RTC::GetInstance()->Get();

        if (!this->afsk->IsTransmit())
            ScheduleMessage();

        //get the temp every second
        if(this->timer1sFlag) {
            this->timer1sFlag = false;

            tempF = LM92::GetInstance()->ReadTempF();
            // write the temperature out to the SD card
            numChars = sprintf(tempBuffer, "%0.2f degrees F. %d:%d:%d %ul\n", (float)tempF / 10, (int)time->hours, (int)time->minutes, (int)time->seconds, SystemControl::GetInstance()->CStackSize());

            if(!tempLogger.Append(tempBuffer, numChars))
                UART0::GetInstance()->WriteLine (">Failed to write data to SD card");

            UART0::GetInstance()->WriteLine(tempBuffer);

            IOPorts::StatusLED(IOPorts::LEDRed, true);
            tempLogger.SyncDisk();
            IOPorts::StatusLED(IOPorts::LEDRed, false);
        }

    } // END for(;;)
}




