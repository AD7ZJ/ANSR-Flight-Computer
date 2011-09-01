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
 *               (c) Copyright, 2001-2008, Michael Gray                    *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 * Filename:     APRSBeacon.cpp                                            *
 *                                                                         *
 ***************************************************************************/

#include "main.h"

bool_t GPSLassen::HasData()
{
    return UART1::GetInstance()->IsCharReady();
}
uint8_t GPSLassen::ReadData()
{
    return UART1::GetInstance()->ReadChar();
}

void GPSLassen::WriteData(uint8_t data)
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

    strcat (text, "www.kd7lmo.net\015");
}

/**
 * Generate an APRS packet based on the current GPS time.
 */
void APRSBeacon::ScheduleMessage()
{
    char buffer[150];
    MICEncoder micEncoder;

    this->gps->Update();

    if (SystemControl::GetTick() > this->statusLEDOffTick)
        IOPorts::StatusLED(IOPorts::LEDGreen, false);

    if (this->gps->IsDataReady())
    {
        // Get a pointer to the GPS data set.
        GPSData *gpsData = this->gps->Data();

        // Log the fix information.
        Log::GetInstance()->GPSFix (gpsData);

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
                Log::GetInstance()->TimeStamp(gpsData);
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
        sprintf (buffer, "%ld %s %02d:%02d:%02d %d/%d/%d ", SystemControl::GetTick(), (gpsData->fixType == GPSData::NoFix ? "no fix" : (gpsData->fixType == GPSData::Fix2D ? "2D fix" : "3D fix")), gpsData->hours, gpsData->minutes, gpsData->seconds, gpsData->month, gpsData->day, gpsData->year);
        UART0::GetInstance()->Write (buffer);

        sprintf (buffer, "%ld %ld %ld %ld %d %d 0x%x ", gpsData->weekNumber, gpsData->timeOfWeek, gpsData->latitude, gpsData->longitude, gpsData->dop, gpsData->trackedSats, gpsData->navType);
        UART0::GetInstance()->Write (buffer);

        sprintf (buffer, "%d deg @ %ld knots ", gpsData->heading, gpsData->SpeedKnots());
        UART0::GetInstance()->Write (buffer);

        sprintf (buffer, "%ld", gpsData->AltitudeFeet());
        UART0::GetInstance()->WriteLine (buffer);
    } // END if
}

/**
 * Initialize the FAT library and SD card
 */
void APRSBeacon::fat_initialize() {
	UART0 * uart = UART0::GetInstance();
	if(!sd_raw_init())
	{
		uart->WriteLine("SD Init Error\n\r");
		return;
	}

	if(openroot())
	{
		uart->WriteLine("SD OpenRoot Error\n\r");
	}
	return;
}


/**
 * Start and run the mission application.
 */
void APRSBeacon::Run()
{
    // Set the system clock to the minimum speed required for USB operation.
    SystemControl::GetInstance()->Enable(SystemControl::Clock24MHz, SystemControl::Timer1Base);

    // Set the GPIO to the default state.
    IOPorts::Enable();

    // Turn on both LEDs.
    IOPorts::StatusLED (IOPorts::LEDYellow, true);

    // UART 0 is the console serial port, UART 1 is the GPS engine.
    UART0::GetInstance()->Enable(UART0::BaudRate57600);
    //UART1::GetInstance()->Enable(UART1::BaudRate9600, UART1::Control8O1);

    // SPI 0 is the CMX867A MODEM, SPI 1 is the M25P80 flash memory.
    SPI0::GetInstance()->Enable();
    //SPI1::GetInstance()->Enable(SPI1::MaxClock, SPI1::DataSize_8Bits, SPI1::CPOL0_CPHA0);

    // I2C Bus 0 is the temp sensor, real time clock, and 3D MEMS sensor.
    //I2C0::GetInstance()->Enable();

    // Enable and set the output DAC voltage to 0 VDC.
    DAC::GetInstance()->Enable();
    DAC::GetInstance()->Set (0);

    // CMX867A MODEM
    CMX867A::GetInstance()->Enable();

    // Temperature sensor.
    //LM92::GetInstance()->Enable();

    // Setup the flash memory for logging.
    //Log::GetInstance()->Enable();

    // Get the engineering control object and enable it.
    eng = Engineering::GetInstance();
    eng->Enable();

    // Objects used for the application
    this->afsk = AFSK::GetInstance();
    //this->gps = GPSLassen::GetInstance();

    // Turn on the radio and give it time to boot.
    IOPorts::RadioPower(true);
    SystemControl::Sleep(100);

    FIO0DIR |= (1<<20);

    // Show a startup message on the serial port.
    UART0::GetInstance()->WriteLine ("System booted");

    // Turn off the LEDs.
    IOPorts::StatusLED (IOPorts::LEDYellow, false);

    // Transmit a startup message.
    this->afsk->Transmit (">APRS Beacon v1.1");

    int * test;
    test = (int*)malloc(4);
    *test = 0xdefac8ed;

    fat_initialize();
    struct fat16_file_struct* handle;
	handle = root_open_new("test.txt");
	sd_raw_sync();

    //Log::GetInstance()->SystemBooted();

    for (;;)
    {

        // Check the serial port for engineering commands.
        eng->ProcessCommand();

        // Update the waveform state machine as required.
        this->afsk->Update();



        if (!this->afsk->IsTransmit())
            ScheduleMessage();

    } // END for
}

