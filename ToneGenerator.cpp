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
 * Filename:     ToneGenerator.cpp                                         *
 *                                                                         *
 ***************************************************************************/

#include "main.h"

 const uint16_t  ToneGenerator::sineTable[] = {
            0x2C3, 0x369, 0x3D8, 0x3FF,
            0x3D8, 0x369, 0x2C3, 0x200,
            0x13C, 0x096, 0x027, 0x000,
            0x027, 0x096, 0x13C, 0x200
 };

/**
 * Constructor
 */
 ToneGenerator::ToneGenerator() {
     Timer0::GetInstance()->EnableWithoutInt();
     // default to give a 1 kHz tone
     Timer0::GetInstance()->SetPeriod(63);
 }


 /**
  * Generate a single-frequency tone for the specified duration
  *
  * @param freq Frequency in Hz
  * @param duration Duration in tenths of a second
  */
 void ToneGenerator::SingleTone(uint16_t freq, uint8_t duration) {
     // figure out the timer period
     uint32_t periodus = 1000000 / (freq * 16);
     uint32_t durationus = duration * 100000;
     uint32_t accumDur = 0;
     Timer0::GetInstance()->SetPeriod(periodus);

     int index = 0;
     while(accumDur < durationus) {
         // spin until the timer clicks
         while(T0TCR & 0x01);
         // restart the timer
         T0TCR = TxTCR_COUNTER_ENABLE;
         DAC::GetInstance()->Set(sineTable[index++]);
         index &= 0x0F;
         accumDur += periodus;
     }
 }

