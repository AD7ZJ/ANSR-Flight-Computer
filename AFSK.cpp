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
 * Filename:     AFSK.cpp                                                  *
 *                                                                         *
 ***************************************************************************/

#include "main.h"

/// Reserve memory for singleton object.
static AFSK afskSingletonObject;

/**
 *  Get a pointer to the Waveform waveform generation object.
 */
AFSK *AFSK::GetInstance()
{
    return  &afskSingletonObject;
}

/// Default destination address to transmit if one is not specified.
const char DEFAULT_DEST_ADDRESS[] = { "APRS" };

/**
 * Constructor.
 */
AFSK::AFSK()
{
    this->modem = CMX867A::GetInstance();

    this->txFlag = false;
    this->txDoneFlag = false;

    // Set the preamble time to 200mS.
    ax25.SetPreambleCount (30);

    // Set the source, destination, and DIGI paths.
    ax25.SetSourceAddress ("KD7LMO", 11);
    ax25.SetDestAddress ("APRS", 0);
    ax25.SetDigiPath ("GATE", 0, "WIDE2", 2);
}


/**
 * Generate and transmit a text string with AX.25 encoding and A-FSK modulation.
 *
 * @param destAddress pointer to NULL terminated string that contains destination address
 * @param text pointer to NULL terminated string to send
 */
void AFSK::Transmit (const char *text, const char *destAddress)
{
    if (this->txFlag)
        return;

    if (destAddress == NULL)
        ax25.SetDestAddress (DEFAULT_DEST_ADDRESS, 0);
    else
        ax25.SetDestAddress (destAddress, 0);

    // Generate the AX.25 stream.
    ax25.GenerateMessage (text);

    // Set a pointer to the first byte in the message.
    this->messageIndex = 0;

    IOPorts::StatusLED (IOPorts::LEDRed, true);
    IOPorts::RadioPTT(true);

    this->txFlag = true;
}

/**
 * Method that is periodically called in the same thread as the transmit methods.
 */
void AFSK::Update()
{
    // Quit if we don't have any data to transmit.
    if (!this->txFlag)
        return;

    // Determine if the MODEM is ready for another byte.
    if (!this->modem->IsTxDataReady())
        return;

    // If we are the end of the message, shut down the radio.
    if (this->messageIndex == ax25.Length())
    {
        IOPorts::RadioPTT(false);
        IOPorts::StatusLED (IOPorts::LEDRed, false);

        this->txFlag = false;

        return;
    } // END if

    // Send the next byte in the stream.
    this->modem->SendByte(ax25.Stream()[this->messageIndex]);

    // And select the next byte.
    ++this->messageIndex;
}

/**
 * Indicates if a transmission is in progress.
 *
 * @return true if in transmit mode; otherwise false
 */
bool_t AFSK::IsTransmit()
{
    return this->txFlag;
}
