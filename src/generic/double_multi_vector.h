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
#ifndef OOMPH_DOUBLE_MULTI_VECTOR_CLASS_HEADER
#define OOMPH_DOUBLE_MULTI_VECTOR_CLASS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <oomph-lib-config.h>
#endif

#ifdef OOMPH_HAS_MPI
#include "mpi.h"
#endif

// oomph headers
#include "double_vector.h"

// Trilinos headerss
#ifdef OOMPH_HAS_TRILINOS
#include "Teuchos_Range1D.hpp"
#endif

namespace oomph
{
  //=============================================================================
  /// A multi vector in the mathematical sense, initially developed for
  /// linear algebra type applications.
  /// If MPI then this multi vector can be distributed - its distribution is
  /// described by the LinearAlgebraDistribution object at Distribution_pt.
  /// Data is stored in a C-style pointer vector (double*)
  //=============================================================================
  class DoubleMultiVector : public DistributableLinearAlgebraObject
  {
  public:
    /// Constructor for an uninitialized DoubleMultiVector
    DoubleMultiVector()
      : Values(0), Nvector(0), Internal_values(true), Built(false)
    {
    }

    /// Constructor. Assembles a DoubleMultiVector consisting of
    /// n_vector vectors, each with a prescribed distribution.
    /// Additionally every entry can be set (with argument v -
    /// defaults to 0).
    DoubleMultiVector(const unsigned& n_vector,
                      const LinearAlgebraDistribution* const& dist_pt,
                      const double& v = 0.0)
      : Values(0), Nvector(n_vector), Internal_values(true), Built(false)
    {
      this->build(n_vector, dist_pt, v);
      this->setup_doublevector_representation();
    }

    /// Constructor. Assembles a DoubleMultiVector consisting of
    /// n_vector vectors, each with a prescribed distribution.
    /// Additionally every entry can be set (with argument v -
    /// defaults to 0).
    DoubleMultiVector(const unsigned& n_vector,
                      const LinearAlgebraDistribution& dist,
                      const double& v = 0.0)
      : Values(0), Nvector(n_vector), Internal_values(true), Built(false)
    {
      this->build(n_vector, dist, v);
      this->setup_doublevector_representation();
    }

    /// Constructor. Build a multivector using the same distribution
    /// of the input vector with n_vector columns and initialised to the value
    /// v
    DoubleMultiVector(const unsigned& n_vector,
                      const DoubleMultiVector& old_vector,
                      const double& initial_value = 0.0)
      : Values(0), Nvector(n_vector), Internal_values(true), Built(false)
    {
      this->build(n_vector, old_vector.distribution_pt(), initial_value);
      this->setup_doublevector_representation();
    }

    /// Constructor that builds a multivector from selected columns
    /// of the input multivector. The boolean controls whether it is a
    /// shallow or deep copy (default deep)
    DoubleMultiVector(const DoubleMultiVector& old_vector,
                      const std::vector<int>& index,
                      const bool& deep_copy = true)
      : Values(0), Nvector(0), Internal_values(deep_copy), Built(false)
    {
      // Build the storage based on the size of index
      unsigned n_vector = index.size();
      if (deep_copy)
      {
        // Create an entirely new data structure
        this->build(n_vector, old_vector.distribution_pt());
        // Now (deep) copy the data across
        const unsigned n_row_local = this->nrow_local();
        for (unsigned v = 0; v < n_vector; v++)
        {
          for (unsigned i = 0; i < n_row_local; i++)
          {
            Values[v][i] = old_vector(index[v], i);
          }
        }
      }
      // Otherwise it's a shallow copy
      else
      {
        this->shallow_build(n_vector, old_vector.distribution_pt());
        // Now shallow copy the pointers accross
        for (unsigned v = 0; v < n_vector; ++v)
        {
          Values[v] = old_vector.values(index[v]);
        }
      }
      this->setup_doublevector_representation();
    }

#ifdef OOMPH_HAS_TRILINOS
    /// Constructor that builds a multivector from selected columns
    /// of the input multivector and the provided range. The optional
    /// boolean specifies whether it is a shallow or deep copy. The default
    /// is that it is a deep copy.
    DoubleMultiVector(const DoubleMultiVector& old_vector,
                      const Teuchos::Range1D& index,
                      const bool& deep_copy = true)
      : Values(0), Nvector(0), Internal_values(deep_copy), Built(false)
    {
      // Build the storage based on the size of index
      unsigned n_vector = index.size();
      if (deep_copy)
      {
        // Create entirely new data structure
        this->build(n_vector, old_vector.distribution_pt());
        // Now (deep) copy the data across
        const unsigned n_row_local = this->nrow_local();
        unsigned range_index = index.lbound();
        for (unsigned v = 0; v < n_vector; v++)
        {
          for (unsigned i = 0; i < n_row_local; i++)
          {
            Values[v][i] = old_vector(range_index, i);
          }
          ++range_index;
        }
      }
      // Otherwise it's a shallow copy
      else
      {
        this->shallow_build(n_vector, old_vector.distribution_pt());
        // Shallow copy the pointers accross
        unsigned range_index = index.lbound();
        for (unsigned v = 0; v < n_vector; v++)
        {
          Values[v] = old_vector.values(range_index);
          ++range_index;
        }
      }
      this->setup_doublevector_representation();
    }
#endif

    /// Copy constructor
    DoubleMultiVector(const DoubleMultiVector& new_vector)
      : DistributableLinearAlgebraObject(),
        Values(0),
        Nvector(0),
        Internal_values(true),
        Built(false)
    {
      this->build(new_vector);
      this->setup_doublevector_representation();
    }


    /// Destructor - just calls this->clear() to delete the distribution and
    /// data
    ~DoubleMultiVector()
    {
      this->clear();
    }

    /// assignment operator (deep copy)
    void operator=(const DoubleMultiVector& old_vector)
    {
      this->build(old_vector);
      this->setup_doublevector_representation();
    }

    /// Return the number of vectors
    unsigned nvector() const
    {
      return Nvector;
    }

    /// Provide a (shallow) copy of the old vector
    void shallow_build(const DoubleMultiVector& old_vector)
    {
      // Only bother if the old_vector is not the same as current vector
      if (!(*this == old_vector))
      {
        // the vector does not own the internal data
        Internal_values = false;

        // Copy the number of vectors
        Nvector = old_vector.nvector();
        // Allocate storage for pointers to the values
        this->shallow_build(Nvector, old_vector.distribution_pt());

        // copy all the pointers accross
        if (this->distribution_built())
        {
          for (unsigned v = 0; v < Nvector; ++v)
          {
            Values[v] = old_vector.values(v);
          }
        }
      }
    }

    /// Build the storage for pointers to vectors with a given
    /// distribution, but do not populate the pointers
    void shallow_build(const unsigned& n_vector,
                       const LinearAlgebraDistribution& dist)
    {
      this->shallow_build(n_vector, &dist);
    }


    /// Build the storage for pointers to vectors with a given
    /// distribution, but do not populate the pointers
    void shallow_build(const unsigned& n_vector,
                       const LinearAlgebraDistribution* const& dist_pt)
    {
      // clean the memory
      this->clear();

      // The vector does not own the data
      Internal_values = false;

      // Set the distribution
      this->build_distribution(dist_pt);

      // update the values
      if (dist_pt->built())
      {
        // Set the number of vectors
        Nvector = n_vector;
        // Build the array of pointers to each vector's data
        Values = new double*[n_vector];
        Built = true;
      }
      else
      {
        Built = false;
      }
    }


    /// Provides a (deep) copy of the old_vector
    void build(const DoubleMultiVector& old_vector)
    {
      // Only bother if the old_vector is not the same as current vector
      if (!(*this == old_vector))
      {
        // the vector owns the internal data
        Internal_values = true;

        // Copy the number of vectors
        Nvector = old_vector.nvector();
        // reset the distribution and resize the data
        this->build(Nvector, old_vector.distribution_pt(), 0.0);

        // copy the data
        if (this->distribution_built())
        {
          unsigned n_row_local = this->nrow_local();
          double** const old_vector_values = old_vector.values();
          for (unsigned i = 0; i < n_row_local; i++)
          {
            for (unsigned v = 0; v < Nvector; v++)
            {
              Values[v][i] = old_vector_values[v][i];
            }
          }
        }
      }
    }

    /// Assembles a DoubleMultiVector
    /// with n_vector vectors, a distribution dist, if v is specified
    /// each element is set to v, otherwise each element is set to 0.0
    void build(const unsigned& n_vector,
               const LinearAlgebraDistribution& dist,
               const double& initial_value = 0.0)
    {
      this->build(n_vector, &dist, initial_value);
    }

    /// Assembles a DoubleMultiVector with n_vector vectors, each with a
    /// distribution dist, if v is specified
    /// each element is set to v, otherwise each element is set to 0.0
    void build(const unsigned& n_vector,
               const LinearAlgebraDistribution* const& dist_pt,
               const double& initial_value = 0.0)
    {
      // clean the memory
      this->clear();

      // the vector owns the internal data
      Internal_values = true;

      // Set the distribution
      this->build_distribution(dist_pt);

      // update the values
      if (dist_pt->built())
      {
        // Set the number of vectors
        Nvector = n_vector;
        // Build the array of pointers to each vector's data
        Values = new double*[n_vector];
        // Now build the contiguous array of real data
        const unsigned n_row_local = this->nrow_local();
        double* values = new double[n_row_local * n_vector];
        // set the data
        for (unsigned v = 0; v < n_vector; v++)
        {
          Values[v] = &values[v * n_row_local];
          for (unsigned i = 0; i < n_row_local; i++)
          {
            Values[v][i] = initial_value;
          }
        }
        Built = true;
      }
      else
      {
        Built = false;
      }
    }

    /// initialise the whole vector with value v
    void initialise(const double& initial_value)
    {
      if (Built)
      {
        const unsigned n_vector = this->Nvector;
        const unsigned n_row_local = this->nrow_local();

        // set the residuals
        for (unsigned v = 0; v < n_vector; v++)
        {
          for (unsigned i = 0; i < n_row_local; i++)
          {
            Values[v][i] = initial_value;
          }
        }
      }
    }

    /// initialise the vector with coefficient from the vector v.
    /// Note: The vector v must be of length
    // void initialise(const Vector<double> v);

    /// wipes the DoubleVector
    void clear()
    {
      // Return if nothing to do
      if (Values == 0)
      {
        return;
      }

      // If we are in charge of the data then delete it
      if (Internal_values)
      {
        // Delete the double storage arrays at once
        //(they were allocated as a contiguous block)
        delete[] Values[0];
      }

      // Always Delete the pointers to the arrays
      delete[] Values;

      // Then set the pointer (to a pointer) to null
      Values = 0;
      this->clear_distribution();
      Built = false;
    }

    // indicates whether this DoubleVector is built
    bool built() const
    {
      return Built;
    }

    /// Allows are external data to be used by this vector.
    /// WARNING: The size of the external data must correspond to the
    /// LinearAlgebraDistribution dist_pt argument.
    /// 1. When a rebuild method is called new internal values are created.
    /// 2. It is not possible to redistribute(...) a vector with external
    /// values .
    /// 3. External values are only deleted by this vector if
    /// delete_external_values = true.
    /*void set_external_values(const LinearAlgebraDistribution* const& dist_pt,
                             double* external_values,
                             bool delete_external_values)
     {
      // clean the memory
      this->clear();

      // Set the distribution
      this->build_distribution(dist_pt);

      // set the external values
      set_external_values(external_values,delete_external_values);
      }*/

    /// Allows are external data to be used by this vector.
    /// WARNING: The size of the external data must correspond to the
    /// distribution of this vector.
    /// 1. When a rebuild method is called new internal values are created.
    /// 2. It is not possible to redistribute(...) a vector with external
    /// values .
    /// 3. External values are only deleted by this vector if
    /// delete_external_values = true.
    /*void set_external_values(double* external_values,
                             bool delete_external_values)
     {
   #ifdef PARANOID
      // check that this distribution is setup
      if (!this->distribution_built())
       {
       // if this vector does not own the double* values then it cannot be
       // distributed.
       // note: this is not stictly necessary - would just need to be careful
       // with delete[] below.
        std::ostringstream error_message;
        error_message << "The distribution of the vector must be setup before "
                      << "external values can be set";
        throw OomphLibError(error_message.str(),
        OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
       }
   #endif
      if (Internal_values)
       {
        delete[] Values_pt;
       }
      Values_pt = external_values;
      Internal_values = delete_external_values;
      }*/

    /// The contents of the vector are redistributed to match the new
    /// distribution. In a non-MPI rebuild this method works, but does nothing.
    /// \b NOTE 1: The current distribution and the new distribution must have
    /// the same number of global rows.
    /// \b NOTE 2: The current distribution and the new distribution must have
    /// the same Communicator.
    void redistribute(const LinearAlgebraDistribution* const& dist_pt);

    /// [] access function to the (local) values of the v-th vector
    double& operator()(int v, int i) const
    {
#ifdef RANGE_CHECKING
      std::ostringstream error_message;
      bool error = false;
      if (v > int(Nvector))
      {
        error_message << "Range Error: Vector " << v
                      << "is not in the range (0," << Nvector - 1 << ")";
        error = true;
      }

      if (i >= int(this->nrow_local()))
      {
        error_message << "Range Error: " << i << " is not in the range (0,"
                      << this->nrow_local() - 1 << ")";
        error = true;
      }

      if (error)
      {
        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
#endif
      return Values[v][i];
    }

    /// == operator
    bool operator==(const DoubleMultiVector& vec)
    {
      // if vec is not setup return false
      if (vec.built() && !this->built())
      {
        return false;
      }
      else if (!vec.built() && this->built())
      {
        return false;
      }
      else if (!vec.built() && !this->built())
      {
        return true;
      }
      else
      {
        double** const v_values = vec.values();
        const unsigned n_row_local = this->nrow_local();
        const unsigned n_vector = this->nvector();
        for (unsigned v = 0; v < n_vector; ++v)
        {
          for (unsigned i = 0; i < n_row_local; ++i)
          {
            if (Values[v][i] != v_values[v][i])
            {
              return false;
            }
          }
        }
        return true;
      }
    }

    /// += operator
    void operator+=(DoubleMultiVector vec)
    {
#ifdef PARANOID
      // PARANOID check that this vector is setup
      if (!this->built())
      {
        std::ostringstream error_message;
        error_message << "This vector must be setup.";
        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
      // PARANOID check that the vector v is setup
      if (!vec.built())
      {
        std::ostringstream error_message;
        error_message << "The vector v must be setup.";
        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
      // PARANOID check that the vectors have the same distribution
      if (!(*vec.distribution_pt() == *this->distribution_pt()))
      {
        std::ostringstream error_message;
        error_message << "The vector v and this vector must have the same "
                      << "distribution.";
        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
#endif //


      double** v_values = vec.values();
      const unsigned n_vector = this->nvector();
      const unsigned n_row_local = this->nrow_local();
      for (unsigned v = 0; v < n_vector; ++v)
      {
        for (unsigned i = 0; i < n_row_local; ++i)
        {
          Values[v][i] += v_values[v][i];
        }
      }
    }

    /// -= operator
    void operator-=(DoubleMultiVector vec)
    {
#ifdef PARANOID
      // PARANOID check that this vector is setup
      if (!this->distribution_built())
      {
        std::ostringstream error_message;
        error_message << "This vector must be setup.";
        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
      // PARANOID check that the vector v is setup
      if (!vec.built())
      {
        std::ostringstream error_message;
        error_message << "The vector v must be setup.";
        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
      // PARANOID check that the vectors have the same distribution
      if (!(*vec.distribution_pt() == *this->distribution_pt()))
      {
        std::ostringstream error_message;
        error_message << "The vector v and this vector must have the same "
                      << "distribution.";
        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
#endif

      double** v_values = vec.values();
      const unsigned n_vector = this->nvector();
      const unsigned n_row_local = this->nrow_local();
      for (unsigned v = 0; v < n_vector; ++v)
      {
        for (unsigned i = 0; i < n_row_local; ++i)
        {
          Values[v][i] -= v_values[v][i];
        }
      }
    }

    /// Multiply by a scalar
    void operator*=(const double& scalar_value)
    {
#ifdef PARANOID
      // PARANOID check that this vector is setup
      if (!this->distribution_built())
      {
        std::ostringstream error_message;
        error_message << "This vector must be setup.";
        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
#endif
      const unsigned n_vector = this->nvector();
      const unsigned n_row_local = this->nrow_local();
      for (unsigned v = 0; v < n_vector; ++v)
      {
        for (unsigned i = 0; i < n_row_local; ++i)
        {
          Values[v][i] *= scalar_value;
        }
      }
    }


    /// access function to the underlying values
    double** values()
    {
      return Values;
    }

    /// access function to the underlying values (const version)
    double** values() const
    {
      return Values;
    }

    /// access function to the i-th vector's data
    double* values(const unsigned& i)
    {
      return Values[i];
    }

    /// access function to the i-th vector's data (const version)
    double* values(const unsigned& i) const
    {
      return Values[i];
    }

    /// access to the DoubleVector representatoin
    DoubleVector& doublevector(const unsigned& i)
    {
      return Internal_doublevector[i];
    }

    /// access to the DoubleVector representation (const version)
    const DoubleVector& doublevector(const unsigned& i) const
    {
      return Internal_doublevector[i];
    }

    /// output the contents of the vector
    void output(std::ostream& outfile) const
    {
      // temp pointer to values
      double** temp;

      // Number of vectors
      unsigned n_vector = this->nvector();

      // number of global row
      unsigned nrow = this->nrow();

#ifdef OOMPH_HAS_MPI

      // number of local rows
      int nrow_local = this->nrow_local();

      // gather from all processors
      if (this->distributed() &&
          this->distribution_pt()->communicator_pt()->nproc() > 1)
      {
        // number of processors
        int nproc = this->distribution_pt()->communicator_pt()->nproc();

        // number of gobal row
        unsigned nrow = this->nrow();

        // get the vector of first_row s and nrow_local s
        int* dist_first_row = new int[nproc];
        int* dist_nrow_local = new int[nproc];
        for (int p = 0; p < nproc; p++)
        {
          dist_first_row[p] = this->first_row(p);
          dist_nrow_local[p] = this->nrow_local(p);
        }

        // gather
        temp = new double*[n_vector];
        double* temp_value = new double[nrow * n_vector];
        for (unsigned v = 0; v < n_vector; v++)
        {
          temp[v] = &temp_value[v * nrow];
        }

        // Now do an all gather for each vector
        // Possibly costly in terms of extra communication, but it's only
        // output!
        for (unsigned v = 0; v < n_vector; ++v)
        {
          MPI_Allgatherv(
            Values[v],
            nrow_local,
            MPI_DOUBLE,
            temp[v],
            dist_nrow_local,
            dist_first_row,
            MPI_DOUBLE,
            this->distribution_pt()->communicator_pt()->mpi_comm());
        }

        // clean up
        delete[] dist_first_row;
        delete[] dist_nrow_local;
      }
      else
      {
        temp = Values;
      }
#else
      temp = Values;
#endif

      // output
      for (unsigned i = 0; i < nrow; i++)
      {
        outfile << i << " ";
        for (unsigned v = 0; v < n_vector; v++)
        {
          outfile << temp[v][i] << " ";
        }
        outfile << "\n";
      }

      // clean up if required
#ifdef OOMPH_HAS_MPI
      if (this->distributed() &&
          this->distribution_pt()->communicator_pt()->nproc() > 1)
      {
        delete[] temp[0];
        delete[] temp;
      }
#endif
    }

    /// output the contents of the vector
    void output(std::string filename)
    {
      // Open file
      std::ofstream some_file;
      some_file.open(filename.c_str());
      output(some_file);
      some_file.close();
    }


    /// compute the 2 norm of this vector
    void dot(const DoubleMultiVector& vec, std::vector<double>& result) const
    {
#ifdef PARANOID
      // paranoid check that the vector is setup
      if (!this->built())
      {
        std::ostringstream error_message;
        error_message << "This vector must be setup.";
        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
      if (!vec.built())
      {
        std::ostringstream error_message;
        error_message << "The input vector be setup.";
        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
      if (*this->distribution_pt() != *vec.distribution_pt())
      {
        std::ostringstream error_message;
        error_message << "The distribution of this vector and the vector vec "
                      << "must be the same."
                      << "\n\n  this: " << *this->distribution_pt()
                      << "\n  vec:  " << *vec.distribution_pt();
        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
#endif

      // compute the local norm
      unsigned nrow_local = this->nrow_local();
      int n_vector = this->nvector();
      double n[n_vector];
      for (int v = 0; v < n_vector; v++)
      {
        // Initialise
        n[v] = 0.0;
        const double* vec_values_pt = vec.values(v);
        for (unsigned i = 0; i < nrow_local; i++)
        {
          n[v] += Values[v][i] * vec_values_pt[i];
        }
      }

      // if this vector is distributed and on multiple processors then gather
#ifdef OOMPH_HAS_MPI
      double n2[n_vector];
      for (int v = 0; v < n_vector; v++)
      {
        n2[v] = n[v];
      }

      if (this->distributed() &&
          this->distribution_pt()->communicator_pt()->nproc() > 1)
      {
        MPI_Allreduce(&n,
                      &n2,
                      n_vector,
                      MPI_DOUBLE,
                      MPI_SUM,
                      this->distribution_pt()->communicator_pt()->mpi_comm());
      }
      for (int v = 0; v < n_vector; v++)
      {
        n[v] = n2[v];
      }
#endif

      result.resize(n_vector);
      for (int v = 0; v < n_vector; v++)
      {
        result[v] = n[v];
      }
    }

    /// compute the 2 norm of this vector
    void norm(std::vector<double>& result) const
    {
#ifdef PARANOID
      // paranoid check that the vector is setup
      if (!this->built())
      {
        std::ostringstream error_message;
        error_message << "This vector must be setup.";
        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
#endif

      // compute the local norm
      unsigned nrow_local = this->nrow_local();
      int n_vector = this->nvector();
      double n[n_vector];
      for (int v = 0; v < n_vector; v++)
      {
        n[v] = 0.0;
        for (unsigned i = 0; i < nrow_local; i++)
        {
          n[v] += Values[v][i] * Values[v][i];
        }
      }

      // if this vector is distributed and on multiple processors then gather
#ifdef OOMPH_HAS_MPI
      double n2[n_vector];
      for (int v = 0; v < n_vector; v++)
      {
        n2[v] = n[v];
      }
      if (this->distributed() &&
          this->distribution_pt()->communicator_pt()->nproc() > 1)
      {
        MPI_Allreduce(&n,
                      &n2,
                      n_vector,
                      MPI_DOUBLE,
                      MPI_SUM,
                      this->distribution_pt()->communicator_pt()->mpi_comm());
      }
      for (int v = 0; v < n_vector; v++)
      {
        n[v] = n2[v];
      }
#endif

      // Now sqrt the norm and fill in result
      result.resize(n_vector);
      for (int v = 0; v < n_vector; v++)
      {
        result[v] = sqrt(n[v]);
      }
    }

    /// compute the A-norm using the matrix at matrix_pt
    /*double norm(const CRDoubleMatrix* matrix_pt) const
     {
   #ifdef PARANOID
      // paranoid check that the vector is setup
      if (!this->built())
       {
        std::ostringstream error_message;
        error_message << "This vector must be setup.";
        throw OomphLibError(error_message.str(),
        OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
       }
      if (!matrix_pt->built())
       {
        std::ostringstream error_message;
        error_message << "The input matrix be built.";
        throw OomphLibError(error_message.str(),
        OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
       }
      if (*this->distribution_pt() != *matrix_pt->distribution_pt())
       {
        std::ostringstream error_message;
        error_message << "The distribution of this vector and the matrix at "
                      << "matrix_pt must be the same";
        throw OomphLibError(error_message.str(),
        OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
       }
   #endif

      // compute the matrix norm
      DoubleVector x(this->distribution_pt(),0.0);
      matrix_pt->multiply(*this,x);
      return sqrt(this->dot(x));

      }*/

  private:
    /// Setup the doublevector representation
    void setup_doublevector_representation()
    {
      const unsigned n_vector = this->nvector();
      Internal_doublevector.resize(n_vector);
      // Loop over all and set the external values of the DoubleVectors
      for (unsigned v = 0; v < n_vector; v++)
      {
        Internal_doublevector[v].set_external_values(
          this->distribution_pt(), this->values(v), false);
      }
    }

    /// the local data, need a pointer to a pointer so that the
    /// individual vectors can be extracted
    double** Values;

    /// The number of vectors
    unsigned Nvector;

    /// Boolean flag to indicate whether the vector's data (values_pt)
    /// is owned by this vector.
    bool Internal_values;

    /// indicates that the vector has been built and is usable
    bool Built;

    /// Need a vector of DoubleVectors to interface with our linear solvers
    Vector<DoubleVector> Internal_doublevector;

  }; // end of DoubleVector

} // namespace oomph


#endif
