// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*****************************************************************************
 *   Copyright (C) 2010-2012 by Andreas Lauser                               *
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
 * \copydoc Ewoms::VcfvElementGeometry
 */
#ifndef EWOMS_VCFV_ELEMENT_GEOMETRY_HH
#define EWOMS_VCFV_ELEMENT_GEOMETRY_HH

#include <ewoms/common/quadraturegeometries.hh>

#include <dune/grid/common/intersectioniterator.hh>
#include <dune/common/fvector.hh>
#include <dune/geometry/referenceelements.hh>
#include <dune/geometry/type.hh>
#include <dune/localfunctions/lagrange/pqkfactory.hh>

#include <vector>

namespace Ewoms {

/*!
 * \cond SKIP_THIS
 */
template <class Scalar, int dim, int basicGeomType>
class VcfvScvGeometries;

////////////////////
// local geometries for 1D elements
////////////////////
template <class Scalar>
class VcfvScvGeometries<Scalar, /*dim=*/1, Dune::GeometryType::cube>
{
    enum { dim = 1 };
    enum { numScv = 2};
    static const Dune::GeometryType::BasicType basicType = Dune::GeometryType::simplex;

public:
    typedef Ewoms::QuadrialteralQuadratureGeometry<Scalar, dim> ScvGeometry;

    static int init()
    {
        // 1D LINE SEGMENTS
        Scalar scvCorners[numScv][ScvGeometry::numCorners][dim] = {
            { // corners of the first sub control volume
                { 0.0 },
                { 0.5 }
            },

            { // corners of the second sub control volume
                { 0.5 },
                { 1.0 }
            }
        };
        for (unsigned scvIdx = 0; scvIdx < numScv; ++scvIdx)
            scvGeoms_[scvIdx].setCorners(scvCorners[scvIdx], ScvGeometry::numCorners);

        return 0;
    }

    static const ScvGeometry &get(int scvIdx)
    { return scvGeoms_[scvIdx]; }

private:
    static ScvGeometry scvGeoms_[numScv];
};

template <class Scalar>
typename VcfvScvGeometries<Scalar, /*dim=*/1, Dune::GeometryType::cube>::ScvGeometry
VcfvScvGeometries<Scalar, /*dim=*/1, Dune::GeometryType::cube>::scvGeoms_[
    VcfvScvGeometries<Scalar, /*dim=*/1, Dune::GeometryType::cube>::numScv];

template <class Scalar>
class VcfvScvGeometries<Scalar, /*dim=*/1, Dune::GeometryType::simplex>
{
    enum { dim = 1 };
    enum { numScv = 2};
    static const Dune::GeometryType::BasicType basicType = Dune::GeometryType::simplex;

public:
    typedef Ewoms::QuadrialteralQuadratureGeometry<Scalar, dim> ScvGeometry;

    static const ScvGeometry &get(int scvIdx)
    { DUNE_THROW(Dune::NotImplemented, "VcfvScvGeometries<Scalar, 1, Dune::GeometryType::simplex>"); }
};

////////////////////
// local geometries for 2D elements
////////////////////
template <class Scalar>
class VcfvScvGeometries<Scalar, /*dim=*/2, Dune::GeometryType::simplex>
{
    enum { dim = 2 };
    enum { numScv = 3};
    static const Dune::GeometryType::BasicType basicType = Dune::GeometryType::simplex;

public:
    typedef Ewoms::QuadrialteralQuadratureGeometry<Scalar, dim> ScvGeometry;

    static const ScvGeometry &get(int scvIdx)
    { return scvGeoms_[scvIdx]; }

    static int init()
    {
        // 2D SIMPLEX
        Scalar scvCorners[numScv][ScvGeometry::numCorners][dim] =
        {
            { // SCV 0 corners
                { 0.0,   0.0 },
                { 1.0/2, 0.0 },
                { 1.0/3, 1.0/3 },
                { 0.0,   1.0/2 },
            },

            { // SCV 1 corners
                { 1.0/2, 0.0 },
                { 1.0,   0.0 },
                { 1.0/3, 1.0/3 },
                { 1.0/2, 1.0/2 },
            },

            { // SCV 2 corners
                { 0.0,   1.0/2 },
                { 1.0/3, 1.0/3 },
                { 0.0,   1.0 },
                { 1.0/2, 1.0/2 },
            }
        };

        for (int scvIdx = 0; scvIdx < numScv; ++scvIdx)
            scvGeoms_[scvIdx].setCorners(scvCorners[scvIdx], ScvGeometry::numCorners);

        return 0;
    }

private:
    static ScvGeometry scvGeoms_[numScv];
};

template <class Scalar>
typename VcfvScvGeometries<Scalar, /*dim=*/2, Dune::GeometryType::simplex>::ScvGeometry
VcfvScvGeometries<Scalar, /*dim=*/2, Dune::GeometryType::simplex>::scvGeoms_[
    VcfvScvGeometries<Scalar, /*dim=*/2, Dune::GeometryType::simplex>::numScv];

template <class Scalar>
class VcfvScvGeometries<Scalar, /*dim=*/2, Dune::GeometryType::cube>
{
    enum { dim = 2 };
    enum { numScv = 4 };
    static const Dune::GeometryType::BasicType basicType = Dune::GeometryType::cube;

public:
    typedef Ewoms::QuadrialteralQuadratureGeometry<Scalar, dim> ScvGeometry;

    static const ScvGeometry &get(int scvIdx)
    { return scvGeoms_[scvIdx]; }


    static int init()
    {
        // 2D CUBE
        Scalar scvCorners[numScv][ScvGeometry::numCorners][dim] =
        {
            { // SCV 0 corners
                { 0.0, 0.0 },
                { 0.5, 0.0 },
                { 0.0, 0.5 },
                { 0.5, 0.5 }
            },

            { // SCV 1 corners
                { 0.5, 0.0 },
                { 1.0, 0.0 },
                { 0.5, 0.5 },
                { 1.0, 0.5 }
            },

            { // SCV 2 corners
                { 0.0,  0.5 },
                { 0.5,  0.5 },
                { 0.0,  1.0 },
                { 0.5,  1.0 }
            },

            { // SCV 3 corners
                { 0.5,   0.5 },
                { 1.0,   0.5 },
                { 0.5,   1.0 },
                { 1.0,   1.0 }
            }
        };

        for (int scvIdx = 0; scvIdx < numScv; ++scvIdx)
            scvGeoms_[scvIdx].setCorners(scvCorners[scvIdx], ScvGeometry::numCorners);

        return 0;
    }

    static ScvGeometry scvGeoms_[numScv];
};

template <class Scalar>
typename VcfvScvGeometries<Scalar, /*dim=*/2, Dune::GeometryType::cube>::ScvGeometry
VcfvScvGeometries<Scalar, /*dim=*/2, Dune::GeometryType::cube>::scvGeoms_[
    VcfvScvGeometries<Scalar, /*dim=*/2, Dune::GeometryType::cube>::numScv];

////////////////////
// local geometries for 3D elements
////////////////////
template <class Scalar>
class VcfvScvGeometries<Scalar, /*dim=*/3, Dune::GeometryType::simplex>
{
    enum { dim = 3 };
    enum { numScv = 4 };
    static const Dune::GeometryType::BasicType basicType = Dune::GeometryType::simplex;

public:
    typedef Ewoms::QuadrialteralQuadratureGeometry<Scalar, dim> ScvGeometry;

    static const ScvGeometry &get(int scvIdx)
    { return scvGeoms_[scvIdx]; }

    static int init()
    {
        // 3D SIMPLEX
        Scalar scvCorners[numScv][ScvGeometry::numCorners][dim] =
        {
            { // SCV 0 corners
                { 0.0,   0.0,   0.0 },
                { 1.0/2, 0.0,   0.0 },
                { 0.0,   1.0/2, 0.0 },
                { 1.0/3, 1.0/3, 0.0 },

                { 0.0,   0.0,   0.5 },
                { 1.0/3, 0.0,   1.0/3 },
                { 0.0,   1.0/3, 1.0/3 },
                { 1.0/4, 1.0/4, 1.0/4 },
            },

            { // SCV 1 corners
                { 1.0/2, 0.0,   0.0 },
                { 1.0,   0.0,   0.0 },
                { 1.0/3, 1.0/3, 0.0 },
                { 1.0/2, 1.0/2, 0.0 },

                { 1.0/3, 0.0,   1.0/3 },
                { 1.0/2, 0.0,   1.0/2 },
                { 1.0/4, 1.0/4, 1.0/4 },
                { 1.0/3, 1.0/3, 1.0/3 },
            },

            { // SCV 2 corners
                { 0.0,   1.0/2, 0.0 },
                { 1.0/3, 1.0/3, 0.0 },
                { 0.0,   1.0,   0.0 },
                { 1.0/2, 1.0/2, 0.0 },

                { 0.0,   1.0/3, 1.0/3 },
                { 1.0/4, 1.0/4, 1.0/4 },
                { 0.0,   1.0/2, 1.0/2 },
                { 1.0/3, 1.0/3, 1.0/3 },
            },

            { // SCV 3 corners
                { 0.0,   0.0,   1.0/2 },
                { 1.0/3, 0.0,   1.0/3 },
                { 0.0,   1.0/3, 1.0/3 },
                { 1.0/4, 1.0/4, 1.0/4 },

                { 0.0,   0.0,   1.0   },
                { 1.0/2, 0.0,   1.0/2 },
                { 0.0,   1.0/2, 1.0/2 },
                { 1.0/3, 1.0/3, 1.0/3 },
            }
        };

        for (int scvIdx = 0; scvIdx < numScv; ++scvIdx)
            scvGeoms_[scvIdx].setCorners(scvCorners[scvIdx],  ScvGeometry::numCorners);

        return 0;
    }

private:
    static ScvGeometry scvGeoms_[numScv];
};

template <class Scalar>
typename VcfvScvGeometries<Scalar, /*dim=*/3, Dune::GeometryType::simplex>::ScvGeometry
VcfvScvGeometries<Scalar, /*dim=*/3, Dune::GeometryType::simplex>::scvGeoms_[
    VcfvScvGeometries<Scalar, /*dim=*/3, Dune::GeometryType::simplex>::numScv];

template <class Scalar>
class VcfvScvGeometries<Scalar, /*dim=*/3, Dune::GeometryType::cube>
{
    enum { dim = 3 };
    enum { numScv = 8 };
    static const Dune::GeometryType::BasicType basicType = Dune::GeometryType::cube;

public:
    typedef Ewoms::QuadrialteralQuadratureGeometry<Scalar, dim> ScvGeometry;

    static const ScvGeometry &get(int scvIdx)
    { return scvGeoms_[scvIdx]; }

    static int init()
    {
        // 3D CUBE
        Scalar scvCorners[numScv][ScvGeometry::numCorners][dim] =
        {
            { // SCV 0 corners
                { 0.0,   0.0,   0.0 },
                { 1.0/2, 0.0,   0.0 },
                { 0.0,   1.0/2, 0.0 },
                { 1.0/2, 1.0/2, 0.0 },

                { 0.0,   0.0,   1.0/2 },
                { 1.0/2, 0.0,   1.0/2 },
                { 0.0,   1.0/2, 1.0/2 },
                { 1.0/2, 1.0/2, 1.0/2 },
            },

            { // SCV 1 corners
                { 1.0/2, 0.0,   0.0 },
                { 1.0,   0.0,   0.0 },
                { 1.0/2, 1.0/2, 0.0 },
                { 1.0,   1.0/2, 0.0 },

                { 1.0/2, 0.0,   1.0/2 },
                { 1.0,   0.0,   1.0/2 },
                { 1.0/2, 1.0/2, 1.0/2 },
                { 1.0,   1.0/2, 1.0/2 },
            },

            { // SCV 2 corners
                { 0.0,   1.0/2, 0.0 },
                { 1.0/2, 1.0/2, 0.0 },
                { 0.0,   1.0,   0.0 },
                { 1.0/2, 1.0,   0.0 },

                { 0.0,   1.0/2, 1.0/2 },
                { 1.0/2, 1.0/2, 1.0/2 },
                { 0.0,   1.0,   1.0/2 },
                { 1.0/2, 1.0,   1.0/2 },
            },

            { // SCV 3 corners
                { 1.0/2, 1.0/2, 0.0 },
                { 1.0,   1.0/2, 0.0 },
                { 1.0/2, 1.0,   0.0 },
                { 1.0,   1.0,   0.0 },

                { 1.0/2, 1.0/2, 1.0/2 },
                { 1.0,   1.0/2, 1.0/2 },
                { 1.0/2, 1.0,   1.0/2 },
                { 1.0,   1.0,   1.0/2 },
            },

            { // SCV 4 corners
                { 0.0,   0.0,   1.0/2 },
                { 1.0/2, 0.0,   1.0/2 },
                { 0.0,   1.0/2, 1.0/2 },
                { 1.0/2, 1.0/2, 1.0/2 },

                { 0.0,   0.0,   1.0 },
                { 1.0/2, 0.0,   1.0 },
                { 0.0,   1.0/2, 1.0 },
                { 1.0/2, 1.0/2, 1.0 },
            },

            { // SCV 5 corners
                { 1.0/2, 0.0,   1.0/2 },
                { 1.0,   0.0,   1.0/2 },
                { 1.0/2, 1.0/2, 1.0/2 },
                { 1.0,   1.0/2, 1.0/2 },

                { 1.0/2, 0.0,   1.0 },
                { 1.0,   0.0,   1.0 },
                { 1.0/2, 1.0/2, 1.0 },
                { 1.0,   1.0/2, 1.0 },
            },

            { // SCV 6 corners
                { 0.0,   1.0/2, 1.0/2 },
                { 1.0/2, 1.0/2, 1.0/2 },
                { 0.0,   1.0,   1.0/2 },
                { 1.0/2, 1.0,   1.0/2 },

                { 0.0,   1.0/2, 1.0 },
                { 1.0/2, 1.0/2, 1.0 },
                { 0.0,   1.0,   1.0 },
                { 1.0/2, 1.0,   1.0 },
            },

            { // SCV 7 corners
                { 1.0/2, 1.0/2, 1.0/2 },
                { 1.0,   1.0/2, 1.0/2 },
                { 1.0/2, 1.0,   1.0/2 },
                { 1.0,   1.0,   1.0/2 },

                { 1.0/2, 1.0/2, 1.0 },
                { 1.0,   1.0/2, 1.0 },
                { 1.0/2, 1.0,   1.0 },
                { 1.0,   1.0,   1.0 },
            },
        };

        for (int scvIdx = 0; scvIdx < numScv; ++scvIdx)
            scvGeoms_[scvIdx].setCorners(scvCorners[scvIdx], ScvGeometry::numCorners);

        return 0;
    }
private:
    static ScvGeometry scvGeoms_[numScv];
};

template <class Scalar>
typename VcfvScvGeometries<Scalar, /*dim=*/3, Dune::GeometryType::cube>::ScvGeometry
VcfvScvGeometries<Scalar, /*dim=*/3, Dune::GeometryType::cube>::scvGeoms_[
    VcfvScvGeometries<Scalar, /*dim=*/3, Dune::GeometryType::cube>::numScv];

/*!
 * \endcond
 */

/*!
 * \ingroup VcfvModel
 *
 * \brief Represents the finite volume geometry of a single element in
 *        the VCVF discretization.
 *
 * The VCVF discretization is a vertex centered finite volume approach. This
 * means that each vertex corresponds to a control volume which
 * intersects each of the vertex' neighboring elements. If only
 * looking at a single element of the primary grid (which is what this
 * class does), the element is subdivided into multiple fragments of
 * control volumes called sub-control volumes. Each of the element's
 * vertices corresponds to exactly one sub-control volume in this
 * scenario.
 *
 * For the vertex-cented finite volume method the sub-control volumes
 * are constructed by connecting the element's center with each edge
 * of the element.
 */
template <class Scalar, class GridView>
class VcfvElementGeometry
{
    enum{dim = GridView::dimension};
    enum{dimWorld = GridView::dimensionworld};
    enum{maxNC = (dim < 3 ? 4 : 8)};
    enum{maxNE = (dim < 3 ? 4 : 12)};
    enum{maxNF = (dim < 3 ? 1 : 6)};
    enum{maxCOS = (dim < 3 ? 2 : 4)};
    enum{maxBF = (dim < 3 ? 8 : 24)};
    typedef typename GridView::ctype CoordScalar;
    typedef typename GridView::Traits::template Codim<0>::Entity Element;
    typedef typename Element::Geometry Geometry;
    typedef Dune::FieldVector<Scalar,dimWorld> DimVector;
    typedef Dune::FieldVector<CoordScalar,dimWorld> GlobalPosition;
    typedef Dune::FieldVector<CoordScalar,dim> LocalPosition;
    typedef typename GridView::IntersectionIterator IntersectionIterator;

    typedef Dune::PQkLocalFiniteElementCache<CoordScalar, Scalar, dim, 1> LocalFiniteElementCache;
    typedef typename LocalFiniteElementCache::FiniteElementType LocalFiniteElement;
    typedef typename LocalFiniteElement::Traits::LocalBasisType::Traits LocalBasisTraits;
    typedef typename LocalBasisTraits::JacobianType ShapeJacobian;

    typedef Ewoms::QuadrialteralQuadratureGeometry<Scalar, dim> ScvGeometry;

    Scalar quadrilateralArea(const GlobalPosition& p0, const GlobalPosition& p1, const GlobalPosition& p2, const GlobalPosition& p3)
    {
        return 0.5*std::abs((p3[0] - p1[0])*(p2[1] - p0[1]) - (p3[1] - p1[1])*(p2[0] - p0[0]));
    }

    void crossProduct(DimVector& c, const DimVector& a, const DimVector& b)
    {
        c[0] = a[1]*b[2] - a[2]*b[1];
        c[1] = a[2]*b[0] - a[0]*b[2];
        c[2] = a[0]*b[1] - a[1]*b[0];
    }

    Scalar pyramidVolume (const GlobalPosition& p0, const GlobalPosition& p1, const GlobalPosition& p2, const GlobalPosition& p3, const GlobalPosition& p4)
    {
        DimVector a(p2); a -= p0;
        DimVector b(p3); b -= p1;

        DimVector n;
        crossProduct(n, a, b);

        a = p4; a -= p0;

        return 1.0/6.0*(n*a);
    }

    Scalar prismVolume (const GlobalPosition& p0, const GlobalPosition& p1, const GlobalPosition& p2, const GlobalPosition& p3, const GlobalPosition& p4, const GlobalPosition& p5)
    {
        DimVector a(p4);
        for (int k = 0; k < dimWorld; ++k)
            a[k] -= p0[k];
        DimVector b(p1);
        for (int k = 0; k < dimWorld; ++k)
            b[k] -= p3[k];
        DimVector m;
        crossProduct(m, a, b);

        for (int k = 0; k < dimWorld; ++k)
            a[k] = p1[k] - p0[k];
        for (int k = 0; k < dimWorld; ++k)
            b[k] = p2[k] - p0[k];
        DimVector n;
        crossProduct(n, a, b);
        n += m;

        for (int k = 0; k < dimWorld; ++k)
            a[k] = p5[k] - p0[k];

        return std::abs(1.0/6.0*(n*a));
    }

    Scalar hexahedronVolume (const GlobalPosition& p0, const GlobalPosition& p1, const GlobalPosition& p2, const GlobalPosition& p3,
                             const GlobalPosition& p4, const GlobalPosition& p5, const GlobalPosition& p6, const GlobalPosition& p7)
    {
        return
            prismVolume(p0,p1,p2,p4,p5,p6)
            + prismVolume(p0,p2,p3,p4,p6,p7);
    }

    void normalOfQuadrilateral3D(DimVector &normal, const GlobalPosition& p0, const GlobalPosition& p1, const GlobalPosition& p2, const GlobalPosition& p3)
    {
        DimVector a(p2);
        for (int k = 0; k < dimWorld; ++k)
            a[k] -= p0[k];
        DimVector b(p3);
        for (int k = 0; k < dimWorld; ++k)
            b[k] -= p1[k];

        crossProduct(normal, a, b);
        normal *= 0.5;
    }

    Scalar quadrilateralArea3D(const GlobalPosition& p0, const GlobalPosition& p1, const GlobalPosition& p2, const GlobalPosition& p3)
    {
        DimVector normal;
        normalOfQuadrilateral3D(normal, p0, p1, p2, p3);
        return normal.two_norm();
    }

    void getFaceIndices(int numVertices, int k, int& leftFace, int& rightFace)
    {
        static const int edgeToFaceTet[2][6] = {
            {1, 0, 3, 2, 1, 3},
            {0, 2, 0, 1, 3, 2}
        };
        static const int edgeToFacePyramid[2][8] = {
            {1, 2, 3, 4, 1, 3, 4, 2},
            {0, 0, 0, 0, 3, 2, 1, 4}
        };
        static const int edgeToFacePrism[2][9] = {
            {1, 0, 2, 0, 1, 2, 4, 4, 4},
            {0, 2, 1, 3, 3, 3, 0, 1, 2}
        };
        static const int edgeToFaceHex[2][12] = {
            {0, 2, 3, 1, 4, 1, 2, 4, 0, 5, 5, 3},
            {2, 1, 0, 3, 0, 4, 4, 3, 5, 1, 2, 5}
        };

        switch (numVertices) {
        case 4:
            leftFace = edgeToFaceTet[0][k];
            rightFace = edgeToFaceTet[1][k];
            break;
        case 5:
            leftFace = edgeToFacePyramid[0][k];
            rightFace = edgeToFacePyramid[1][k];
            break;
        case 6:
            leftFace = edgeToFacePrism[0][k];
            rightFace = edgeToFacePrism[1][k];
            break;
        case 8:
            leftFace = edgeToFaceHex[0][k];
            rightFace = edgeToFaceHex[1][k];
            break;
        default:
            DUNE_THROW(Dune::NotImplemented, "VcfvElementGeometry :: getFaceIndices for numVertices = " << numVertices);
            break;
        }
    }

    void getEdgeIndices(int numVertices, int face, int vert, int& leftEdge, int& rightEdge)
    {
        static const int faceAndVertexToLeftEdgeTet[4][4] = {
                { 0, 0, 2, -1},
                { 0, 0, -1, 3},
                { 1, -1, 1, 3},
                {-1, 2, 2, 4}
        };
        static const int faceAndVertexToRightEdgeTet[4][4] = {
                { 1, 2, 1, -1},
                { 3, 4, -1, 4},
                { 3, -1, 5, 5},
                {-1, 4, 5, 5}
        };
        static const int faceAndVertexToLeftEdgePyramid[5][5] = {
            { 0, 2, 3, 1, -1},
            { 0, -1, 0, -1, 4},
            {-1, 1, -1, 1, 5},
            { 2, 2, -1, -1, 4},
            {-1, -1, 3, 3, 7}
        };
        static const int faceAndVertexToRightEdgePyramid[5][5] = {
            { 2, 1, 0, 3, -1},
            { 4, -1, 6, -1, 6},
            {-1, 5, -1, 7, 7},
            { 4, 5, -1, -1, 5},
            {-1, -1, 6, 7, 6}
        };
        static const int faceAndVertexToLeftEdgePrism[5][6] = {
            { 3, 3, -1, 0, 1, -1},
            { 4, -1, 4, 0, -1, 2},
            {-1, 5, 5, -1, 1, 2},
            { 3, 3, 5, -1, -1, -1},
            {-1, -1, -1, 6, 6, 8}
        };
        static const int faceAndVertexToRightEdgePrism[5][6] = {
            { 0, 1, -1, 6, 6, -1},
            { 0, -1, 2, 7, -1, 7},
            {-1, 1, 2, -1, 8, 8},
            { 4, 5, 4, -1, -1, -1},
            {-1, -1, -1, 7, 8, 7}
        };
        static const int faceAndVertexToLeftEdgeHex[6][8] = {
            { 0, -1, 4, -1, 8, -1, 2, -1},
            {-1, 5, -1, 3, -1, 1, -1, 9},
            { 6, 1, -1, -1, 0, 10, -1, -1},
            {-1, -1, 2, 7, -1, -1, 11, 3},
            { 4, 6, 7, 5, -1, -1, -1, -1},
            {-1, -1, -1, -1, 10, 9, 8, 11}
        };
        static const int faceAndVertexToRightEdgeHex[6][8] = {
            { 4, -1, 2, -1, 0, -1, 8, -1},
            {-1, 1, -1, 5, -1, 9, -1, 3},
            { 0, 6, -1, -1, 10, 1, -1, -1},
            {-1, -1, 7, 3, -1, -1, 2, 11},
            { 6, 5, 4, 7, -1, -1, -1, -1},
            {-1, -1, -1, -1, 8, 10, 11, 9}
        };

        switch (numVertices) {
        case 4:
            leftEdge = faceAndVertexToLeftEdgeTet[face][vert];
            rightEdge = faceAndVertexToRightEdgeTet[face][vert];
            break;
        case 5:
            leftEdge = faceAndVertexToLeftEdgePyramid[face][vert];
            rightEdge = faceAndVertexToRightEdgePyramid[face][vert];
            break;
        case 6:
            leftEdge = faceAndVertexToLeftEdgePrism[face][vert];
            rightEdge = faceAndVertexToRightEdgePrism[face][vert];
            break;
        case 8:
            leftEdge = faceAndVertexToLeftEdgeHex[face][vert];
            rightEdge = faceAndVertexToRightEdgeHex[face][vert];
            break;
        default:
            DUNE_THROW(Dune::NotImplemented, "VcfvElementGeometry :: getFaceIndices for numVertices = " << numVertices);
            break;
        }
    }

public:
    int boundaryFaceIndex(int faceIdx, int vertInFaceIdx) const
    { return faceIdx*maxCOS + vertInFaceIdx; }

    int subContVolIndex(int boundaryFaceIdx) const
    {
        // convert boundary index into element-local face index and
        // face-local vertex index
        int faceIdx = boundaryFaceIdx / maxCOS;
        int vertInFaceIdx = boundaryFaceIdx % maxCOS;

        // convert the element-local face index plus face-local vertex
        // index to the element-local vertex index using the reference
        // element.
        const typename Dune::GenericReferenceElementContainer<CoordScalar,dim>::value_type&
            referenceElement = Dune::GenericReferenceElements<CoordScalar,dim>::general(geometryType_);
        return referenceElement.subEntity(faceIdx, 1, vertInFaceIdx, dim);
    }

    struct SubControlVolume //! FV intersected with element
    {
        LocalPosition local; //!< local vert position
        GlobalPosition global; //!< global vert position
        Scalar volume; //!< volume of scv
        const ScvGeometry *localGeometry; //!< The geometry of the sub-control volume in local coordinates.
        Dune::FieldVector<DimVector, maxNC> grad; //! derivative of shape function associated with the sub control volume
        Dune::FieldVector<DimVector, maxNC> gradCenter; //! derivative of shape function at the center of the sub control volume
        Dune::FieldVector<Scalar, maxNC> shapeValue; //! value of shape function associated with the sub control volume
    };

    struct SubControlVolumeFace //! interior face of a sub control volume
    {
        int i,j; //!< scvf seperates corner i and j of elem
        LocalPosition ipLocal; //!< integration point in local coords
        GlobalPosition ipGlobal; //!< integration point in global coords
        DimVector normal; //!< normal on face pointing to CV j or outward of the domain with length equal to |scvf|
        Scalar area; //!< area of face
        Dune::FieldVector<DimVector, maxNC> grad; //!< derivatives of shape functions at ip
        Dune::FieldVector<Scalar, maxNC> shapeValue; //!< value of shape functions at ip
    };

    typedef SubControlVolumeFace BoundaryFace; //!< compatibility typedef

    LocalPosition elementLocal; //!< local coordinate of element center
    GlobalPosition elementGlobal; //!< global coordinate of element center
    Scalar elementVolume; //!< element volume
    SubControlVolume subContVol[maxNC]; //!< data of the sub control volumes
    SubControlVolumeFace subContVolFace[maxNE]; //!< data of the sub control volume faces
    BoundaryFace boundaryFace[maxBF]; //!< data of the boundary faces
    GlobalPosition edgeCoord[maxNE]; //!< global coordinates of the edge centers
    GlobalPosition faceCoord[maxNF]; //!< global coordinates of the face centers
    int numVertices; //!< number of verts
    int numEdges; //!< number of edges
    int numFaces; //!< number of faces (0 in < 3D)
    Dune::GeometryType geometryType_;

    const LocalFiniteElementCache feCache_;

    VcfvElementGeometry()
    {
        static bool localGeometriesInitialized = false;
        if (!localGeometriesInitialized) {
            localGeometriesInitialized = true;

            VcfvScvGeometries<Scalar, /*dim=*/1, Dune::GeometryType::cube>::init();
            VcfvScvGeometries<Scalar, /*dim=*/2, Dune::GeometryType::cube>::init();
            VcfvScvGeometries<Scalar, /*dim=*/2, Dune::GeometryType::simplex>::init();
            VcfvScvGeometries<Scalar, /*dim=*/3, Dune::GeometryType::cube>::init();
            VcfvScvGeometries<Scalar, /*dim=*/3, Dune::GeometryType::simplex>::init();
        }
    }

    void update(const GridView& gridView, const Element& e)
    {
        const Geometry& geometry = e.geometry();
        geometryType_ = geometry.type();

        const typename Dune::GenericReferenceElementContainer<CoordScalar,dim>::value_type&
            referenceElement = Dune::GenericReferenceElements<CoordScalar,dim>::general(geometryType_);

        const LocalFiniteElement
            &localFiniteElement = feCache_.get(geometryType_);

        elementVolume = geometry.volume();
        elementLocal = referenceElement.position(0,0);
        elementGlobal = geometry.global(elementLocal);

        numVertices = referenceElement.size(dim);
        numEdges = referenceElement.size(dim-1);
        numFaces = (dim<3)?0:referenceElement.size(1);

        // corners:
        for (int vert = 0; vert < numVertices; vert++) {
            subContVol[vert].local = referenceElement.position(vert, dim);
            subContVol[vert].global = geometry.global(subContVol[vert].local);
        }

        // edges:
        for (int edge = 0; edge < numEdges; edge++) {
            edgeCoord[edge] = geometry.global(referenceElement.position(edge, dim-1));
        }

        // faces:
        for (int face = 0; face < numFaces; face++) {
            faceCoord[face] = geometry.global(referenceElement.position(face, 1));
        }

        // fill sub control volume data use specialization for this
        // \todo maybe it would be a good idea to migrate everything
        // which is dependend of the grid's dimension to
        // _VcfvFVElemGeomHelper in order to benefit from more aggressive
        // compiler optimizations...
        fillSubContVolData_();

        // fill sub control volume face data:
        for (int k = 0; k < numEdges; k++) { // begin loop over edges / sub control volume faces
            int i = referenceElement.subEntity(k, dim-1, 0, dim);
            int j = referenceElement.subEntity(k, dim-1, 1, dim);
            if (numEdges == 4 && (i == 2 || j == 2))
                std::swap(i, j);
            subContVolFace[k].i = i;
            subContVolFace[k].j = j;

            // calculate the local integration point and
            // the face normal. note that since dim is a
            // constant which is known at compile time
            // the compiler can optimize away all if
            // cases which don't apply.
            LocalPosition ipLocal;
            DimVector diffVec;
            if (dim==1) {
                subContVolFace[k].ipLocal = 0.5;
                subContVolFace[k].normal = 1.0;
                ipLocal = subContVolFace[k].ipLocal;
            }
            else if (dim==2) {
                ipLocal = referenceElement.position(k, dim-1) + elementLocal;
                ipLocal *= 0.5;
                subContVolFace[k].ipLocal = ipLocal;
                for (int m = 0; m < dimWorld; ++m)
                    diffVec[m] = elementGlobal[m] - edgeCoord[k][m];
                subContVolFace[k].normal[0] = diffVec[1];
                subContVolFace[k].normal[1] = -diffVec[0];

                for (int m = 0; m < dimWorld; ++m)
                    diffVec[m] = subContVol[j].global[m] - subContVol[i].global[m];
                // make sure the normal points to the right direction
                if (subContVolFace[k].normal * diffVec < 0)
                    subContVolFace[k].normal *= -1;

            }
            else if (dim==3) {
                int leftFace;
                int rightFace;
                getFaceIndices(numVertices, k, leftFace, rightFace);
                ipLocal = referenceElement.position(k, dim-1) + elementLocal
                    + referenceElement.position(leftFace, 1)
                    + referenceElement.position(rightFace, 1);
                ipLocal *= 0.25;
                subContVolFace[k].ipLocal = ipLocal;
                normalOfQuadrilateral3D(subContVolFace[k].normal,
                                        edgeCoord[k], faceCoord[rightFace],
                                        elementGlobal, faceCoord[leftFace]);
            }

            // get the global integration point and the Jacobian inverse
            subContVolFace[k].ipGlobal = geometry.global(ipLocal);

            typedef Dune::FieldVector< Scalar, 1 > ShapeValue;
            std::vector<ShapeValue>  shapeVal;
            localFiniteElement.localBasis().evaluateFunction(subContVolFace[k].ipLocal, shapeVal);
            for (int i = 0; i < numVertices; ++i) {
                subContVolFace[k].shapeValue[i] = shapeVal[i];
            }

        } // end loop over edges / sub control volume faces

        // fill boundary face data:
        IntersectionIterator endit = gridView.iend(e);
        for (IntersectionIterator it = gridView.ibegin(e); it != endit; ++it) {
            if (!it->boundary())
                continue;

            int face = it->indexInInside();
            int numVerticesOfFace = referenceElement.size(face, 1, dim);
            for (int vertInFace = 0; vertInFace < numVerticesOfFace; vertInFace++)
            {
                int vertInElement = referenceElement.subEntity(face, 1, vertInFace, dim);
                int bfIdx = boundaryFaceIndex(face, vertInFace);

                if (dim == 1) {
                    boundaryFace[bfIdx].ipLocal = referenceElement.position(vertInElement, dim);
                    boundaryFace[bfIdx].area = 1.0;
                }
                else if (dim == 2) {
                    boundaryFace[bfIdx].ipLocal = referenceElement.position(vertInElement, dim)
                        + referenceElement.position(face, 1);
                    boundaryFace[bfIdx].ipLocal *= 0.5;
                    boundaryFace[bfIdx].area = 0.5*it->geometry().volume();
                }
                else if (dim == 3) {
                    int leftEdge;
                    int rightEdge;
                    getEdgeIndices(numVertices, face, vertInElement, leftEdge, rightEdge);
                    boundaryFace[bfIdx].ipLocal = referenceElement.position(vertInElement, dim)
                        + referenceElement.position(face, 1)
                        + referenceElement.position(leftEdge, dim-1)
                        + referenceElement.position(rightEdge, dim-1);
                    boundaryFace[bfIdx].ipLocal *= 0.25;
                    boundaryFace[bfIdx].area = quadrilateralArea3D(subContVol[vertInElement].global,
                                                                   edgeCoord[rightEdge], faceCoord[face], edgeCoord[leftEdge]);
                }
                else
                    DUNE_THROW(Dune::NotImplemented, "VcfvElementGeometry for dim = " << dim);

                boundaryFace[bfIdx].ipGlobal = geometry.global(boundaryFace[bfIdx].ipLocal);
                boundaryFace[bfIdx].i = vertInElement;
                boundaryFace[bfIdx].j = vertInElement;

                // ASSUME constant normal
                Dune::FieldVector<CoordScalar, dim-1> localDimM1(0);
                for (int i = 0; i < dimWorld; ++i)
                    boundaryFace[bfIdx].normal[i] = Scalar(it->unitOuterNormal(localDimM1)[i]);
                boundaryFace[bfIdx].normal *= boundaryFace[bfIdx].area;

                typedef Dune::FieldVector< Scalar, 1 > ShapeValue;
                std::vector<ShapeValue>  shapeVal;
                localFiniteElement.localBasis().evaluateFunction(boundaryFace[bfIdx].ipLocal, shapeVal);
                for (int i = 0; i < numVertices; ++i) {
                    boundaryFace[bfIdx].shapeValue[i] = shapeVal[i];
                }
            }
        }

        updateScvLocalGeometry(e);
        updateScvfGradients(e, localFiniteElement);
        updateScvGradients(e, localFiniteElement);
        updateScvCenterGradients(e, localFiniteElement);
    }

    const LocalFiniteElement &localFiniteElement() const
    { return feCache_.get(geometryType_); }

    void updateScvLocalGeometry(const Element &element)
    {
        auto geomType = element.geometry().type();

        // get the local geometries of the sub control volumes
        if (geomType.isTriangle() || geomType.isTetrahedron()) {
            for (int vertIdx = 0; vertIdx < numVertices; ++vertIdx)
                subContVol[vertIdx].localGeometry = &VcfvScvGeometries<Scalar, dim, Dune::GeometryType::simplex>::get(vertIdx);
        }
        else if (geomType.isLine() || geomType.isQuadrilateral() || geomType.isHexahedron()) {
            for (int vertIdx = 0; vertIdx < numVertices; ++vertIdx)
                subContVol[vertIdx].localGeometry = &VcfvScvGeometries<Scalar, dim, Dune::GeometryType::cube>::get(vertIdx);
        }
        else
            DUNE_THROW(Dune::NotImplemented,
                       "SCV geometries for non hexahedron elements");
    }

    /*!
     * \brief Update the finite element gradients at the positions of
     *        the integration points of the sub-control volume faces.
     */
    void updateScvfGradients(const Element &element, const LocalFiniteElement &localFiniteElement)
    {
        const auto &geom = element.geometry();
        std::vector<ShapeJacobian> localJac;

        for (int scvfIdx = 0; scvfIdx < numEdges; ++ scvfIdx) {
            localFiniteElement.localBasis().evaluateJacobian(subContVolFace[scvfIdx].ipLocal, localJac);

            const auto jacInvT =
                geom.jacobianInverseTransposed(subContVolFace[scvfIdx].ipLocal);
            for (int vert = 0; vert < numVertices; vert++)
                jacInvT.mv(localJac[vert][0], subContVolFace[scvfIdx].grad[vert]);
        }

    }

    /*!
     * \brief Update the finite element gradients at the positions of
     *        the element's vertices.
     */
    void updateScvGradients(const Element &element, const LocalFiniteElement &localFiniteElement)
    {
        const auto &geom = element.geometry();
        std::vector<ShapeJacobian> localJac;

        for (int scvIdx = 0; scvIdx < numVertices; ++ scvIdx) {
            localFiniteElement.localBasis().evaluateJacobian(subContVol[scvIdx].local, localJac);

            const auto jacInvT =
                geom.jacobianInverseTransposed(subContVol[scvIdx].local);
            for (int vert = 0; vert < numVertices; vert++)
                jacInvT.mv(localJac[vert][0], subContVol[scvIdx].grad[vert]);
        }

    }

    /*!
     * \brief Update the finite element gradients at the centers of
     *        the sub-control volumes vertices.
     */
    void updateScvCenterGradients(const Element &element, const LocalFiniteElement &localFiniteElement)
    {
        const auto &geom = element.geometry();
        std::vector<ShapeJacobian> localJac;

        for (int scvIdx = 0; scvIdx < numVertices; ++ scvIdx) {
            const auto &localCenter = subContVol[scvIdx].localGeometry->center();
            localFiniteElement.localBasis().evaluateJacobian(localCenter, localJac);

            const auto jacInvT =
                geom.jacobianInverseTransposed(localCenter);
            for (int vert = 0; vert < numVertices; vert++) {
                jacInvT.mv(localJac[vert][0], subContVol[scvIdx].gradCenter[vert]);
            }
        }
    }

private:
    void fillSubContVolData_()
    {
        if (dim == 1) {
            // 1D
            subContVol[0].volume = 0.5*elementVolume;
            subContVol[1].volume = 0.5*elementVolume;
        }
        else if (dim == 2) {
            switch (numVertices) {
            case 3: // 2D, triangle
                subContVol[0].volume = elementVolume/3;
                subContVol[1].volume = elementVolume/3;
                subContVol[2].volume = elementVolume/3;
                break;
            case 4: // 2D, quadrilinear
                subContVol[0].volume = quadrilateralArea(subContVol[0].global, edgeCoord[2], elementGlobal, edgeCoord[0]);
                subContVol[1].volume = quadrilateralArea(subContVol[1].global, edgeCoord[1], elementGlobal, edgeCoord[2]);
                subContVol[2].volume = quadrilateralArea(subContVol[2].global, edgeCoord[0], elementGlobal, edgeCoord[3]);
                subContVol[3].volume = quadrilateralArea(subContVol[3].global, edgeCoord[3], elementGlobal, edgeCoord[1]);
                break;
            default:
                DUNE_THROW(Dune::NotImplemented, "VcfvElementGeometry dim = " << dim << ", numVertices = " << numVertices);
            }
        }
        else if (dim == 3) {
            switch (numVertices) {
            case 4: // 3D, tetrahedron
                for (int k = 0; k < numVertices; k++)
                    subContVol[k].volume = elementVolume / 4.0;
                break;
            case 5: // 3D, pyramid
                subContVol[0].volume = hexahedronVolume(subContVol[0].global,
                                                        edgeCoord[2],
                                                        faceCoord[0],
                                                        edgeCoord[0],
                                                        edgeCoord[4],
                                                        faceCoord[3],
                                                        elementGlobal,
                                                        faceCoord[1]);
                subContVol[1].volume = hexahedronVolume(subContVol[1].global,
                                                        edgeCoord[1],
                                                        faceCoord[0],
                                                        edgeCoord[2],
                                                        edgeCoord[5],
                                                        faceCoord[2],
                                                        elementGlobal,
                                                        faceCoord[3]);
                subContVol[2].volume = hexahedronVolume(subContVol[2].global,
                                                        edgeCoord[0],
                                                        faceCoord[0],
                                                        edgeCoord[3],
                                                        edgeCoord[6],
                                                        faceCoord[1],
                                                        elementGlobal,
                                                        faceCoord[4]);
                subContVol[3].volume = hexahedronVolume(subContVol[3].global,
                                                        edgeCoord[3],
                                                        faceCoord[0],
                                                        edgeCoord[1],
                                                        edgeCoord[7],
                                                        faceCoord[4],
                                                        elementGlobal,
                                                        faceCoord[2]);
                subContVol[4].volume = elementVolume -
                    subContVol[0].volume -
                    subContVol[1].volume -
                    subContVol[2].volume -
                    subContVol[3].volume;
                break;
            case 6: // 3D, prism
                subContVol[0].volume = hexahedronVolume(subContVol[0].global,
                                                        edgeCoord[3],
                                                        faceCoord[3],
                                                        edgeCoord[4],
                                                        edgeCoord[0],
                                                        faceCoord[0],
                                                        elementGlobal,
                                                        faceCoord[1]);
                subContVol[1].volume = hexahedronVolume(subContVol[1].global,
                                                        edgeCoord[5],
                                                        faceCoord[3],
                                                        edgeCoord[3],
                                                        edgeCoord[1],
                                                        faceCoord[2],
                                                        elementGlobal,
                                                        faceCoord[0]);
                subContVol[2].volume = hexahedronVolume(subContVol[2].global,
                                                        edgeCoord[4],
                                                        faceCoord[3],
                                                        edgeCoord[5],
                                                        edgeCoord[2],
                                                        faceCoord[1],
                                                        elementGlobal,
                                                        faceCoord[2]);
                subContVol[3].volume = hexahedronVolume(edgeCoord[0],
                                                        faceCoord[0],
                                                        elementGlobal,
                                                        faceCoord[1],
                                                        subContVol[3].global,
                                                        edgeCoord[6],
                                                        faceCoord[4],
                                                        edgeCoord[7]);
                subContVol[4].volume = hexahedronVolume(edgeCoord[1],
                                                        faceCoord[2],
                                                        elementGlobal,
                                                        faceCoord[0],
                                                        subContVol[4].global,
                                                        edgeCoord[8],
                                                        faceCoord[4],
                                                        edgeCoord[6]);
                subContVol[5].volume = hexahedronVolume(edgeCoord[2],
                                                        faceCoord[1],
                                                        elementGlobal,
                                                        faceCoord[2],
                                                        subContVol[5].global,
                                                        edgeCoord[7],
                                                        faceCoord[4],
                                                        edgeCoord[8]);
                break;
            case 8: // 3D, hexahedron
                subContVol[0].volume = hexahedronVolume(subContVol[0].global,
                                                        edgeCoord[6],
                                                        faceCoord[4],
                                                        edgeCoord[4],
                                                        edgeCoord[0],
                                                        faceCoord[2],
                                                        elementGlobal,
                                                        faceCoord[0]);
                subContVol[1].volume = hexahedronVolume(subContVol[1].global,
                                                        edgeCoord[5],
                                                        faceCoord[4],
                                                        edgeCoord[6],
                                                        edgeCoord[1],
                                                        faceCoord[1],
                                                        elementGlobal,
                                                        faceCoord[2]);
                subContVol[2].volume = hexahedronVolume(subContVol[2].global,
                                                        edgeCoord[4],
                                                        faceCoord[4],
                                                        edgeCoord[7],
                                                        edgeCoord[2],
                                                        faceCoord[0],
                                                        elementGlobal,
                                                        faceCoord[3]);
                subContVol[3].volume = hexahedronVolume(subContVol[3].global,
                                                        edgeCoord[7],
                                                        faceCoord[4],
                                                        edgeCoord[5],
                                                        edgeCoord[3],
                                                        faceCoord[3],
                                                        elementGlobal,
                                                        faceCoord[1]);
                subContVol[4].volume = hexahedronVolume(edgeCoord[0],
                                                        faceCoord[2],
                                                        elementGlobal,
                                                        faceCoord[0],
                                                        subContVol[4].global,
                                                        edgeCoord[10],
                                                        faceCoord[5],
                                                        edgeCoord[8]);
                subContVol[5].volume = hexahedronVolume(edgeCoord[1],
                                                        faceCoord[1],
                                                        elementGlobal,
                                                        faceCoord[2],
                                                        subContVol[5].global,
                                                        edgeCoord[9],
                                                        faceCoord[5],
                                                        edgeCoord[10]);
                subContVol[6].volume = hexahedronVolume(edgeCoord[2],
                                                        faceCoord[0],
                                                        elementGlobal,
                                                        faceCoord[3],
                                                        subContVol[6].global,
                                                        edgeCoord[8],
                                                        faceCoord[5],
                                                        edgeCoord[11]);
                subContVol[7].volume = hexahedronVolume(edgeCoord[3],
                                                        faceCoord[3],
                                                        elementGlobal,
                                                        faceCoord[1],
                                                        subContVol[7].global,
                                                        edgeCoord[11],
                                                        faceCoord[5],
                                                        edgeCoord[9]);
                break;
            default:
                DUNE_THROW(Dune::NotImplemented, "VcfvElementGeometry for dim = " << dim << ", numVertices = " << numVertices);
            }
        }
        else
            DUNE_THROW(Dune::NotImplemented, "VcfvElementGeometry for dim = " << dim);
    }

};

}


#endif

