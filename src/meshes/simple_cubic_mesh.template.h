// LIC// ====================================================================
// LIC// This file forms part of oomph-lib, the object-oriented,
// LIC// multi-physics finite-element library, available
// LIC// at http://www.oomph-lib.org.
// LIC//
// LIC// Copyright (C) 2006-2021 Matthias Heil and Andrew Hazel
// LIC//
// LIC// This library is free software; you can redistribute it and/or
// LIC// modify it under the terms of the GNU Lesser General Public
// LIC// License as published by the Free Software Foundation; either
// LIC// version 2.1 of the License, or (at your option) any later version.
// LIC//
// LIC// This library is distributed in the hope that it will be useful,
// LIC// but WITHOUT ANY WARRANTY; without even the implied warranty of
// LIC// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// LIC// Lesser General Public License for more details.
// LIC//
// LIC// You should have received a copy of the GNU Lesser General Public
// LIC// License along with this library; if not, write to the Free Software
// LIC// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// LIC// 02110-1301  USA.
// LIC//
// LIC// The authors may be contacted at oomph-lib@maths.man.ac.uk.
// LIC//
// LIC//====================================================================
#ifndef OOMPH_SIMPLE_CUBIC_MESH_HEADER
#define OOMPH_SIMPLE_CUBIC_MESH_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <oomph-lib-config.h>
#endif

// Include the OOMPH-LIB header files
#include "../generic/mesh.h"
#include "../generic/matrices.h"
#include "../generic/brick_mesh.h"
#include "../generic/refineable_brick_mesh.h"

namespace oomph
{
  //=======================================================================
  /// Simple cubic 3D Brick mesh class.
  //=======================================================================
  template<class ELEMENT>
  class SimpleCubicMesh : public virtual BrickMeshBase
  {
  public:
    /// Constructor: Pass number of elements in the x, y, and z
    /// directions, and the corresponding dimensions. Assume that the back lower
    /// left corner is located at (0,0,0) Timestepper defaults to Steady.
    SimpleCubicMesh(const unsigned& nx,
                    const unsigned& ny,
                    const unsigned& nz,
                    const double& lx,
                    const double& ly,
                    const double& lz,
                    TimeStepper* time_stepper_pt = &Mesh::Default_TimeStepper)
      : Nx(nx),
        Ny(ny),
        Nz(nz),
        Xmin(0.0),
        Xmax(lx),
        Ymin(0.0),
        Ymax(ly),
        Zmin(0.0),
        Zmax(lz)
    {
      // Mesh can only be built with 3D Qelements.
      MeshChecker::assert_geometric_element<QElementGeometricBase, ELEMENT>(3);

      // Call the generic build function
      build_mesh(time_stepper_pt);
    }

    /// Constructor: Pass the number of elements in the x,y and z
    /// directions and the correspoding minimum and maximum values of the
    /// coordinates in each direction
    SimpleCubicMesh(const unsigned& nx,
                    const unsigned& ny,
                    const unsigned& nz,
                    const double& xmin,
                    const double& xmax,
                    const double& ymin,
                    const double& ymax,
                    const double& zmin,
                    const double& zmax,
                    TimeStepper* time_stepper_pt = &Mesh::Default_TimeStepper)
      : Nx(nx),
        Ny(ny),
        Nz(nz),
        Xmin(xmin),
        Xmax(xmax),
        Ymin(ymin),
        Ymax(ymax),
        Zmin(zmin),
        Zmax(zmax)
    {
      // Mesh can only be built with 3D Qelements.
      MeshChecker::assert_geometric_element<QElementGeometricBase, ELEMENT>(3);

      // Call the generic mesh constructor
      build_mesh(time_stepper_pt);
    }


    /// Access function for number of elements in x directions
    const unsigned& nx() const
    {
      return Nx;
    }

    /// Access function for number of elements in y directions
    const unsigned& ny() const
    {
      return Ny;
    }

    /// Access function for number of elements in y directions
    const unsigned& nz() const
    {
      return Nx;
    }

  protected:
    /// Number of elements in x direction
    unsigned Nx;

    /// Number of elements in y direction
    unsigned Ny;

    /// Number of elements in y direction
    unsigned Nz;

    /// Minimum value of x coordinate
    double Xmin;

    /// Maximum value of x coordinate
    double Xmax;

    /// Minimum value of y coordinate
    double Ymin;

    /// Minimum value of y coordinate
    double Ymax;

    /// Minimum value of z coordinate
    double Zmin;

    /// Maximum value of z coordinate
    double Zmax;

    /// Generic mesh construction function: contains all the hard work
    void build_mesh(TimeStepper* time_stepper_pt = &Mesh::Default_TimeStepper);
  };


  /// /////////////////////////////////////////////////////////////////////////
  /// /////////////////////////////////////////////////////////////////////////
  /// /////////////////////////////////////////////////////////////////////////


  //=======================================================================
  /// Refineable version of simple cubic 3D Brick mesh class.
  //=======================================================================
  template<class ELEMENT>
  class RefineableSimpleCubicMesh : public virtual SimpleCubicMesh<ELEMENT>,
                                    public virtual RefineableBrickMesh<ELEMENT>
  {
  public:
    /// Constructor: Pass number of elements in the x, y, and z
    /// directions, and the corresponding dimensions. Assume that the back lower
    /// left corner is located at (0,0,0) Timestepper defaults to Steady.
    RefineableSimpleCubicMesh(
      const unsigned& nx,
      const unsigned& ny,
      const unsigned& nz,
      const double& lx,
      const double& ly,
      const double& lz,
      TimeStepper* time_stepper_pt = &Mesh::Default_TimeStepper)
      : SimpleCubicMesh<ELEMENT>(nx, ny, nz, lx, ly, lz, time_stepper_pt)
    {
      // Nodal positions etc. were created in constructor for
      // base class Only need to setup octree forest
      this->setup_octree_forest();
    }


    /// Constructor: Pass the number of elements in the x,y and z
    /// directions and the correspoding minimum and maximum values of the
    /// coordinates in each direction.
    RefineableSimpleCubicMesh(
      const unsigned& nx,
      const unsigned& ny,
      const unsigned& nz,
      const double& xmin,
      const double& xmax,
      const double& ymin,
      const double& ymax,
      const double& zmin,
      const double& zmax,
      TimeStepper* time_stepper_pt = &Mesh::Default_TimeStepper)
      : SimpleCubicMesh<ELEMENT>(
          nx, ny, nz, xmin, xmax, ymin, ymax, zmin, zmax, time_stepper_pt)
    {
      // Nodal positions etc. were created in constructor for
      // base class Only need to setup octree forest
      this->setup_octree_forest();
    }
  };


} // namespace oomph

#endif
