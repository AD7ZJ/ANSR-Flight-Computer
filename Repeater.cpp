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
     debouncedCD = false;
 }

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
  *
  */
 void Repeater::Update() {
     if(!IOPorts::GetCarrierDet()) {
         // Setup the audio routing
         AudioControl(this->UPLINK_RADIO);

         // key up the downlink radio
         IOPorts::RadioPTT(true);
     }
     else {
         IOPorts::RadioPTT(false);
         AudioControl(this->NONE);
     }

 }

 void Repeater::DebounceCarrierDet() {


 }

