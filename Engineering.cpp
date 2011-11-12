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
 * Filename:     Engineering.cpp                                           *
 *                                                                         *
 ***************************************************************************/

#include "main.h"

/// Reserve memory for singleton object.
static Engineering engineeringSingletonObject;

/**
 * Get a pointer to the DAC singleton object.
 */
Engineering *Engineering::GetInstance()
{
    return &engineeringSingletonObject;
}

/**
 *   Constructor.
 */
Engineering::Engineering()
{
    this->radioPower = true;    
    this->radioPTT = false;    
    this->radioFreq = false;
}

/**
 * Configure the engineering mode for operation.
 */
void Engineering::Enable()
{
    this->uart = UART0::GetInstance();
    
    this->gps = GPSLassen::GetInstance();
}

/**
 * Display the command menu on the control port.
 */
void Engineering::Menu()
{
    uart->WriteLine ("ANSR Flight Computer Engineering Console");
    //uart->WriteLine ("(S)tatus");
    uart->WriteLine ("(1) Radio Power");
    uart->WriteLine ("(2) Radio PTT");
    uart->WriteLine ("(3) Radio Freq");
    uart->WriteLine ("Re(b)oot");
    uart->WriteLine ("Trans(m)it");
    uart->WriteLine ("(n) MIC-E Test");
    //uart->WriteLine ("Cycle (d)ac output 0, 0.5, 1, 1.5, 2, 2.5, 3, 3.3 VDC");
    uart->WriteLine ("(h)elp");
    uart->WriteLine("");
}

/**
 * Set the GPS Data Set based on the desired unit test case.
 * 
 * @param gps pointer to GPS Data Set to populate
 * @param testCase number in the range 0 to 5
 */
void Engineering::MICEncoderTestCase (GPSData *gps, uint32_t testCase)
{
    switch (testCase)
    {
        // Home base.  N37 37.12' W111 44.03' 1760', 55MPH 95deg
        case 0:
            gps->latitude = 336187459;
            gps->longitude = -1117339150;
            gps->altitude = 53645; 
            gps->speed = 2459; 
            gps->heading = 9500;
            break;
            
        // N/W coordinates at boundaries.  N33 30.00' W111 59.98' 123,450', 215MPH, 359deg
        case 1:
            gps->latitude = 335000000;
            gps->longitude = -1119997222;
            gps->altitude = 3762756; 
            gps->speed = 9611; 
            gps->heading = 35900;
            break;  
            
        // N/W coordinates at boundaries. S21 45.67' W83 15.17, 25', 18MPH, 210deg 
        case 2:
            gps->latitude = -217611667;
            gps->longitude = -832527778;
            gps->altitude = 762; 
            gps->speed = 805; 
            gps->heading = 21010;
            break;  
            
        // N/W coordinates at boundaries. N55 47.50'  E 35.775', 30,500', 116MPH, 210deg  
        case 3:
            gps->latitude = 557916667;
            gps->longitude = 5963333;
            gps->altitude = 929640; 
            gps->speed = 5196; 
            gps->heading = 550;
            break;             
            
        // N45 00.00' W 79 45.00', 7000', 75MPH, 70.5deg 
        case 4:
            gps->latitude = 450000000;
            gps->longitude = -797500000;
            gps->altitude = 213360;  
            gps->speed = 3352; 
            gps->heading = 7050;
            break;    
            
        // N45 00.00' W 79 45.00', 7000', 110MPH, 70.5deg 
        case 5:
            gps->latitude = 557916667;
            gps->longitude = 5963333;
            gps->altitude = 929640; 
            gps->speed = 5093; 
            gps->heading = 550; 
            break;
            
        // 
        case 6:
            gps->latitude = 336089640;
            gps->longitude = -1117210150;
            gps->altitude = 47100;; 
            gps->speed = 1104;
            gps->heading = 28670; 
            break;
    } // END switch    
}

/**
 * Read and process any commands entered on the serial port.
 */
void Engineering::ProcessCommand()
{
    uint8_t value;
    char buffer[160];
    MICEncoder micEncoder;
    GPSData gpsData;
    
    while (uart->IsCharReady())
    {
        value = uart->ReadChar();
        
        switch (value)
        {
            case '1':
                radioPower = (radioPower ? false : true);
                IOPorts::RadioPower(radioPower);
                break;
                
            case '2':
                radioPTT = (radioPTT ? false : true);
                IOPorts::RadioPTT(radioPTT);
                break;
                
            case '3':
                radioFreq = (radioFreq ? false : true);
                IOPorts::RadioFreq(radioFreq);
                break;
                
            case 'b':
                uart->WriteLine ("rebooting");
                WatchDogTimer::Reset();
                break;
                
            case 'h':
                Menu();
                break;
                
            case 'm':
                if (AFSK::GetInstance()->IsTransmit())
                    break;
                
                NMEA::GPGGA (this->gps->Data(), buffer);
                uart->WriteLine (buffer);                
            
                AFSK::GetInstance()->Transmit(buffer);
                break;                
                
            case 'n':  
                if (AFSK::GetInstance()->IsTransmit())
                    break;
                
                uart->WriteLine ("Which test case?");
                
                while (!uart->IsCharReady());
                
                value = uart->ReadChar();
                
                if (value >= '0' && value <= '9')
                {
                    MICEncoderTestCase (&gpsData, value - '0');
                
                    micEncoder.Encode (&gpsData); 
                    
                    AFSK::GetInstance()->Transmit(micEncoder.GetInformationField(), micEncoder.GetDestAddress());
                } // END if
                break;  
                
            case 'u':
                Log::GetInstance()->Dump();
                break;    
                
            case 'e':
                uart->Write ("Are you sure you want to erase the flash memory (yes)? ");
                
                while (!uart->IsCharReady());
                
                if (uart->ReadChar() == 'y')
                {
                    while (!uart->IsCharReady());
                    
                    if (uart->ReadChar() == 'e')
                    {
                        while (!uart->IsCharReady());
                        
                        if (uart->ReadChar() == 's')
                        {                        
                            uart->WriteLine("");
                            uart->Write("Erasing flash memory...");
                            Log::GetInstance()->Erase();
                            uart->WriteLine ("done.");
                        } // END if
                    } // END if
                } // END if
                
                break;                

            case 't':
                sprintf (buffer, "%ld", LM92::GetInstance()->ReadTempF());
                uart->WriteLine (buffer);                
                break;                
                
            case 'i':
                uart->WriteLine (this->gps->Version());
                break;                
                       
            /*case 'q':
            	if(!sd_raw_init())
            	{
            		uart->WriteLine("SD Init Error\n\r");
            	}
            	break; */
            default:
                uart->WriteLine ("Unknown command.  Press 'h' for help.");
                break;

        } // END switch
    } // END while
}

