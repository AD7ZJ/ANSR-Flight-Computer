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
 *               (c) Copyright, 1997-2012, ANSR                            *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 * Filename:     Repeater.cpp                                              *
 *                                                                         *
 ***************************************************************************/

#include "main.h"

/// Reserve memory for singleton object.
static Repeater repeaterSingletonObject;

/// How often in milliseconds the repeater should ID
const uint32_t repeaterIDPeriod = (10 * 60 * 1000);

/// Transmit duration in ms
const uint32_t maxTxDuration = (3 * 60 * 1000);

/**
 *  Get a pointer to the repeater control object.
 */
Repeater * Repeater::GetInstance()
{
    return  &repeaterSingletonObject;
}

/**
 * Constructor
 */
 Repeater::Repeater() {
     this->debouncedCD = false;
     this->repeaterIsTransmitting = false;
     this->repeaterIDtick = repeaterIDPeriod;
     this->CW_INTER_SYMBOL_TIME = 56; //85;
     this->CW_INTER_CHAR_TIME = 168; //255;
     this->CW_FREQ = 1200;
 }

 /**
  * Indicates whether the repeater is currently keyed up or not
  *
  * @return True indicates currently transmitting
  */
 bool_t Repeater::IsTransmit() {
     return this->repeaterIsTransmitting;
 }

 /**
  * Method to switch the audio gates
  *
  * @param input Enumated type indicating which audio source should be connected
  *              to the downlink radio
  */
 void Repeater::AudioControl(AUDIO_CONTROL input) {
     switch (input) {
         case CPU_AUDIO:
             IOPorts::AudioSW1(true);
             IOPorts::AudioSW2(false);
             IOPorts::AudioSW3(false);
             break;

         case PACKET_AUDIO:
             IOPorts::AudioSW1(false);
             IOPorts::AudioSW2(true);
             IOPorts::AudioSW3(false);
             break;

         case UPLINK_RADIO:
             IOPorts::AudioSW1(false);
             IOPorts::AudioSW2(false);
             IOPorts::AudioSW3(true);
             break;
         case NONE:
             IOPorts::AudioSW1(false);
             IOPorts::AudioSW2(false);
             IOPorts::AudioSW3(false);
             break;
     }

 }

 /**
  * Called every 100 ms to update the repeater's status
  */
 void Repeater::Update() {
     bool_t txTimedOut = (SystemControl::GetInstance()->GetTick() > txTimeoutTick) ? true : false;

     // Check to see if the repeater is keyed up
     if(!IOPorts::GetCarrierDet() && !txTimedOut && !AFSK::GetInstance()->IsTransmit()) {
         // Setup the audio routing
         AudioControl(this->UPLINK_RADIO);

         // key up the downlink radio
         IOPorts::RadioPTT(true);
         repeaterIsTransmitting = true;
     }
     else {
         // Check to see if we need to ID
        if(SystemControl::GetInstance()->GetTick() > repeaterIDtick && !AFSK::GetInstance()->IsTransmit()) {
            this->repeaterIDtick = SystemControl::GetInstance()->GetTick() + repeaterIDPeriod;
            SendID();
        }
        if(repeaterIsTransmitting) {
            this->CourteseyBeep();

            IOPorts::RadioPTT(false);
            AudioControl(this->NONE);
            repeaterIsTransmitting = false;
        }

        if(IOPorts::GetCarrierDet())
            // update the TX timeout tick if the uplink radio is squelched
            txTimeoutTick = SystemControl::GetInstance()->GetTick() + maxTxDuration;
     }
 }

 /*
  * Send the repeater's CW ID
  */
 void Repeater::SendID() {
     // Setup the audio routing
     AudioControl(this->CPU_AUDIO);

     // Key up the downlink radio
     IOPorts::RadioPTT(true);

     // K
     this->CWDash();
     this->CWDot();
     this->CWDash();
     SystemControl::GetInstance()->Sleep(CW_INTER_CHAR_TIME);

     // A
     this->CWDot();
     this->CWDash();
     SystemControl::GetInstance()->Sleep(CW_INTER_CHAR_TIME);

     // 7
     this->CWDash();
     this->CWDash();
     this->CWDot();
     this->CWDot();
     this->CWDot();
     SystemControl::GetInstance()->Sleep(CW_INTER_CHAR_TIME);

     // N
     this->CWDash();
     this->CWDot();
     SystemControl::GetInstance()->Sleep(CW_INTER_CHAR_TIME);

     // S
     this->CWDot();
     this->CWDot();
     this->CWDot();
     SystemControl::GetInstance()->Sleep(CW_INTER_CHAR_TIME);

     // R
     this->CWDot();
     this->CWDash();
     this->CWDot();
     SystemControl::GetInstance()->Sleep(CW_INTER_CHAR_TIME);

     // /
     this->CWDash();
     this->CWDot();
     this->CWDot();
     this->CWDash();
     this->CWDot();
     SystemControl::GetInstance()->Sleep(CW_INTER_CHAR_TIME);

     // R
     this->CWDot();
     this->CWDash();
     this->CWDot();
     SystemControl::GetInstance()->Sleep(CW_INTER_CHAR_TIME);

     this->CourteseyBeep();

     IOPorts::RadioPTT(false);
 }

 /**
  * Geneate the repeater's courtesey beep
  */
 void Repeater::CourteseyBeep() {
     // Setup the audio routing
     AudioControl(this->CPU_AUDIO);
     SystemControl::GetInstance()->Sleep(700);
     ToneGenerator::GetInstance()->SingleTone(330, 100);
     ToneGenerator::GetInstance()->SingleTone(495, 100);
     ToneGenerator::GetInstance()->SingleTone(660, 100);
     SystemControl::GetInstance()->Sleep(CW_INTER_CHAR_TIME);
 }

 void Repeater::DebounceCarrierDet() {

 }

/**
 * Generates a single CW 'dot'
 */
void Repeater::CWDot() {
    ToneGenerator::GetInstance()->SingleTone(CW_FREQ, this->DOT);
    SystemControl::GetInstance()->Sleep(this->CW_INTER_SYMBOL_TIME);
}

/**
 * Generates a single CW 'dash'
 */
void Repeater::CWDash() {
    ToneGenerator::GetInstance()->SingleTone(CW_FREQ, this->DASH);
    SystemControl::GetInstance()->Sleep(this->CW_INTER_SYMBOL_TIME);
}
