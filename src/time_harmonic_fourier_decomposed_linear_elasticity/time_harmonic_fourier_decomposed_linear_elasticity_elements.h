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

// Include guards to prevent multiple inclusion of the header
#ifndef OOMPH_FOURIER_DECOMPOSED_TIME_HARMONIC_LINEAR_ELASTICITY_ELEMENTS_HEADER
#define OOMPH_FOURIER_DECOMPOSED_TIME_HARMONIC_LINEAR_ELASTICITY_ELEMENTS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <oomph-lib-config.h>
#endif


#ifdef OOMPH_HAS_MPI
#include "mpi.h"
#endif

#include <complex>


// OOMPH-LIB headers
#include "../generic/Qelements.h"
#include "../generic/Telements.h"
#include "../generic/projection.h"
#include "../generic/error_estimator.h"


namespace oomph
{
  //=======================================================================
  /// A base class for elements that solve the Fourier decomposed (in
  /// cylindrical polars) equations of time-harmonic linear elasticity.
  //=======================================================================
  class TimeHarmonicFourierDecomposedLinearElasticityEquationsBase
    : public virtual FiniteElement
  {
  public:
    /// Return the index at which the i-th (i=0: r, i=1: z; i=2: theta)
    /// real or imag unknown displacement component is stored at the nodes.
    /// The default assignment here (u_r_real, u_z_real, ..., u_theta_imag)
    /// is appropriate for single-physics problems.
    virtual inline std::complex<unsigned> u_index_time_harmonic_fourier_decomposed_linear_elasticity(
      const unsigned i) const
    {
      return std::complex<unsigned>(i, i + 3);
    }

    /// Compute vector of FE interpolated displacement u at local coordinate s
    void interpolated_u_time_harmonic_fourier_decomposed_linear_elasticity(
      const Vector<double>& s, Vector<std::complex<double>>& disp) const
    {
      // Find number of nodes
      unsigned n_node = nnode();

      // Local shape function
      Shape psi(n_node);

      // Find values of shape function
      shape(s, psi);

      for (unsigned i = 0; i < 3; i++)
      {
        // Index at which the nodal value is stored
        std::complex<unsigned> u_nodal_index =
          u_index_time_harmonic_fourier_decomposed_linear_elasticity(i);

        // Initialise value of u
        disp[i] = std::complex<double>(0.0, 0.0);

        // Loop over the local nodes and sum
        for (unsigned l = 0; l < n_node; l++)
        {
          const std::complex<double> u_value(
            nodal_value(l, u_nodal_index.real()),
            nodal_value(l, u_nodal_index.imag()));

          disp[i] += u_value * psi[l];
        }
      }
    }

    /// Return FE interpolated displacement u[i] (i=0: r, i=1: z; i=2:
    /// theta) at local coordinate s
    std::complex<double> interpolated_u_time_harmonic_fourier_decomposed_linear_elasticity(
      const Vector<double>& s, const unsigned& i) const
    {
      // Find number of nodes
      unsigned n_node = nnode();

      // Local shape function
      Shape psi(n_node);

      // Find values of shape function
      shape(s, psi);

      // Get nodal index at which i-th velocity is stored
      std::complex<unsigned> u_nodal_index =
        u_index_time_harmonic_fourier_decomposed_linear_elasticity(i);

      // Initialise value of u
      std::complex<double> interpolated_u(0.0, 0.0);

      // Loop over the local nodes and sum
      for (unsigned l = 0; l < n_node; l++)
      {
        const std::complex<double> u_value(
          nodal_value(l, u_nodal_index.real()),
          nodal_value(l, u_nodal_index.imag()));

        interpolated_u += u_value * psi[l];
      }

      return (interpolated_u);
    }


    /// Function pointer to function that specifies the body force
    /// as a function of the Cartesian coordinates and time FCT(x,b) --
    /// x and b are  Vectors!
    typedef void (*BodyForceFctPt)(const Vector<double>& x,
                                   Vector<std::complex<double>>& b);

    /// Constructor: Set null pointers for constitutive law.
    /// Set physical parameter values to
    /// default values, and set body force to zero.
    TimeHarmonicFourierDecomposedLinearElasticityEquationsBase()
      : Omega_sq_pt(&Default_omega_sq_value),
        Youngs_modulus_pt(&Default_youngs_modulus_value),
        Nu_pt(0),
        Fourier_wavenumber_pt(0),
        Body_force_fct_pt(0)
    {
    }

    /// Access function for square of non-dim frequency
    const std::complex<double>& omega_sq() const
    {
      return *Omega_sq_pt;
    }

    /// Access function for square of non-dim frequency
    std::complex<double>*& omega_sq_pt()
    {
      return Omega_sq_pt;
    }

    /// Return the pointer to Young's modulus
    std::complex<double>*& youngs_modulus_pt()
    {
      return Youngs_modulus_pt;
    }

    /// Access function to Young's modulus
    inline std::complex<double> youngs_modulus() const
    {
      return (*Youngs_modulus_pt);
    }

    /// Access function for Poisson's ratio
    std::complex<double>& nu() const
    {
#ifdef PARANOID
      if (Nu_pt == 0)
      {
        std::ostringstream error_message;
        error_message << "No pointer to Poisson's ratio set. Please set one!\n";
        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
#endif
      return *Nu_pt;
    }

    /// Access function for pointer to Poisson's ratio
    std::complex<double>*& nu_pt()
    {
      return Nu_pt;
    }

    /// Access function for Fourier wavenumber
    int& fourier_wavenumber() const
    {
#ifdef PARANOID
      if (Fourier_wavenumber_pt == 0)
      {
        std::ostringstream error_message;
        error_message
          << "No pointer to Fourier wavenumber set. Please set one!\n";
        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
#endif
      return *Fourier_wavenumber_pt;
    }

    /// Access function for pointer to Fourier wavenumber
    int*& fourier_wavenumber_pt()
    {
      return Fourier_wavenumber_pt;
    }

    /// Access function: Pointer to body force function
    BodyForceFctPt& body_force_fct_pt()
    {
      return Body_force_fct_pt;
    }

    /// Access function: Pointer to body force function (const version)
    BodyForceFctPt body_force_fct_pt() const
    {
      return Body_force_fct_pt;
    }

    /// Evaluate body force at Eulerian coordinate x at present time
    /// (returns zero vector if no body force function pointer has been set)
    inline void body_force(const Vector<double>& x,
                           Vector<std::complex<double>>& b) const
    {
      // If no function has been set, return zero vector
      if (Body_force_fct_pt == 0)
      {
        // Get spatial dimension of element
        unsigned n = dim();
        for (unsigned i = 0; i < n; i++)
        {
          b[i] = std::complex<double>(0.0, 0.0);
        }
      }
      else
      {
        (*Body_force_fct_pt)(x, b);
      }
    }

    /// The number of "DOF types" that degrees of freedom in this element
    /// are sub-divided into: for now lump them all into one DOF type.
    /// Can be adjusted later
    unsigned ndof_types() const
    {
      return 1;
    }

    /// Create a list of pairs for all unknowns in this element,
    /// so that the first entry in each pair contains the global equation
    /// number of the unknown, while the second one contains the number
    /// of the "DOF type" that this unknown is associated with.
    /// (Function can obviously only be called if the equation numbering
    /// scheme has been set up.)
    void get_dof_numbers_for_unknowns(
      std::list<std::pair<unsigned long, unsigned>>& dof_lookup_list) const
    {
      // temporary pair (used to store dof lookup prior to being added
      // to list)
      std::pair<unsigned long, unsigned> dof_lookup;

      // number of nodes
      const unsigned n_node = this->nnode();

      // Integer storage for local unknown
      int local_unknown = 0;

      // Loop over the nodes
      for (unsigned n = 0; n < n_node; n++)
      {
        // Loop over dimension (real and imag displacement components)
        for (unsigned i = 0; i < 6; i++)
        {
          // If the variable is free
          local_unknown = nodal_local_eqn(n, i);

          // ignore pinned values
          if (local_unknown >= 0)
          {
            // store dof lookup in temporary pair: First entry in pair
            // is global equation number; second entry is DOF type
            dof_lookup.first = this->eqn_number(local_unknown);
            dof_lookup.second = 0;

            // add to list
            dof_lookup_list.push_front(dof_lookup);
          }
        }
      }
    }


  protected:
    /// Square of nondim frequency
    std::complex<double>* Omega_sq_pt;

    /// Pointer to the Young's modulus
    std::complex<double>* Youngs_modulus_pt;

    /// Pointer to Poisson's ratio
    std::complex<double>* Nu_pt;

    /// Pointer to Fourier wavenumber
    int* Fourier_wavenumber_pt;

    /// Pointer to body force function
    BodyForceFctPt Body_force_fct_pt;

    /// Static default value for squared frequency
    static std::complex<double> Default_omega_sq_value;

    /// Static default value for Young's modulus (1.0 -- for natural
    /// scaling, i.e. all stresses have been non-dimensionalised by
    /// the same reference Young's modulus. Setting the "non-dimensional"
    /// Young's modulus (obtained by de-referencing Youngs_modulus_pt)
    /// to a number larger than one means that the material is stiffer
    /// than assumed in the non-dimensionalisation.
    static std::complex<double> Default_youngs_modulus_value;
  };


  /// ////////////////////////////////////////////////////////////////////
  /// ////////////////////////////////////////////////////////////////////
  /// ////////////////////////////////////////////////////////////////////


  //=======================================================================
  /// A class for elements that solve the Fourier decomposed (in cylindrical
  /// polars) equations of time-harmonic linear elasticity
  //=======================================================================
  class TimeHarmonicFourierDecomposedLinearElasticityEquations
    : public TimeHarmonicFourierDecomposedLinearElasticityEquationsBase
  {
  public:
    ///  Constructor
    TimeHarmonicFourierDecomposedLinearElasticityEquations() {}

    /// Number of values required at node n.
    unsigned required_nvalue(const unsigned& n) const
    {
      return 6;
    }

    /// Return the residuals for the equations (the discretised
    /// principle of virtual displacements)
    void fill_in_contribution_to_residuals(Vector<double>& residuals)
    {
      fill_in_generic_contribution_to_residuals_fourier_decomp_time_harmonic_linear_elasticity(
        residuals, GeneralisedElement::Dummy_matrix, 0);
    }


    /// The jacobian is calculated by finite differences by default,
    /// We need only to take finite differences w.r.t. positional variables
    /// For this element
    void fill_in_contribution_to_jacobian(Vector<double>& residuals,
                                          DenseMatrix<double>& jacobian)
    {
      // Add the contribution to the residuals
      this
        ->fill_in_generic_contribution_to_residuals_fourier_decomp_time_harmonic_linear_elasticity(
          residuals, jacobian, 1);
    }


    /// Get strain tensor
    void get_strain(const Vector<double>& s,
                    DenseMatrix<std::complex<double>>& strain);

    /// Compute norm of solution: square of the L2 norm
    void compute_norm(double& norm);

    /// Output exact solution: r,z, u_r_real, u_z_real, ..., u_theta_imag
    void output_fct(std::ostream& outfile,
                    const unsigned& nplot,
                    FiniteElement::SteadyExactSolutionFctPt exact_soln_pt);

    /// Output: r,z, u_r_real, u_z_real, ..., u_theta_imag
    void output(std::ostream& outfile)
    {
      unsigned n_plot = 5;
      output(outfile, n_plot);
    }

    /// Output: r,z, u_r_real, u_z_real, ..., u_theta_imag
    void output(std::ostream& outfile, const unsigned& n_plot);

    /// C-style output: r,z, u_r_real, u_z_real, ..., u_theta_imag
    void output(FILE* file_pt)
    {
      unsigned n_plot = 5;
      output(file_pt, n_plot);
    }

    /// Output:  r,z, u_r_real, u_z_real, ..., u_theta_imag
    void output(FILE* file_pt, const unsigned& n_plot);

    /// Validate against exact solution.
    /// Solution is provided via function pointer.
    /// Plot at a given number of plot points and compute L2 error
    /// and L2 norm of displacement solution over element
    void compute_error(std::ostream& outfile,
                       FiniteElement::SteadyExactSolutionFctPt exact_soln_pt,
                       double& error,
                       double& norm);


  private:
    /// Private helper function to compute residuals and (if requested
    /// via flag) also the Jacobian matrix.
    virtual void fill_in_generic_contribution_to_residuals_fourier_decomp_time_harmonic_linear_elasticity(
      Vector<double>& residuals, DenseMatrix<double>& jacobian, unsigned flag);
  };


  /// /////////////////////////////////////////////////////////////////////
  /// /////////////////////////////////////////////////////////////////////
  /// /////////////////////////////////////////////////////////////////////


  //===========================================================================
  /// An Element that solves the equations of Fourier decomposed (in cylindrical
  /// polars) time-harmonic linear elasticity, using QElements for the geometry.
  //============================================================================
  template<unsigned NNODE_1D>
  class QTimeHarmonicFourierDecomposedLinearElasticityElement
    : public virtual QElement<2, NNODE_1D>,
      public virtual TimeHarmonicFourierDecomposedLinearElasticityEquations
  {
  public:
    /// Constructor
    QTimeHarmonicFourierDecomposedLinearElasticityElement()
      : QElement<2, NNODE_1D>(),
        TimeHarmonicFourierDecomposedLinearElasticityEquations()
    {
    }

    /// Output function
    void output(std::ostream& outfile)
    {
      TimeHarmonicFourierDecomposedLinearElasticityEquations::output(outfile);
    }

    /// Output function
    void output(std::ostream& outfile, const unsigned& n_plot)
    {
      TimeHarmonicFourierDecomposedLinearElasticityEquations::output(outfile,
                                                                     n_plot);
    }


    /// C-style output function
    void output(FILE* file_pt)
    {
      TimeHarmonicFourierDecomposedLinearElasticityEquations::output(file_pt);
    }

    /// C-style output function
    void output(FILE* file_pt, const unsigned& n_plot)
    {
      TimeHarmonicFourierDecomposedLinearElasticityEquations::output(file_pt,
                                                                     n_plot);
    }
  };


  //============================================================================
  /// FaceGeometry of a linear
  /// QTimeHarmonicFourierDecomposedLinearElasticityElement element
  //============================================================================
  template<unsigned NNODE_1D>
  class FaceGeometry<
    QTimeHarmonicFourierDecomposedLinearElasticityElement<NNODE_1D>>
    : public virtual QElement<1, NNODE_1D>
  {
  public:
    /// Constructor must call the constructor of the underlying element
    FaceGeometry() : QElement<1, NNODE_1D>() {}
  };


  /// /////////////////////////////////////////////////////////////////////
  /// /////////////////////////////////////////////////////////////////////
  /// /////////////////////////////////////////////////////////////////////


  //===========================================================================
  /// An Element that solves the equations of Fourier decomposed (in cylindrical
  /// polars) time-harmonic linear elasticity, using TElements for the geometry.
  //============================================================================
  template<unsigned NNODE_1D>
  class TTimeHarmonicFourierDecomposedLinearElasticityElement
    : public virtual TElement<2, NNODE_1D>,
      public virtual TimeHarmonicFourierDecomposedLinearElasticityEquations,
      public virtual ElementWithZ2ErrorEstimator
  {
  public:
    /// Constructor
    TTimeHarmonicFourierDecomposedLinearElasticityElement()
      : TElement<2, NNODE_1D>(),
        TimeHarmonicFourierDecomposedLinearElasticityEquations()
    {
    }

    /// Output function
    void output(std::ostream& outfile)
    {
      TimeHarmonicFourierDecomposedLinearElasticityEquations::output(outfile);
    }

    /// Output function
    void output(std::ostream& outfile, const unsigned& n_plot)
    {
      TimeHarmonicFourierDecomposedLinearElasticityEquations::output(outfile,
                                                                     n_plot);
    }

    /// C-style output function
    void output(FILE* file_pt)
    {
      TimeHarmonicFourierDecomposedLinearElasticityEquations::output(file_pt);
    }

    /// C-style output function
    void output(FILE* file_pt, const unsigned& n_plot)
    {
      TimeHarmonicFourierDecomposedLinearElasticityEquations::output(file_pt,
                                                                     n_plot);
    }


    /// Number of vertex nodes in the element
    unsigned nvertex_node() const
    {
      return TElement<2, NNODE_1D>::nvertex_node();
    }

    /// Pointer to the j-th vertex node in the element
    Node* vertex_node_pt(const unsigned& j) const
    {
      return TElement<2, NNODE_1D>::vertex_node_pt(j);
    }

    /// Order of recovery shape functions for Z2 error estimation:
    /// Same order as shape functions.
    unsigned nrecovery_order()
    {
      return NNODE_1D - 1;
    }

    /// Number of 'flux' terms for Z2 error estimation
    unsigned num_Z2_flux_terms()
    {
      // 3 Diagonal strain rates and 3 off diagonal terms for real and imag part
      return 12;
    }

    /// Get 'flux' for Z2 error recovery:   Upper triangular entries
    /// in strain tensor.
    void get_Z2_flux(const Vector<double>& s, Vector<double>& flux)
    {
#ifdef PARANOID
      unsigned num_entries = 12;
      if (flux.size() != num_entries)
      {
        std::ostringstream error_message;
        error_message << "The flux vector has the wrong number of entries, "
                      << flux.size() << ", whereas it should be " << num_entries
                      << std::endl;
        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
#endif

      // Get strain matrix
      DenseMatrix<std::complex<double>> strain(3);
      this->get_strain(s, strain);

      // Pack into flux Vector
      unsigned icount = 0;

      // Start with diagonal terms
      for (unsigned i = 0; i < 3; i++)
      {
        flux[icount] = strain(i, i).real();
        icount++;
        flux[icount] = strain(i, i).imag();
        icount++;
      }

      // Off diagonals row by row
      for (unsigned i = 0; i < 3; i++)
      {
        for (unsigned j = i + 1; j < 3; j++)
        {
          flux[icount] = strain(i, j).real();
          icount++;
          flux[icount] = strain(i, j).imag();
          icount++;
        }
      }
    }
  };


  //============================================================================
  /// FaceGeometry of a linear
  /// TTimeHarmonicFourierDecomposedLinearElasticityElement element
  //============================================================================
  template<unsigned NNODE_1D>
  class FaceGeometry<
    TTimeHarmonicFourierDecomposedLinearElasticityElement<NNODE_1D>>
    : public virtual TElement<1, NNODE_1D>
  {
  public:
    /// Constructor must call the constructor of the underlying element
    FaceGeometry() : TElement<1, NNODE_1D>() {}
  };


  /// /////////////////////////////////////////////////////////////////
  /// /////////////////////////////////////////////////////////////////
  /// /////////////////////////////////////////////////////////////////


  //==========================================================
  /// Fourier-decomposed time-harmonic linear elasticity
  /// upgraded to become projectable
  //==========================================================
  template<class TIME_HARMONIC_LINEAR_ELAST_ELEMENT>
  class ProjectableTimeHarmonicFourierDecomposedLinearElasticityElement
    : public virtual ProjectableElement<TIME_HARMONIC_LINEAR_ELAST_ELEMENT>
  {
  public:
    /// Constructor [this was only required explicitly
    /// from gcc 4.5.2 onwards...]
    ProjectableTimeHarmonicFourierDecomposedLinearElasticityElement() {}

    /// Specify the values associated with field fld.
    /// The information is returned in a vector of pairs which comprise
    /// the Data object and the value within it, that correspond to field fld.
    /// In the underlying time-harmonic linear elasticity elemements the
    /// real and complex parts of the displacements are stored
    /// at the nodal values
    Vector<std::pair<Data*, unsigned>> data_values_of_field(const unsigned& fld)
    {
      // Create the vector
      Vector<std::pair<Data*, unsigned>> data_values;

      // Loop over all nodes and extract the fld-th nodal value
      unsigned nnod = this->nnode();
      for (unsigned j = 0; j < nnod; j++)
      {
        // Add the data value associated with the velocity components
        data_values.push_back(std::make_pair(this->node_pt(j), fld));
      }

      // Return the vector
      return data_values;
    }

    /// Number of fields to be projected: 3*dim, corresponding to
    /// real and imag parts of the displacement components
    unsigned nfields_for_projection()
    {
      return 3 * this->dim();
    }

    /// Number of history values to be stored for fld-th field.
    /// (includes present value!)
    unsigned nhistory_values_for_projection(const unsigned& fld)
    {
#ifdef PARANOID
      if (fld > 5)
      {
        std::stringstream error_stream;
        error_stream << "Elements only store six fields so fld can't be"
                     << " " << fld << std::endl;
        throw OomphLibError(
          error_stream.str(), OOMPH_CURRENT_FUNCTION, OOMPH_EXCEPTION_LOCATION);
      }
#endif
      return this->node_pt(0)->ntstorage();
    }

    /// Number of positional history values: Read out from
    /// positional timestepper
    /// (Note: count includes current value!)
    unsigned nhistory_values_for_coordinate_projection()
    {
      return this->node_pt(0)->position_time_stepper_pt()->ntstorage();
    }

    /// Return Jacobian of mapping and shape functions of field fld
    /// at local coordinate s
    double jacobian_and_shape_of_field(const unsigned& fld,
                                       const Vector<double>& s,
                                       Shape& psi)
    {
      unsigned n_dim = this->dim();
      unsigned n_node = this->nnode();
      DShape dpsidx(n_node, n_dim);

      // Call the derivatives of the shape functions and return
      // the Jacobian
      return this->dshape_eulerian(s, psi, dpsidx);
    }


    /// Return interpolated field fld at local coordinate s, at time
    /// level t (t=0: present; t>0: history values)
    double get_field(const unsigned& t,
                     const unsigned& fld,
                     const Vector<double>& s)
    {
      unsigned n_node = this->nnode();

#ifdef PARANOID
      unsigned n_dim = this->node_pt(0)->ndim();
#endif

      // Local shape function
      Shape psi(n_node);

      // Find values of shape function
      this->shape(s, psi);

      // Initialise value of u
      double interpolated_u = 0.0;

      // Sum over the local nodes at that time
      for (unsigned l = 0; l < n_node; l++)
      {
#ifdef PARANOID
        unsigned nvalue = this->node_pt(l)->nvalue();
        if (nvalue != 3 * n_dim)
        {
          std::stringstream error_stream;
          error_stream
            << "Current implementation only works for non-resized nodes\n"
            << "but nvalue= " << nvalue << "!= 3 dim = " << 3 * n_dim
            << std::endl;
          throw OomphLibError(error_stream.str(),
                              OOMPH_CURRENT_FUNCTION,
                              OOMPH_EXCEPTION_LOCATION);
        }
#endif
        interpolated_u += this->nodal_value(t, l, fld) * psi[l];
      }
      return interpolated_u;
    }


    /// Return number of values in field fld
    unsigned nvalue_of_field(const unsigned& fld)
    {
      return this->nnode();
    }


    /// Return local equation number of value j in field fld.
    int local_equation(const unsigned& fld, const unsigned& j)
    {
#ifdef PARANOID
      unsigned n_dim = this->node_pt(0)->ndim();
      unsigned nvalue = this->node_pt(j)->nvalue();
      if (nvalue != 3 * n_dim)
      {
        std::stringstream error_stream;
        error_stream
          << "Current implementation only works for non-resized nodes\n"
          << "but nvalue= " << nvalue << "!= 3 dim = " << 3 * n_dim
          << std::endl;
        throw OomphLibError(
          error_stream.str(), OOMPH_CURRENT_FUNCTION, OOMPH_EXCEPTION_LOCATION);
      }
#endif
      return this->nodal_local_eqn(j, fld);
    }
  };


  //=======================================================================
  /// Face geometry for element is the same as that for the underlying
  /// wrapped element
  //=======================================================================
  template<class ELEMENT>
  class FaceGeometry<
    ProjectableTimeHarmonicFourierDecomposedLinearElasticityElement<ELEMENT>>
    : public virtual FaceGeometry<ELEMENT>
  {
  public:
    FaceGeometry() : FaceGeometry<ELEMENT>() {}
  };


  //=======================================================================
  /// Face geometry of the Face Geometry for element is the same as
  /// that for the underlying wrapped element
  //=======================================================================
  template<class ELEMENT>
  class FaceGeometry<FaceGeometry<
    ProjectableTimeHarmonicFourierDecomposedLinearElasticityElement<ELEMENT>>>
    : public virtual FaceGeometry<FaceGeometry<ELEMENT>>
  {
  public:
    FaceGeometry() : FaceGeometry<FaceGeometry<ELEMENT>>() {}
  };


} // namespace oomph


#endif
