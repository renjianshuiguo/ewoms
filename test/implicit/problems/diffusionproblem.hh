// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*****************************************************************************
 *   Copyright (C) 2012 by Andreas Lauser                                    *
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
 * \copydoc Ewoms::DiffusionProblem
 */
#ifndef EWOMS_POWER_INJECTION_PROBLEM_HH
#define EWOMS_POWER_INJECTION_PROBLEM_HH

#include <ewoms/models/ncp/ncpproperties.hh>
#include <opm/material/fluidmatrixinteractions/mp/mplinearmaterial.hh>
#include <opm/material/fluidsystems/h2on2fluidsystem.hh>
#include <opm/material/fluidstates/compositionalfluidstate.hh>
#include <opm/material/constraintsolvers/computefromreferencephase.hh>
#include <ewoms/io/cubegridcreator.hh>

#include <dune/common/fvector.hh>
#include <dune/common/fmatrix.hh>

#include <sstream>
#include <string>

namespace Ewoms {

template <class TypeTag>
class DiffusionProblem;

//////////
// Specify the properties for the powerInjection problem
//////////
namespace Properties {

NEW_TYPE_TAG(DiffusionBaseProblem);

// Set the grid implementation to be used
SET_TYPE_PROP(DiffusionBaseProblem, Grid, Dune::YaspGrid</*dim=*/1>);

// set the GridCreator property
SET_TYPE_PROP(DiffusionBaseProblem, GridCreator, CubeGridCreator<TypeTag>);

// Set the problem property
SET_TYPE_PROP(DiffusionBaseProblem, Problem, Ewoms::DiffusionProblem<TypeTag>);

// Set the fluid system
SET_PROP(DiffusionBaseProblem, FluidSystem)
{
private:
    typedef typename GET_PROP_TYPE(TypeTag, Scalar) Scalar;

public:
    typedef Opm::FluidSystems::H2ON2<Scalar> type;
};

// Set the material Law
SET_PROP(DiffusionBaseProblem, MaterialLaw)
{
private:
    typedef typename GET_PROP_TYPE(TypeTag, Scalar) Scalar;
    typedef typename GET_PROP_TYPE(TypeTag, FluidSystem) FluidSystem;

public:
    typedef Opm::MpLinearMaterial<FluidSystem::numPhases, Scalar> type;
};

// Enable molecular diffusion for this problem
SET_BOOL_PROP(DiffusionBaseProblem, EnableDiffusion, true);

// Disable gravity
SET_BOOL_PROP(DiffusionBaseProblem, EnableGravity, false);

// define the properties specific for the diffusion problem
SET_SCALAR_PROP(DiffusionBaseProblem, DomainSizeX, 1.0);
SET_SCALAR_PROP(DiffusionBaseProblem, DomainSizeY, 1.0);
SET_SCALAR_PROP(DiffusionBaseProblem, DomainSizeZ, 1.0);

SET_INT_PROP(DiffusionBaseProblem, CellsX, 250);
SET_INT_PROP(DiffusionBaseProblem, CellsY, 1);
SET_INT_PROP(DiffusionBaseProblem, CellsZ, 1);

// The default for the end time of the simulation
SET_SCALAR_PROP(DiffusionBaseProblem, EndTime, 1e6);

// The default for the initial time step size of the simulation
SET_SCALAR_PROP(DiffusionBaseProblem, InitialTimeStepSize, 1000);
}

/*!
 * \ingroup VcfvTestProblems
 * \brief 1D problem which is driven by molecular diffusion.
 *
 * The domain is one meter long and completely filled with gas and
 * closed on all boundaries. Its left half exhibits a slightly higher
 * water concentration than the right one. After a while, the
 * concentration of water will be equilibrate due to molecular
 * diffusion.
 */
template <class TypeTag>
class DiffusionProblem
    : public GET_PROP_TYPE(TypeTag, BaseProblem)
{
    typedef typename GET_PROP_TYPE(TypeTag, BaseProblem) ParentType;

    typedef typename GET_PROP_TYPE(TypeTag, Scalar) Scalar;
    typedef typename GET_PROP_TYPE(TypeTag, GridView) GridView;
    typedef typename GET_PROP_TYPE(TypeTag, FluidSystem) FluidSystem;
    typedef typename GET_PROP_TYPE(TypeTag, PrimaryVariables) PrimaryVariables;
    typedef typename GET_PROP_TYPE(TypeTag, TimeManager) TimeManager;

    enum {
        // number of phases
        numPhases = FluidSystem::numPhases,

        // phase indices
        lPhaseIdx = FluidSystem::lPhaseIdx,
        gPhaseIdx = FluidSystem::gPhaseIdx,

        // component indices
        H2OIdx = FluidSystem::H2OIdx,
        N2Idx = FluidSystem::N2Idx,

        // Grid and world dimension
        dim = GridView::dimension,
        dimWorld = GridView::dimensionworld
    };

    typedef typename GET_PROP_TYPE(TypeTag, RateVector) RateVector;
    typedef typename GET_PROP_TYPE(TypeTag, BoundaryRateVector) BoundaryRateVector;

    typedef typename GET_PROP_TYPE(TypeTag, MaterialLaw) MaterialLaw;
    typedef typename GET_PROP_TYPE(TypeTag, MaterialLawParams) MaterialLawParams;

    typedef typename GridView::ctype CoordScalar;
    typedef Dune::FieldVector<CoordScalar, dimWorld> GlobalPosition;

    typedef Dune::FieldMatrix<Scalar, dimWorld, dimWorld> DimMatrix;

public:
    /*!
     * \copydoc Doxygen::defaultProblemConstructor
     */
    DiffusionProblem(TimeManager &timeManager)
        : ParentType(timeManager, GET_PROP_TYPE(TypeTag, GridCreator)::grid().leafView())
    {
        FluidSystem::init();

        temperature_ = 273.15 + 20.0;

        K_ = this->toDimMatrix_(1e-12); // [m^2]

        setupInitialFluidStates_();
    }

    /*!
     * \name Auxiliary methods
     */
    //! \{

    /*!
     * \copydoc VcfvProblem::name
     */
    const std::string name() const
    {
        std::ostringstream oss;
        oss << "diffusion_" << this->model().name();
        return oss.str();
    }

    //! \}

    /*!
     * \name Soil parameters
     */
    //! \{

    /*!
     * \copydoc VcfvMultiPhaseProblem::intrinsicPermeability
     */
    template <class Context>
    const DimMatrix &intrinsicPermeability(const Context &context, int spaceIdx, int timeIdx) const
    { return K_; }

    /*!
     * \copydoc VcfvMultiPhaseProblem::porosity
     */
    template <class Context>
    Scalar porosity(const Context &context, int spaceIdx, int timeIdx) const
    { return 0.35; }

    /*!
     * \copydoc VcfvMultiPhaseProblem::materialLawParams
     */
    template <class Context>
    const MaterialLawParams& materialLawParams(const Context &context, int spaceIdx, int timeIdx) const
    { return materialParams_; }

    /*!
     * \copydoc VcfvMultiPhaseProblem::temperature
     */
    template <class Context>
    Scalar temperature(const Context &context,
                       int spaceIdx, int timeIdx) const
    { return temperature_; }

    //! \}

    /*!
     * \name Boundary conditions
     */
    //! \{

    /*!
     * \copydoc VcfvProblem::boundary
     *
     * This problem sets no-flow boundaries everywhere.
     */
    template <class Context>
    void boundary(BoundaryRateVector &values,
                  const Context &context,
                  int spaceIdx, int timeIdx) const
    { values.setNoFlow(); }

    //! \}

    /*!
     * \name Volume terms
     */
    //! \{

    /*!
     * \copydoc VcfvProblem::initial
     */
    template <class Context>
    void initial(PrimaryVariables &values,
                 const Context &context,
                 int spaceIdx, int timeIdx) const
    {
        const auto &pos = context.pos(spaceIdx, timeIdx);
        if (onLeftSide_(pos))
            values.assignNaive(leftInitialFluidState_);
        else
            values.assignNaive(rightInitialFluidState_);
    }


    /*!
     * \copydoc VcfvProblem::source
     *
     * For this problem, the source term of all components is 0
     * everywhere.
     */
    template <class Context>
    void source(RateVector &rate,
                const Context &context,
                int spaceIdx, int timeIdx) const
    { rate = Scalar(0.0); }

    //! \}

private:
    bool onLeftSide_(const GlobalPosition &pos) const
    { return pos[0] < (this->bboxMin()[0] + this->bboxMax()[0])/2; }

    void setupInitialFluidStates_()
    {
        // create the initial fluid state for the left half of the domain
        leftInitialFluidState_.setTemperature(temperature_);

        Scalar Sl = 0.0;
        leftInitialFluidState_.setSaturation(lPhaseIdx, Sl);
        leftInitialFluidState_.setSaturation(gPhaseIdx, 1 - Sl);

        Scalar p = 1e5;
        leftInitialFluidState_.setPressure(lPhaseIdx, p);
        leftInitialFluidState_.setPressure(gPhaseIdx, p);

        Scalar xH2O = 0.01;
        leftInitialFluidState_.setMoleFraction(gPhaseIdx, H2OIdx, xH2O);
        leftInitialFluidState_.setMoleFraction(gPhaseIdx, N2Idx, 1 - xH2O);

        typedef Opm::ComputeFromReferencePhase<Scalar, FluidSystem> CFRP;
        typename FluidSystem::ParameterCache paramCache;
        CFRP::solve(leftInitialFluidState_, paramCache, gPhaseIdx, /*setViscosity=*/false, /*setEnthalpy=*/false);

        // create the initial fluid state for the right half of the domain
        rightInitialFluidState_.assign(leftInitialFluidState_);
        xH2O = 0.0;
        rightInitialFluidState_.setMoleFraction(gPhaseIdx, H2OIdx, xH2O);
        rightInitialFluidState_.setMoleFraction(gPhaseIdx, N2Idx, 1 - xH2O);
        CFRP::solve(rightInitialFluidState_, paramCache, gPhaseIdx, /*setViscosity=*/false, /*setEnthalpy=*/false);

    }

    DimMatrix K_;
    MaterialLawParams materialParams_;

    Opm::CompositionalFluidState<Scalar, FluidSystem> leftInitialFluidState_;
    Opm::CompositionalFluidState<Scalar, FluidSystem> rightInitialFluidState_;
    Scalar temperature_;
};

} //end namespace

#endif
