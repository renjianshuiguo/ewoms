/*****************************************************************************
 *   See the file COPYING for full copying permissions.                      *
 *                                                                           *
 *   This program is free software: you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation, either version 2 of the License, or       *
 *   (at your option) any later version.                                     *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   GNU General Public License for more details.                            *
 *                                                                           *
 *   You should have received a copy of the GNU General Public License       *
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/
/*!
 * \file
 *
 * \brief Two phase flow in fractured porous media problem:
 *  Soil contamination problem where a DNAPL migrates in a fully water saturated
 *  fractured porous medium.
 */

#ifndef EWOMS_TEST_2PDFM_TEST_PROBLEM_HH
#define EWOMS_TEST_2PDFM_TEST_PROBLEM_HH

#if HAVE_UG
#include <dune/grid/uggrid.hh>
#endif

#include <dune/grid/yaspgrid.hh>

#include <dumux/boxmodels/2pdfm/2pdfmmodel.hh>
#include <dumux/boxmodels/common/porousmediaboxproblem.hh>
#include <dumux/material/components/simpleh2o.hh>
#include <dumux/material/components/dnapl.hh>
#include <dumux/material/fluidsystems/h2on2fluidsystem.hh>

#include "2pdfmspatialparams.hh"

namespace Ewoms
{

template <class TypeTag>
class TwoPDFMTestProblem;

namespace Properties
{
NEW_TYPE_TAG(TwoPDFMTestProblem, INHERITS_FROM(BoxTwoPDFM, TwoPDFMSpatialParameters));

// Set the grid type
#if HAVE_UG
SET_TYPE_PROP(TwoPDFMTestProblem, Grid, Dune::UGGrid<2>);
#else
SET_TYPE_PROP(TwoPDFMTestProblem, Grid, Dune::YaspGrid<2>);
#endif

// Set the problem property
SET_TYPE_PROP(TwoPDFMTestProblem, Problem, Ewoms::TwoPDFMTestProblem<TypeTag>);

// Set the wetting phase
SET_PROP(TwoPDFMTestProblem, WettingPhase)
{
private:
    typedef typename GET_PROP_TYPE(TypeTag, Scalar) Scalar;
public:
    typedef Ewoms::LiquidPhase<Scalar, Ewoms::SimpleH2O<Scalar> > type;
};

// Set the non-wetting phase
SET_PROP(TwoPDFMTestProblem, NonwettingPhase)
{
private:
    typedef typename GET_PROP_TYPE(TypeTag, Scalar) Scalar;
public:
    typedef Ewoms::LiquidPhase<Scalar, Ewoms::DNAPL<Scalar> > type;
};

// Enable partial reassembly of the jacobian matrix?
SET_BOOL_PROP(TwoPDFMTestProblem, ImplicitEnablePartialReassemble, true);

// Enable reuse of jacobian matrices?
SET_BOOL_PROP(TwoPDFMTestProblem, ImplicitEnableJacobianRecycling, true);

// Write the solutions of individual newton iterations?
SET_BOOL_PROP(TwoPDFMTestProblem, NewtonWriteConvergence, false);

// Use forward differences instead of central differences
SET_INT_PROP(TwoPDFMTestProblem, ImplicitNumericDifferenceMethod, +1);

// Linear solver settings
SET_TYPE_PROP(TwoPDFMTestProblem, LinearSolver, Ewoms::BoxBiCGStabILU0Solver<TypeTag>);
SET_INT_PROP(TwoPDFMTestProblem, LinearSolverVerbosity, 0);
SET_INT_PROP(TwoPDFMTestProblem, LinearSolverPreconditionerIterations, 1);
SET_SCALAR_PROP(TwoPDFMTestProblem, LinearSolverPreconditionerRelaxation, 1.0);

// Enable gravity
SET_BOOL_PROP(TwoPDFMTestProblem, ProblemEnableGravity, false);
}

/*!
 * \ingroup TwoPDFMBoxModel
 * \ingroup BoxTestProblems
 * \brief Soil contamination problem involving a DNAPL migration into a
 *        fully water saturated media
 *
 * This problem uses the \ref TwoPDFMModel.
 *
 * This problem should typically be simulated until \f$t_{\text{end}}
 * \approx 100\,000\;s\f$ is reached. A good choice for the initial time step
 * size is \f$t_{\text{inital}} = 1\;s\f$.
 *
 * To run the simulation execute the following line in shell:
 * <tt>./test_2pDFM grids/defaultgrid.net 1e6 1</tt>
 */
template <class TypeTag >
class TwoPDFMTestProblem : public PorousMediaBoxProblem<TypeTag>
{
    typedef PorousMediaBoxProblem<TypeTag> ParentType;
    typedef typename GET_PROP_TYPE(TypeTag, GridView) GridView;

    typedef typename GET_PROP_TYPE(TypeTag, Indices) Indices;

    typedef typename GET_PROP_TYPE(TypeTag, FluidSystem) FluidSystem;
    typedef typename GET_PROP_TYPE(TypeTag, WettingPhase) WettingPhase;
    typedef typename GET_PROP_TYPE(TypeTag, NonwettingPhase) NonwettingPhase;

    enum {
        // primary variable indices
        pwIdx = Indices::pwIdx,
        SnIdx = Indices::SnIdx,

        // equation indices
        contiNEqIdx = Indices::contiNEqIdx,

        // phase indices
        wPhaseIdx = Indices::wPhaseIdx,
        nPhaseIdx = Indices::nPhaseIdx,

        // Grid and world dimension
        dim = GridView::dimension,
        dimWorld = GridView::dimensionworld
    };

    typedef typename GridView::template Codim<0>::Entity Element;
    typedef typename GridView::template Codim<dim>::Entity Vertex;
    typedef typename GridView::Intersection Intersection;
    typedef typename GridView::template Codim<0>::Iterator ElementIterator;

    typedef typename GET_PROP_TYPE(TypeTag, PrimaryVariables) PrimaryVariables;
    typedef typename GET_PROP_TYPE(TypeTag, BoundaryTypes) BoundaryTypes;
    typedef typename GET_PROP_TYPE(TypeTag, TimeManager) TimeManager;
    typedef typename GridType::ctype DT;

    typedef typename GET_PROP_TYPE(TypeTag, Scalar) Scalar;
    typedef Dune::FieldVector<Scalar, dimWorld> GlobalPosition;

public:
    /*!
     * \brief The constructor
     *
     * \param timeManager The time manager
     * \param gridView The grid view
     * \param isDuneFractureVertex Vector of bools which indicates fracture vertices
     * \param isDuneFractureEdge Vector of bools which indicates fracture edges
     * \param fractureEdgesIdx Vector of edge indices which are fractures
     */
    TwoPDFMTestProblem(TimeManager &timeManager,
                       const GridView &gridView,
                       std::vector<bool>& isDuneFractureVertex,
                       std::vector<bool>& isDuneFractureEdge,
                       std::vector<int>& fractureEdgesIdx)
        : ParentType(timeManager, gridView),
          isDuneFractureVertex_(isDuneFractureVertex),
          isDuneFractureEdge_(isDuneFractureEdge),
          fractureEdgesIdx_(fractureEdgesIdx),
          useInterfaceCondition_(true)
    {
        eps_ = 3e-6;
        temperature_ = 273.15 + 20; // -> 20°C

        inactivateFractures_ = false;
        this->spatialParams().setGridView(gridView);
        this->spatialParams().setFractureBoolVectors(isDuneFractureVertex_,
                isDuneFractureEdge_, fractureEdgesIdx_, inactivateFractures_);
        bboxMin_ = this->bboxMin();
        bboxMax_ = this->bboxMax();
    }

    /*!
     * \name Problem parameters
     */
    // \{

    /*!
     * \brief The problem name.
     *
     * This is used as a prefix for files generated by the simulation.
     */
    const char *name() const
    { return "2pdfm"; }

    /*!
     * \brief Called directly after the time integration.
     */
    void postTimeStep()
    {
        // Calculate storage terms
        PrimaryVariables storage;
        this->model().globalStorage(storage);

        // Write mass balance information for rank 0
        if (this->gridView().comm().rank() == 0)
        {
            std::cout << "Storage: " << storage << std::endl;
        }
    }

    /*!
     * \brief Returns the temperature within the domain.
     *
     * This problem assumes a uniform temperature of 10 degrees Celsius.
     */
    Scalar temperature() const
    { return temperature_; };

    /*!
     * \brief Returns the source within the domain.
     *
     * \param source Values of the source.
     * \param globalPos Global position
     */
    void sourceAtPos(PrimaryVariables &source,
                const GlobalPosition &globalPos) const
    {
        source = 0;
    }

    // \}

    /*!
     * \name Boundary conditions
     */
    // \{

    /*!
     * \brief Specifies which kind of boundary condition should be
     *        used for which equation on a given boundary control volume.
     *
     * \param values The boundary types for the conservation equations
     * \param globalPos The position of the center of the finite volume
     */
    void boundaryTypesAtPos(BoundaryTypes &values,
            const GlobalPosition &globalPos) const
    {
        if (onLeftBoundary_(globalPos) || onRightBoundary_(globalPos))
        {
            values.setAllDirichlet();
        }
        else
        {
            values.setAllNeumann();
        }
    }

    /*!
     * \brief Evaluate the boundary conditions for a Dirichlet
     *        control volume.
     *
     * For this method, the \a values parameter stores primary variables.
     *
     * \param values The dirichlet values for the primary variables
     * \param globalPos The center of the finite volume which ought to be set.
     */
    void dirichletAtPos(PrimaryVariables &values,
                        const GlobalPosition &globalPos) const
    {
        typename GET_PROP_TYPE(TypeTag, FluidState) fluidState;
        fluidState.setTemperature(temperature_);
        fluidState.setPressure(FluidSystem::wPhaseIdx, /*pressure=*/1e5);
        fluidState.setPressure(FluidSystem::nPhaseIdx, /*pressure=*/1e5);

        Scalar densityW = FluidSystem::density(fluidState, FluidSystem::wPhaseIdx);

        if (onLeftBoundary_(globalPos))
        {
            Scalar height = this->bboxMax()[1] - this->bboxMin()[1];
            Scalar depth = this->bboxMax()[1] - globalPos[1];
            Scalar alpha = (1 + 1.5/height);

            // hydrostatic pressure scaled by alpha
            values[pwIdx] = 2 - alpha*densityW*this->gravity()[1]*depth;
            values[SnIdx] = 1.0;
        }
        else if (onRightBoundary_(globalPos))
        {
            Scalar depth = this->bboxMax()[1] - globalPos[1];

            // hydrostatic pressure
            values[pwIdx] = 1 - densityW*this->gravity()[1]*depth;
            values[SnIdx] = 0.0;
        }
        else
        {
            values = 0.0;
        }
    }

    /*!
     * \brief Evaluate the boundary conditions for a neumann
     *        boundary segment.
     *
     * \param values The neumann values for the conservation equations [kg / (m^2 *s )]
     * \param globalPos The position of the integration point of the boundary segment.
     *
     * For this method, the \a values parameter stores the mass flux
     * in normal direction of each phase. Negative values mean influx.
     */
    void neumannAtPos(PrimaryVariables &values,
                      const GlobalPosition &globalPos) const
    {
        values = 0.0;
    }
    // \}

    /*!
     * \name Volume terms
     */
    // \{

    /*!
     * \brief Evaluate the initial value for a control volume.
     *
     * \param values The initial values for the primary variables
     * \param globalPos The center of the finite volume which ought to be set.
     *
     * For this method, the \a values parameter stores primary
     * variables.
     */
    void initialAtPos(PrimaryVariables &values,
                      const GlobalPosition &globalPos) const
    {
        Scalar depth = this->bboxMax()[1] - globalPos[1];

        typename GET_PROP_TYPE(TypeTag, FluidState) fluidState;
        fluidState.setTemperature(temperature_);
        fluidState.setPressure(FluidSystem::wPhaseIdx, /*pressure=*/1e5);
        fluidState.setPressure(FluidSystem::nPhaseIdx, /*pressure=*/1e5);

        Scalar densityW = FluidSystem::density(fluidState, FluidSystem::wPhaseIdx);

        // hydrostatic pressure
        values[pwIdx] = 1 - densityW*this->gravity()[1]*depth;
        values[SnIdx] = 0.0;
    }
    // \}
    
    /*!
     * \brief Whether the interface condition is used.
     */
    bool useInterfaceCondition() const
    {
        return useInterfaceCondition_;
    }

private:
    bool onLeftBoundary_(const GlobalPosition &globalPos) const
    {
        return globalPos[0] < this->bboxMin()[0] + eps_;
    }

    bool onRightBoundary_(const GlobalPosition &globalPos) const
    {
        return globalPos[0] > this->bboxMax()[0] - eps_;
    }

    bool onLowerBoundary_(const GlobalPosition &globalPos) const
    {
        return globalPos[1] < this->bboxMin()[1] + eps_;
    }

    bool onUpperBoundary_(const GlobalPosition &globalPos) const
    {
        return globalPos[1] > this->bboxMax()[1] - eps_;
    }

    bool onInlet_(const GlobalPosition &globalPos) const
    {
        Scalar width = this->bboxMax()[0] - this->bboxMin()[0];
        Scalar lambda = (this->bboxMax()[0] - globalPos[0])/width;
        return onUpperBoundary_(globalPos) && 0.5 < lambda && lambda < 2.0/3.0;
    }

    bool upperRightCorner(const GlobalPosition &globalPos) const
    {
        return ((globalPos[0]>bboxMax_[0] - eps_)
                &&(globalPos[1]>bboxMax_[1] - eps_));
    }
    
    /*!
     * \brief Checks whether the fracture intersects the boundary.
     *
     * \param element The current element
     * \param scvIdx Index of the sub-control volume to be checked
     */
    bool isFractureVertexOnBoundary_(const Element &element, int scvIdx) const
    {
        const GlobalPosition &globalPos = element.geometry().corner(scvIdx);
        int globalIdx = this->soil().vertexmapper().map(element, scvIdx, dim);

        int isVF = this->soil().isVertexFracture(globalIdx);
        return ((isVF && onUpperBoundary_(globalPos))
                 || (isVF && onLowerBoundary_(globalPos))
                 || (isVF && onLeftBoundary_(globalPos))
                 || (isVF && onRightBoundary_(globalPos)));
    }

    Scalar temperature_;
    Scalar eps_;

    GlobalPosition bboxMin_;
    GlobalPosition bboxMax_;
    std::vector<bool> isDuneFractureVertex_;
    std::vector<bool> isDuneFractureEdge_;
    std::vector<bool> fractureVolFractionVector;
    std::vector<int> fractureEdgesIdx_;
    std::vector<Scalar> distNodesOnBoundary;

    bool useInterfaceCondition_;
    bool inactivateFractures_;

};
} //end namespace

#endif // EWOMS_TEST_2PDFM_TEST_PROBLEM_HH