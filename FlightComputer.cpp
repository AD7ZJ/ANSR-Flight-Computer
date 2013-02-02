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
 * Filename:     FlightComputer.cpp                                            *
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
static FlightComputer flightComputerSingletonObject;

/**
 *  Get a pointer to the HF-APRS control object.
 */
FlightComputer *FlightComputer::GetInstance()
{
    return  &flightComputerSingletonObject;
}

/**
 * Constructor.
 */
FlightComputer::FlightComputer()
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
    this->launchDetect = false;
    this->passThruMode = false;
}

/**
 * Generate a plain text status packet.
 *
 * @param gps pointer to current GPS fix object
 * @param text pointer to NULL terminated string with status packet
 */
void FlightComputer::StatusPacket(const GPSData *gps, char *text)
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

void FlightComputer::LandingPrediction() {
    MICEncoder micEncoder;
    if(Log::GetInstance()->burstDetect) {
        Log::GetInstance()->PredictLanding(&landingPrediction);
        //NMEA::GPGGA(&landingPrediction, buffer);
        //UART0::GetInstance()->WriteLine(buffer);
        // transmit the predictions on a different SSID
        this->afsk->PredictPacketSrc();
        micEncoder.Encode (&landingPrediction);
        this->afsk->Transmit (micEncoder.GetInformationField(), micEncoder.GetDestAddress());
        this->afsk->NormalPacketSrc();
    }
}

/**
 * Generate an APRS packet based on the current GPS time.
 */
void FlightComputer::ScheduleMessage()
{
    char buffer[150];
    MICEncoder micEncoder;

    // Check for new characters from the GPS engine
    this->gps->Update();

    if (SystemControl::GetTick() > this->statusLEDOffTick)
        IOPorts::StatusLED(IOPorts::LEDGreen, false);

    if (this->gps->IsDataReady())
    {
        // Get a pointer to the GPS data set.
        GPSData *gpsData = this->gps->Data();

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

                // set the real-time clock
                RTC::GetInstance()->Set(gpsData);
            }

            // update the wind data table
            Log::GetInstance()->UpdateWindTable();

            // check for launch
            if(!launchDetect) {
                posAscentCount = posAscentCount << 1;
                if(Log::GetInstance()->RawAscentRate() > 300)
                    posAscentCount |= 0x01;

                // if there's been 2 consecutive readings with a positive ascent rate
                if((posAscentCount & 0x03) == 0x03) {
                    launchDetect = true;
                    // Initialize the wind data log
                    Log::GetInstance()->InitWindLog();

                    Log::GetInstance()->LaunchDetected();
                }
            }
        }

        IOPorts::StatusLED(IOPorts::LEDGreen, true);

        if (gpsData->fixType == GPSData::NoFix)
            this->statusLEDOffTick = SystemControl::GetTick() + 800;
        else {
            this->statusLEDOffTick = SystemControl::GetTick() + 100;

            switch (gpsData->seconds)
            {
                case 15:
                    StatusPacket (this->gps->Data(), buffer);
                    this->afsk->Transmit (buffer);
                    break;

                case 25:
                    // transmit a landing prediction packet
                    this->LandingPrediction();
                    break;
                case 0:
                case 30:
                    // transmit current location
                    micEncoder.Encode (this->gps->Data());
                    this->afsk->Transmit (micEncoder.GetInformationField(), micEncoder.GetDestAddress());
                    break;
            } // END switch
        }

        // Engineering data set reported
        sprintf (buffer, "%s Time:%02d:%02d:%02d %d/%d/%d\r\n", (gpsData->fixType == GPSData::NoFix ? "no fix" : (gpsData->fixType == GPSData::Fix2D ? "2D fix" : "3D fix")), gpsData->hours, gpsData->minutes, gpsData->seconds, gpsData->month, gpsData->day, gpsData->year);
        //UART0::GetInstance()->Write (buffer);

        sprintf (buffer, "Lat:%ld Long:%ld Dop:%d Tracked Sats:%d\r\n", gpsData->latitude, gpsData->longitude, gpsData->dop, gpsData->trackedSats);
        //UART0::GetInstance()->Write (buffer);

        sprintf (buffer, "%d deg @ %ld knots ", gpsData->heading, gpsData->SpeedKnots());
        //UART0::GetInstance()->Write (buffer);

        sprintf (buffer, "Alt:%ld ft", gpsData->AltitudeFeet());
        UART0::GetInstance()->WriteLine (buffer);

        UART0::GetInstance()->WriteLine ("AscentRate: %d", Log::GetInstance()->FilteredAscentRate());
    } // END if
}

/**
 * Function called every 1 ms to generate the application's timer tick
 */
void FlightComputer::SystemTimerTick()
{
    // Get a pointer to the application's instance
    FlightComputer * fc = FlightComputer::GetInstance();

    // run the 1ms task necessary for the SD card library
    disk_timerproc();

    fc->msElapsed++;
	
	// set 100ms, 1s, and 10s flags used for timing in the main loop
	if(fc->msElapsed >= 100) {
	    fc->ms100Elapsed++;
	    fc->msElapsed = 0;
	    fc->timer100msFlag = true;
	}
	
	if(fc->ms100Elapsed >= 10) {
	    fc->sElapsed++;
	    fc->ms100Elapsed = 0;
	    fc->timer1sFlag = true;
	}

	if(fc->sElapsed >= 10) {
	    fc->sElapsed = 0;
	    fc->timer10sFlag = true;
	}
}


/**
 * Start and run the mission application.
 */
void FlightComputer::Run()
{
	int32_t tempF;
	char tempBuffer[40];
	UINT numChars;

    // Set the system clock to the minimum speed required for USB operation.
    SystemControl::GetInstance()->Enable(SystemControl::Clock24MHz, SystemControl::Timer1Base);

    // Setup Timer1 to run our system timer tick function on each tick
    Timer1::GetInstance()->SetCallback((void (*)())&FlightComputer::SystemTimerTick);

    // Set the GPIO to the default state.
    IOPorts::Enable();

    // Turn on both LEDs.
    IOPorts::StatusLED (IOPorts::LEDYellow, true);

    // UART 0 is the console serial port, UART 1 is the GPS engine.
    UART0::GetInstance()->Enable(UART0::BaudRate57600);
    UART1::GetInstance()->Enable(UART1::BaudRate4800, UART1::Control8N1);

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

    // Show a startup message on the serial port.
    UART0::GetInstance()->WriteLine (">ANSR Flight Computer v0.3 booted!");

    // Turn off the LEDs.
    IOPorts::StatusLED (IOPorts::LEDYellow, false);

    // Transmit a startup message.
    this->afsk->Transmit (">ANSR Flight Computer v0.3");

    Log::GetInstance()->SystemBooted();

    RTCTime * time = RTC::GetInstance()->Get();

    for (;;)
    {
        // forward packets to the UART from the GPS if in passthru mode
        if(passThruMode) {
            GPSNmea::GetInstance()->GpsPassthru();
        }
        else {
            // Check the serial port for engineering commands.
            eng->ProcessCommand();

            // Update the waveform state machine as required.
            this->afsk->Update();

            if (!this->afsk->IsTransmit())
                ScheduleMessage();

            // 100 ms tasks
            if(this->timer100msFlag) {
                this->timer100msFlag = false;
                // update the repeater controller
                Repeater::GetInstance()->Update();
            }


            // 1 second tasks
            if(this->timer1sFlag) {
                this->timer1sFlag = false;
            }

            // 10 second tasks
            if(this->timer10sFlag && !this->afsk->IsTransmit()) {
                this->timer10sFlag = false;
                // log the current GPS fix
                Log::GetInstance()->GPSFix(gps->Data());
                IOPorts::StatusLED(IOPorts::LEDRed, true);
                Log::GetInstance()->Flush();
                IOPorts::StatusLED(IOPorts::LEDRed, false);
            }
        }
    } // END for(;;)
}




