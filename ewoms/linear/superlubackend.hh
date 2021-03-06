// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*****************************************************************************
 *   Copyright (C) 2013 by Andreas Lauser                                    *
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
 * \copydoc Ewoms::Linear::SuperLUBackend
 */
#ifndef EWOMS_SUPER_LU_BACKEND_HH
#define EWOMS_SUPER_LU_BACKEND_HH

#if HAVE_SUPERLU

#include <ewoms/common/parametersystem.hh>

#include <ewoms/istl/solvers.hh>
#include <dune/istl/superlu.hh>
#include <dune/common/fmatrix.hh>

namespace Ewoms {
namespace Properties {
// forward declaration of the required property tags
NEW_PROP_TAG(Problem);
NEW_PROP_TAG(Scalar);
NEW_PROP_TAG(NumEq);
NEW_PROP_TAG(LinearSolverVerbosity);
NEW_PROP_TAG(LinearSolverBackend);

NEW_TYPE_TAG(SuperLULinearSolver);
}

namespace Linear {
/*!
 * \ingroup Linear
 * \brief A linear solver backend for the SuperLU sparse matrix library.
 */
template <class TypeTag>
class SuperLUBackend
{
    typedef typename GET_PROP_TYPE(TypeTag, Scalar) Scalar;
    typedef typename GET_PROP_TYPE(TypeTag, Problem) Problem;

public:
    SuperLUBackend(const Problem& problem)
        : problem_(problem)
    {}

    static void registerParameters()
    { REGISTER_PARAM(TypeTag, int, LinearSolverVerbosity, "The verbosity level of the linear solver"); }

    template<class Matrix, class Vector>
    bool solve(const Matrix& A, Vector& x, const Vector& b)
    {
        Vector bTmp(b);

        int verbosity = GET_PARAM(TypeTag, int, LinearSolverVerbosity);
        Dune::InverseOperatorResult result;
        Dune::SuperLU<Matrix> solver(A, verbosity > 0);
        solver.apply(x, bTmp, result);

        if (result.converged) {
            // make sure that the result only contains finite values.
            Scalar tmp = 0;
            for (unsigned i = 0; i < x.size(); ++i) {
                const auto &xi = x[i];
                for (int j = 0; j < Vector::block_type::dimension; ++j)
                    tmp += xi[j];
            }
            result.converged = std::isfinite(tmp);
        }

        return result.converged;
    }

private:
    const Problem& problem_;
};

} // namespace Linear

namespace Properties {
SET_INT_PROP(SuperLULinearSolver, LinearSolverVerbosity, 0);
SET_TYPE_PROP(SuperLULinearSolver, LinearSolverBackend, Ewoms::Linear::SuperLUBackend<TypeTag>);
} // namespace Properties
} // namespace Ewoms

#endif // HAVE_SUPERLU

#endif
