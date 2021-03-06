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
 * Filename:     AFSK.h                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef AFSK_H
#define AFSK_H

/**
 *  @defgroup application Flight Computer Application
 *
 *  @{
 */

/**
 *   Generate the 1200-BPS, A-FSK signal using the asynchronous MODEM.
 */
class AFSK
{
    public:
        AFSK();

        bool_t IsTransmit();
        void Transmit (const char *text, const char *destAddress = NULL);
        void Update();
        
        void NormalPacketSrc();
        void PredictPacketSrc();

        static AFSK *GetInstance();
        
    private:
        /// AX.25 object that generates and holds the message stream.
        AX25 ax25;
        
        /// CMX867A MODEM control object.
        CMX867A *modem;
        
        /// Flag that indicates the transmit operation is in progress.
        bool_t txFlag;
        
        /// Index to current transmit byte.
        uint32_t messageIndex;

        /// Flag that indicates all the data has been sent.
        bool_t txDoneFlag;

        /// Flag that indicates there is an AX25 stream waiting to be sent
        bool_t txDataQueued;
};

/** @} */


#endif  // #ifndef AFSK_H
