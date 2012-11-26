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
 * Filename:     ToneGenerator.h                                           *
 *                                                                         *
 ***************************************************************************/

#ifndef TONEGENERATOR_H
#define TONEGENERATOR_H

/**
 *  @defgroup library Generic Library Functions
 *
 *  @{
 */

/**
*   Object that manages audio tone generation using the DAC.
*/
class ToneGenerator
{

public:
    ToneGenerator();
    void SingleTone(uint16_t freq, uint8_t duration);
    static ToneGenerator * GetInstance();

private:
    static const uint16_t sineTable[16];
    volatile uint8_t sinIndex;
};

/** @} */

#endif // #ifndef TONEGENERATOR_H
