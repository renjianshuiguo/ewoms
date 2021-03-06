// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*****************************************************************************
 *   Copyright (C) 2008-2013 by Andreas Lauser                               *
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
 * \copydoc Ewoms::DgfGridCreator
 */
#ifndef EWOMS_DGF_GRID_CREATOR_HH
#define EWOMS_DGF_GRID_CREATOR_HH

#include <ewoms/parallel/mpihelper.hh>

#include <dune/grid/io/file/dgfparser.hh>

#include <ewoms/common/propertysystem.hh>
#include <ewoms/common/parametersystem.hh>

#include <string>

namespace Ewoms {

namespace Properties {
NEW_PROP_TAG(Grid);
NEW_PROP_TAG(GridFile);
NEW_PROP_TAG(GridGlobalRefinements);
}

/*!
 * \brief Provides a grid creator which reads Dune Grid Format (DGF) files
 */
template <class TypeTag>
class DgfGridCreator
{
    typedef typename GET_PROP_TYPE(TypeTag, Grid) Grid;
    typedef Dune::GridPtr<Grid> GridPointer;

public:
    /*!
     * \brief Register all run-time parameters for the grid creator.
     */
    static void registerParameters()
    {
        REGISTER_PARAM(TypeTag, std::string, GridFile, "The file name of the DGF file to load");
        REGISTER_PARAM(TypeTag, unsigned, GridGlobalRefinements, "The number of global refinements of the grid executed after it was loaded");
    }

    /*!
     * \brief Load the grid from the file.
     */
    static void makeGrid()
    {
        const std::string dgfFileName = GET_PARAM(TypeTag, std::string, GridFile);
        unsigned numRefinments = GET_PARAM(TypeTag, unsigned, GridGlobalRefinements);

        gridPtr_ = GridPointer(dgfFileName.c_str(), Dune::MPIHelper::getCommunicator());
        if (numRefinments > 0)
            gridPtr_->globalRefine(numRefinments);
        initialized_ = true;
    }

    /*!
     * \brief Returns a reference to the grid.
     */
    static Grid &grid()
    { return *gridPtr_; }

    /*!
     * \brief Returns a reference to the grid pointer.
     *
     * This method is specific to the DgfGridCreator!
     */
    static GridPointer &gridPtr()
    { return gridPtr_; }

    /*!
     * \brief Distribute the grid (and attached data) over all
     *        processes.
     */
    static void loadBalance()
    { gridPtr_.loadBalance(); }

    /*!
     * \brief Destroys the grid
     *
     * This is required to guarantee that the grid is deleted before MPI_Comm_free is called.
     */
    static void deleteGrid()
    { if (initialized_) delete gridPtr_.release(); initialized_ = false; }

private:
    static GridPointer gridPtr_;
    static bool initialized_;
};

template <class TypeTag>
typename DgfGridCreator<TypeTag>::GridPointer DgfGridCreator<TypeTag>::gridPtr_;

template <class TypeTag>
bool DgfGridCreator<TypeTag>::initialized_ = false;

} // namespace Ewoms

#endif
