// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*****************************************************************************
 *   Copyright (C) 2008-2012 by Andreas Lauser                               *
 *                                                                           *
 *   This program is free software: you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation, either version 2 of the License, or       *
 *   (at your option) any later version.                                     *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU General Public License for more details.                            *
 *                                                                           *
 *   You should have received a copy of the GNU General Public License       *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 *****************************************************************************/
/*!
 * \file
 *
 * \copydoc Ewoms::RichardsIndices
 */
#ifndef EWOMS_RICHARDS_INDICES_HH
#define EWOMS_RICHARDS_INDICES_HH

namespace Ewoms {

/*!
 * \ingroup RichardsModel
 * \brief Indices for the primary variables/conservation equations of the Richards model.
 */
struct RichardsIndices
{
    //////////
    // primary variable indices
    //////////

    //! Primary variable index for the wetting phase pressure
    static const int pressureWIdx = 0;

    //////////
    // equation indices
    //////////
    //! Equation index for the mass conservation of the wetting phase
    static const int contiWEqIdx = 0;
};

} // end namepace

#endif
