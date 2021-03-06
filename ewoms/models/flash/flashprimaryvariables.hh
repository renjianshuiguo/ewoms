// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*****************************************************************************
 *   Copyright (C) 2011-2012 by Andreas Lauser                               *
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
 * \copydoc Ewoms::FlashPrimaryVariables
 */
#ifndef EWOMS_FLASH_PRIMARY_VARIABLES_HH
#define EWOMS_FLASH_PRIMARY_VARIABLES_HH

#include <dune/common/fvector.hh>

#include <ewoms/models/modules/energy/vcfvenergymodule.hh>
#include <opm/material/constraintsolvers/ncpflash.hh>
#include <opm/material/fluidstates/compositionalfluidstate.hh>

#include "flashindices.hh"
#include "flashproperties.hh"

#include <iostream>

namespace Ewoms {

/*!
 * \ingroup FlashModel
 *
 * \brief Represents the primary variables used by the compositional
 *        flow model based on flash calculations.
 *
 * This class is basically a Dune::FieldVector which can retrieve its
 * contents from an aribitatry fluid state.
 */
template <class TypeTag>
class FlashPrimaryVariables
    : public Dune::FieldVector<typename GET_PROP_TYPE(TypeTag, Scalar),
                               GET_PROP_VALUE(TypeTag, NumEq) >
{
    typedef typename GET_PROP_TYPE(TypeTag, Scalar) Scalar;
    enum { numEq = GET_PROP_VALUE(TypeTag, NumEq) };
    typedef Dune::FieldVector<Scalar, numEq> ParentType;
    typedef typename GET_PROP_TYPE(TypeTag, PrimaryVariables) PrimaryVariables;

    typedef typename GET_PROP_TYPE(TypeTag, MaterialLawParams) MaterialLawParams;
    typedef typename GET_PROP_TYPE(TypeTag, FluidSystem) FluidSystem;
    typedef typename GET_PROP_TYPE(TypeTag, Indices) Indices;

    // primary variable indices
    enum { cTot0Idx = Indices::cTot0Idx };

    enum { numPhases = GET_PROP_VALUE(TypeTag, NumPhases) };
    enum { numComponents = GET_PROP_VALUE(TypeTag, NumComponents) };
    typedef VcfvEnergyModule<TypeTag, GET_PROP_VALUE(TypeTag, EnableEnergy)> EnergyModule;

public:
    FlashPrimaryVariables()
        : ParentType()
    { Valgrind::SetDefined(*this); }

    /*!
     * \copydoc ImmisciblePrimaryVariables::ImmisciblePrimaryVariables(Scalar)
     */
    FlashPrimaryVariables(Scalar value)
        : ParentType(value)
    {
        Valgrind::CheckDefined(value);
        Valgrind::SetDefined(*this);
    }

    /*!
     * \copydoc ImmisciblePrimaryVariables::ImmisciblePrimaryVariables(const ImmisciblePrimaryVariables &)
     */
    FlashPrimaryVariables(const FlashPrimaryVariables &value)
        : ParentType(value)
    { Valgrind::SetDefined(*this); }

    /*!
     * \copydoc ImmisciblePrimaryVariables::assignMassConservative
     */
    template <class FluidState>
    void assignMassConservative(const FluidState &fluidState,
                                const MaterialLawParams &matParams,
                                bool isInEquilibrium = false)
    {
        // there is no difference between naive and mass conservative
        // assignment in the flash model. (we only need the total
        // concentrations.)
        assignNaive(fluidState);
    }

    /*!
     * \copydoc ImmisciblePrimaryVariables::assignNaive
     */
    template <class FluidState>
    void assignNaive(const FluidState &fluidState)
    {
        // reset everything
        (*this) = 0.0;

        // assign the phase temperatures. this is out-sourced to
        // the energy module
        EnergyModule::setPriVarTemperatures(*this, fluidState);

        // determine the phase presence.
        for (int phaseIdx = 0; phaseIdx < numPhases; ++ phaseIdx) {
            for (int compIdx = 0; compIdx < numComponents; ++ compIdx) {
                this->operator[](cTot0Idx + compIdx) +=
                    fluidState.molarity(phaseIdx,compIdx)
                    * fluidState.saturation(phaseIdx);
            }
        }
    }

    /*!
     * \brief Prints the names of the primary variables and their values.
     *
     * \param os The \c std::ostream which should be used for the output.
     */
    void print(std::ostream &os = std::cout) const
    {
        for (int compIdx = 0; compIdx < numComponents; ++ compIdx) {
            os << "(c_tot," << FluidSystem::componentName(compIdx)
               << " = " << this->operator[](cTot0Idx + compIdx);
        };
        os << ")";
    }
};

} // end namepace

#endif
