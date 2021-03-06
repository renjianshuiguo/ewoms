// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*****************************************************************************
 *   Copyright (C) 2009-2013 by Andreas Lauser                               *
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
 * \brief Provides a few default main functions for convenience.
 */
#ifndef EWOMS_START_HH
#define EWOMS_START_HH

#include "propertysystem.hh"
#include "parametersystem.hh"
#include <opm/common/valgrind.hh>

#include <ewoms/version.hh>
#include <ewoms/parallel/mpihelper.hh>
#include <ewoms/common/parametersystem.hh>

#include <dune/grid/io/file/dgfparser.hh>
#include <dune/common/parametertreeparser.hh>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <locale>

#include <stdio.h>
#include <unistd.h>

#if HAVE_MPI
#include <mpi.h>
#endif

namespace Ewoms {
// forward declaration of property tags
namespace Properties {
NEW_PROP_TAG(Scalar);
NEW_PROP_TAG(Grid);
NEW_PROP_TAG(GridCreator);
NEW_PROP_TAG(Problem);
NEW_PROP_TAG(TimeManager);
NEW_PROP_TAG(EndTime);
NEW_PROP_TAG(RestartTime);
NEW_PROP_TAG(InitialTimeStepSize);
NEW_PROP_TAG(PrintProperties);
NEW_PROP_TAG(PrintParameters);
NEW_PROP_TAG(ParameterFile);
}

//! \cond SKIP_THIS

/*!
 * \ingroup Startup
 *
 * \brief Prints the default usage message for the startWithParameters() function
 */
void printUsage(const char *progName, const std::string &errorMsg)
{
    if (errorMsg.size() > 0) {
        std::cout << errorMsg << "\n"
                  << "\n";
    }
    std::cout
        <<
        "Usage: " << progName << " [options]\n"
        "\n"
        "The available options are:\n";

    // print the -h/--help parameter. this is a little hack to make
    // things more maintainable but it uses some internals of the
    // parameter system.
    Parameters::ParamInfo tmp;
    tmp.paramName = "Help,-h";
    tmp.usageString = "Print this usage message and exit";
    Parameters::printParamUsage_(std::cout, tmp);

    // print the list of available parameters
    Parameters::printUsage(std::cout);
}

/*!
 * \ingroup Startup
 * \brief Read the command line arguments and write them into the parameter tree.
 *        Do some syntax checks.
 *
 * \param   argc      The 'argc' argument of the main function: count of arguments (1 if there are no arguments)
 * \param   argv      The 'argv' argument of the main function: array of pointers to the argument strings
 * \param   paramTree The parameterTree. It can be filled from an input file or the command line.
 * \return            Empty string if everything worked out. Otherwise the thing that could not be read.
 */
std::string readOptions_(int argc, char **argv, Dune::ParameterTree &paramTree)
{
    // All command line options need to start with '-'
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            std::ostringstream oss;
            oss << "Command line argument " << i << " (='" << argv[i] << "') is invalid.";
            return oss.str();
        }

        std::string paramName, paramValue;

        // read a --my-opt=abc option. This gets transformed
        // into the parameter "MyOpt" with the value being
        // "abc"
        if (argv[i][1] == '-') {
            // There is nothing after the '-'
            if (argv[i][2] == 0 || !std::isalpha(argv[i][2]))
            {
                std::ostringstream oss;
                oss << "Parameter name of argument " << i << " ('" << argv[i] << "')"
                    << " is invalid because it does not start with a letter.";
                return oss.str();
            }

            // copy everything after the "--" into a separate string
            std::string s(argv[i] + 2);

            // parse argument
            unsigned j = 0;
            while (true) {
                if (j >= s.size()) {
                    // encountered the end of the string, i.e. we
                    // have a parameter where the argument is empty
                    paramName = s;
                    paramValue = "";
                    break;
                }
                else if (s[j] == '=') {
                    // we encountered a '=' character. everything
                    // before is the name of the parameter,
                    // everything after is the value.
                    paramName = s.substr(0, j);
                    paramValue = s.substr(j+1);
                    break;
                }
                else if (s[j] == '-') {
                    // remove all "-" characters and capitalize the
                    // character after them
                    s.erase(j, 1);
                    if (s.size() == j)
                    {
                        std::ostringstream oss;
                        oss << "Parameter name of argument " << i << " ('" << argv[i] << "')"
                            << " is invalid (ends with a '-' character).";
                        return oss.str();
                    }
                    else if (s[j] == '-')
                    {
                        std::ostringstream oss;
                        oss << "Malformed parameter name in argument " << i << " ('" << argv[i] << "'): "
                            << "'--' in parameter name.";
                        return oss.str();
                    }
                    s[j] = std::toupper(s[j]);
                }
                else if (!std::isalnum(s[j]))
                {
                    std::ostringstream oss;
                    oss << "Parameter name of argument " << i << " ('" << argv[i] << "')"
                        << " is invalid (character '"<<s[j]<<"' is not a letter or digit).";
                    return oss.str();
                }

                ++j;
            }
        }
        else {
            // read a -myOpt abc option for the parameter "MyOpt" with
            // a value of "abc"
            paramName = argv[i] + 1;

            if (argc == i + 1 || argv[i+1][0] == '-') {
                std::ostringstream oss;
                oss << "No argument given for parameter '" << argv[i] << "'!";
                return oss.str();
            }

            paramValue = argv[i+1];
            ++i; // In the case of '-myOpt abc' each pair counts as two arguments
        }

        // capitalize first letter of parameter name
        paramName[0] = std::toupper(paramName[0]);

        // Put the key=value pair into the parameter tree
        paramTree[paramName] = paramValue;
    }
    return "";
}

/*!
 * \brief Register all runtime parameters, parse the command line
 *        arguments and the parameter file.
 *
 * \param argc The number of command line arguments
 * \param argv Array with the command line argument strings
 */
template <class TypeTag>
bool setupParameters_(int argc, char ** argv)
{
    typedef typename GET_PROP_TYPE(TypeTag, Scalar) Scalar;
    typedef typename GET_PROP_TYPE(TypeTag, GridCreator) GridCreator;
    typedef typename GET_PROP_TYPE(TypeTag, TimeManager) TimeManager;

    // first, get the MPI rank of the current process
    int myRank = 0;
#if HAVE_MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
#endif

    ////////////////////////////////////////////////////////////
    // Register all parameters
    ////////////////////////////////////////////////////////////
    REGISTER_PARAM(TypeTag, std::string,
                   ParameterFile,
                   "An .ini file which contains a set of run-time parameters");
    REGISTER_PARAM(TypeTag, int,
                   PrintProperties,
                   "Print the values of the compile time properties at the start of the simulation");
    REGISTER_PARAM(TypeTag, int,
                   PrintParameters,
                   "Print the values of the run-time parameters at the start of the simulation");
    REGISTER_PARAM(TypeTag, Scalar,
                   EndTime,
                   "The time at which the simulation is finished [s]");
    REGISTER_PARAM(TypeTag, Scalar,
                   InitialTimeStepSize,
                   "The size of the initial time step [s]");
    REGISTER_PARAM(TypeTag, Scalar,
                   RestartTime,
                   "The time time at which a simulation restart should be attempted [s]");

    GridCreator::registerParameters();
    TimeManager::registerParameters();

    ////////////////////////////////////////////////////////////
    // set the parameter values
    ////////////////////////////////////////////////////////////

    // check whether the user wanted to see the help message
    for (int i = 1; i < argc; ++i) {
        if (std::string("--help") == argv[i] || std::string("-h") == argv[i])
        {
            if (myRank == 0)
                printUsage(argv[0], "");
            return /*continueExecution=*/false;
        }
    }

    // fill the parameter tree with the options from the command line
    typedef typename GET_PROP(TypeTag, ParameterTree) ParameterTree;
    std::string s = readOptions_(argc, argv, ParameterTree::tree());
    if (!s.empty()) {
        if (myRank == 0)
            printUsage(argv[0], s);
        return /*continueExecution=*/false;
    }

    std::string paramFileName = GET_PARAM_(TypeTag, std::string, ParameterFile);
    if (paramFileName != "") {
        ////////////////////////////////////////////////////////////
        // add the parameters specified using an .ini file
        ////////////////////////////////////////////////////////////

        // check whether the parameter file is readable.
        std::ifstream tmp;
        tmp.open(paramFileName.c_str());
        if (!tmp.is_open()){
            std::ostringstream oss;
            if (myRank == 0) {
                oss << "Parameter file \"" << paramFileName << "\" is does not exist or is not readable.";
                printUsage(argv[0], oss.str());
            }
            return /*continueExecution=*/false;
        }

        // read the parameter file.
        Dune::ParameterTreeParser::readINITree(paramFileName,
                                               ParameterTree::tree(),
                                               /*overwrite=*/false);
    }

    END_PARAM_REGISTRATION;

    return /*continueExecution=*/true;
}

//! \endcond

/*!
 * \ingroup Startup
 *
 * \brief Provides a main function which reads in parameters from the
 *        command line and a parameter file.
 *
 * \tparam TypeTag  The type tag of the problem which needs to be solved
 *
 * \param argc The number of command line arguments
 * \param argv The array of the command line arguments
 */
template <class TypeTag>
int start(int argc,
          char **argv)
{
    typedef typename GET_PROP_TYPE(TypeTag, Scalar) Scalar;
    typedef typename GET_PROP_TYPE(TypeTag, GridCreator) GridCreator;
    typedef typename GET_PROP_TYPE(TypeTag, Problem) Problem;
    typedef typename GET_PROP_TYPE(TypeTag, TimeManager) TimeManager;

    // initialize MPI, finalize is done automatically on exit
    const Ewoms::MpiHelper mpiHelper(argc, argv);

    int myRank = mpiHelper.rank();

    try {
        if (!setupParameters_<TypeTag>(argc, argv))
            return 1;

        // read the initial time step and the end time
        double tEnd;
        double dt;

        tEnd = GET_PARAM(TypeTag, Scalar, EndTime);
        if (tEnd < -1e50) {
            if (myRank == 0)
                printUsage(argv[0], "Mandatory parameter '--end-time' not specified!");
            return 1;
        }

        dt = GET_PARAM(TypeTag, Scalar, InitialTimeStepSize);
        if (dt < -1e50) {
            if (myRank == 0)
                printUsage(argv[0], "Mandatory parameter '--initial-time-step-size' not specified!");
            return 1;
        }

        // deal with the restart stuff
        bool restart = false;
        Scalar restartTime = GET_PARAM(TypeTag, Scalar, RestartTime);
        if (restartTime != -1e100)
            restart = true;
        else
            restartTime = 0.0;

        if (myRank == 0)
            std::cout
                << "eWoms " << EWOMS_VERSION
#ifdef EWOMS_CODENAME
                << " (\"" << EWOMS_CODENAME << "\")"
#endif
                <<" will now start the trip. "
                << "Please sit back, relax and enjoy the ride.\n";

        // print the parameters if requested
        int printParams = GET_PARAM(TypeTag, int, PrintParameters);
        if (myRank == 0) {
            if (printParams) {
                if (printParams == 1 || !isatty(fileno(stdout)))
                    Ewoms::Parameters::printValues<TypeTag>();
                else
                    // always print the list of specified but unused parameters
                    Ewoms::Parameters::printUnused<TypeTag>();
            }
            else
                // always print the list of specified but unused parameters
                Ewoms::Parameters::printUnused<TypeTag>();
        }

        // print the properties if requested
        int printProps = GET_PARAM(TypeTag, int, PrintProperties);
        if (printProps && myRank == 0) {
            if (printProps == 1 || !isatty(fileno(stdout)))
                Ewoms::Properties::print<TypeTag>();
        }

        // try to create a grid (from the given grid file)
        if (myRank ==  0) std::cout << "Creating the grid\n";
        try { GridCreator::makeGrid(); }
        catch (const Dune::Exception &e) {
            std::cout << __FILE__ << ":" << __LINE__ << ":"
                      << " Creation of the grid failed: " << e.what() << "\n";
            return 1;
        }
        if (myRank ==  0) std::cout << "Distributing the grid\n";
        GridCreator::loadBalance();

        // instantiate and run the concrete problem. make sure to
        // deallocate the problem and before the time manager and the
        // grid
        TimeManager timeManager;
        {
            Problem problem(timeManager);
            timeManager.init(problem, restartTime, dt, tEnd, restart);
            timeManager.run();
        }

        if (myRank == 0) {
            std::cout
                << "eWoms reached the destination. "
                << "If it took a wrong corner, change your booking and try again.\n";
        }
        GridCreator::deleteGrid();
        return 0;
    }
    catch (Ewoms::ParameterException &e) {
        if (myRank == 0)
            std::cout << e << ". Abort!\n";
        GridCreator::deleteGrid();
        return 1;
    }
    catch (Dune::Exception &e) {
        if (myRank == 0)
            std::cout << "Dune reported an error: " << e << std::endl;
        GridCreator::deleteGrid();
        return 2;
    }
    catch (...) {
        if (myRank == 0)
            std::cout << "Unknown exception thrown!\n";
        GridCreator::deleteGrid();
        return 3;
    }
}

} // namespace Ewoms

#endif
