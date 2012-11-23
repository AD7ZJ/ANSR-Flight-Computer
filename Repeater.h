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
 * Filename:     Repeater.h                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef REPEATER_H
#define REPEATER_H

/**
 *  @defgroup library Generic Library Functions
 *
 *  @{
 */

/**
*   Object that manages audio tone generation using the DAC.
*/
class Repeater
{

public:
    /// Controls which audio will be routed to the downlink radio
    typedef enum {
        /// Use the CPU's DAC as the audio source
        CPU_AUDIO,

        /// Use the onboard modem as the audio source
        PACKET_AUDIO,

        /// Use the uplink radio as the audio source
        UPLINK_RADIO,

        /// all switches set to off
        NONE
    } AUDIO_CONTROL;

    Repeater();
    static Repeater * GetInstance();
    void Update();
    void DebounceCarrierDet();
    void AudioControl(AUDIO_CONTROL input);


private:
    void SendCwID();
    void CourteseyBeep();

    bool_t debouncedCD;
};

/** @} */

#endif // #ifndef REPEATER_H
