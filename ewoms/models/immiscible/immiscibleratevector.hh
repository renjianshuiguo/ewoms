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
 * \copydoc Ewoms::ImmiscibleRateVector
 */
#ifndef EWOMS_IMMISCIBLE_RATE_VECTOR_HH
#define EWOMS_IMMISCIBLE_RATE_VECTOR_HH

#include <dune/common/fvector.hh>

#include <opm/common/valgrind.hh>
#include <opm/material/constraintsolvers/ncpflash.hh>

#include "immisciblevolumevariables.hh"

namespace Ewoms
{
/*!
 * \ingroup ImmiscibleModel
 *
 * \brief Implements a vector representing rates of conserved quantities.
 *
 * This class is basically a Dune::FieldVector which can be set using
 * either mass, molar or volumetric rates.
 */
template <class TypeTag>
class ImmiscibleRateVector
    : public Dune::FieldVector<typename GET_PROP_TYPE(TypeTag, Scalar),
                               GET_PROP_VALUE(TypeTag, NumEq) >
{
    typedef typename GET_PROP_TYPE(TypeTag, Scalar) Scalar;
    enum { numEq = GET_PROP_VALUE(TypeTag, NumEq) };
    typedef Dune::FieldVector<Scalar, numEq> ParentType;

    typedef typename GET_PROP_TYPE(TypeTag, FluidSystem) FluidSystem;
    typedef typename GET_PROP_TYPE(TypeTag, Indices) Indices;

    enum { conti0EqIdx = Indices::conti0EqIdx };
    enum { numComponents = GET_PROP_VALUE(TypeTag, NumComponents) };
    enum { enableEnergy = GET_PROP_VALUE(TypeTag, EnableEnergy) };

    typedef VcfvEnergyModule<TypeTag, enableEnergy> EnergyModule;

public:
    /*!
     * \brief Default constructor
     */
    ImmiscibleRateVector()
        : ParentType()
    { Valgrind::SetUndefined(*this); }

    /*!
     * \brief Constructor with assignment from scalar
     *
     * \param value The scalar value to which all entries of the vector will be set.
     */
    ImmiscibleRateVector(Scalar value)
        : ParentType(value)
    { }

    /*!
     * \brief Copy constructor
     *
     * \param value The rate vector that will be duplicated.
     */
    ImmiscibleRateVector(const ImmiscibleRateVector &value)
        : ParentType(value)
    { }

    /*!
     * \brief Set a mass rate of the conservation quantities.
     *
     * Enthalpy is _not_ taken into account seperately here. This
     * means that it must be set to the desired value in the
     * parameter.
     *
     * \param value The mass rate in \f$[kg/(m^2\,s)]\f$ (unit for areal fluxes)
     */
    void setMassRate(const ParentType &value)
    { ParentType::operator=(value); }

    /*!
     * \brief Set a molar rate of the conservation quantities.
     *
     * Enthalpy is _not_ taken into account seperately here. This
     * means that it must be set to the desired value in the
     * parameter.
     *
     * \param value The molar rate in \f$[mol/(m^2\,s)]\f$ (unit for areal fluxes)
     */
    void setMolarRate(const ParentType &value)
    {
        // convert to mass rates
        ParentType massRate(value);
        for (int compIdx = 0; compIdx < numComponents; ++compIdx)
            massRate[conti0EqIdx + compIdx] *= FluidSystem::molarMass(compIdx);

        // set the mass rate
        setMassRate(massRate);
    }

    /*!
     * \brief Set an enthalpy rate [J/As] where \f$A \in \{m^2, m^3\}\f$
     *
     * If the energy equation is not enabled, this method is a no-op.
     *
     * \param rate The enthalpy rate in \f$[J/(m^2\,s)]\f$ (unit for areal fluxes)
     */
    void setEnthalpyRate(Scalar rate)
    { EnergyModule::setEnthalpyRate(*this, rate); }

    /*!
     * \brief Set a volumetric rate of a phase.
     *
     * The enthalpy transported into the domain is taken into account
     * by this method.
     *
     * \param fluidState The thermodynamic state of the fluids which
     *                   should be considered. The density and the
     *                   composition of the considered phase must be
     *                   specified before calling this method.
     * \param phaseIdx The index of the fluid phase for which the
     *                 given amount of volume should be specified.
     * \param volume The volumetric rate of the fluid phase in
     *               \f$[m^3/(m^2\,s)]\f$ (unit for areal fluxes)
     */
    template <class FluidState>
    void setVolumetricRate(const FluidState &fluidState,
                           int phaseIdx,
                           Scalar volume)
    {
        for (int compIdx = 0; compIdx < numComponents; ++compIdx)
            (*this)[conti0EqIdx + compIdx] =
                fluidState.density(phaseIdx, compIdx)
                * fluidState.massFraction(phaseIdx, compIdx)
                * volume;

        EnergyModule::setEnthalpyRate(*this, fluidState, phaseIdx, volume);
    }
};

} // end namepace

#endif
