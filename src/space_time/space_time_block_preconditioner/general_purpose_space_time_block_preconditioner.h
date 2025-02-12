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
// Header file for SpaceTime elements
#ifndef OOMPH_GENERAL_PURPOSE_SPACE_TIME_BLOCK_PRECONDITIONER_HEADER
#define OOMPH_GENERAL_PURPOSE_SPACE_TIME_BLOCK_PRECONDITIONER_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <oomph-lib-config.h>
#endif

// Oomph-lib headers
#include "generic/iterative_linear_solver.h"
#include "generic/general_purpose_block_preconditioners.h"

// Add in the subsidiary preconditioners
#include "general_purpose_space_time_subsidiary_block_preconditioner.h"

/// /////////////////////////////////////////////////////////////////////////
/// /////////////////////////////////////////////////////////////////////////
/// /////////////////////////////////////////////////////////////////////////

namespace oomph
{
  //=============================================================================
  /// General purpose block tridiagonal preconditioner. By default
  /// SuperLUPreconditioner (or SuperLUDistPreconditioner) is used to solve the
  /// subsidiary systems, but other preconditioners can be used by setting them
  /// using passing a pointer to a function of type
  /// SubsidiaryPreconditionerFctPt to the method
  /// subsidiary_preconditioner_function_pt().
  //=============================================================================
  template<typename MATRIX>
  class ExactDGPBlockPreconditioner
    : public GeneralPurposeBlockPreconditioner<MATRIX>
  {
  public:
    /// Constructor. (By default this preconditioner is upper triangular).
    ExactDGPBlockPreconditioner() : GeneralPurposeBlockPreconditioner<MATRIX>()
    {
      // The preconditioner has been created but it hasn't been set up yet
      Preconditioner_has_been_setup = false;

      // By default, don't store the memory statistics of this preconditioner
      Compute_memory_statistics = false;

      // Initialise the value of Memory_usage_in_bytes
      Memory_usage_in_bytes = 0.0;
    } // End of ExactDGPBlockPreconditioner


    /// Destructor - delete the preconditioner matrices
    virtual ~ExactDGPBlockPreconditioner()
    {
      // Forward the call to a helper clean-up function
      this->clean_up_memory();
    } // End of ~ExactDGPBlockPreconditioner


    /// Clean up the memory
    virtual void clean_up_memory()
    {
      // Clean up the base class too
      GeneralPurposeBlockPreconditioner<MATRIX>::clean_up_memory();
    } // End of clean_up_memory


    /// Broken copy constructor
    ExactDGPBlockPreconditioner(const ExactDGPBlockPreconditioner&) = delete;

    /// Broken assignment operator
    void operator=(const ExactDGPBlockPreconditioner&) = delete;

    /// Apply preconditioner to r
    void preconditioner_solve(const DoubleVector& r, DoubleVector& z);


    /// Setup the preconditioner
    void setup();


    /// Document the memory usage
    void enable_doc_memory_usage()
    {
      /// Set the appropriate flag to true
      Compute_memory_statistics = true;
    } // End of enable_doc_memory_usage


    /// Don't document the memory usage!
    void disable_doc_memory_usage()
    {
      /// Set the appropriate flag to false
      Compute_memory_statistics = false;
    } // End of disable_doc_memory_usage


    /// Get the memory statistics
    double get_memory_usage_in_bytes()
    {
      // Has the preconditioner even been set up yet?
      if (Preconditioner_has_been_setup)
      {
        // Were we meant to compute the statistics?
        if (Compute_memory_statistics)
        {
          // Return the appropriate variable value
          return Memory_usage_in_bytes;
        }
        else
        {
          // Allocate storage for an output stream
          std::ostringstream warning_message_stream;

          // Create a warning message
          warning_message_stream
            << "The memory statistics have not been calculated "
            << "so I'm returning\nthe value zero." << std::endl;

          // Give the user a warning
          OomphLibWarning(warning_message_stream.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);

          // Return the value zero
          return 0.0;
        }
      }
      // If the preconditioner hasn't been set up yet
      else
      {
        // Allocate storage for an output stream
        std::ostringstream warning_message_stream;

        // Create a warning message
        warning_message_stream
          << "The preconditioner hasn't even been set up yet "
          << "so I'm returning\nthe value zero." << std::endl;

        // Give the user a warning
        OomphLibWarning(warning_message_stream.str(),
                        OOMPH_CURRENT_FUNCTION,
                        OOMPH_EXCEPTION_LOCATION);

        // Return the value zero
        return 0.0;
      } // if (Preconditioner_has_been_setup)
    } // End of get_memory_usage_in_bytes

  private:
    /// Control flag is true if the preconditioner has been setup
    /// (used so we can wipe the data when the preconditioner is called again)
    bool Preconditioner_has_been_setup;

    /// Flag to indicate whether or not to record the memory statistics
    /// this preconditioner
    bool Compute_memory_statistics;

    /// Storage for the memory usage of the solver if the flag above
    /// is set to true (in bytes)
    double Memory_usage_in_bytes;
  };

  //=============================================================================
  /// General purpose block triangular preconditioner. By default this
  /// operates as an upper triangular preconditioner. Also, by default
  /// SuperLUPreconditioner (or SuperLUDistPreconditioner) is used to solve the
  /// subsidiary systems, but other preconditioners can be used by setting them
  /// using passing a pointer to a function of type
  /// SubsidiaryPreconditionerFctPt to the method
  /// subsidiary_preconditioner_function_pt().
  //=============================================================================
  template<typename MATRIX>
  class BandedBlockTriangularPreconditioner
    : public GeneralPurposeBlockPreconditioner<MATRIX>
  {
  public:
    /// Constructor. (By default this preconditioner is upper triangular).
    BandedBlockTriangularPreconditioner()
      : GeneralPurposeBlockPreconditioner<MATRIX>()
    {
      // Default to upper triangular
      Upper_triangular = true;

      // Default is to assume every block above the diagonal is non-empty
      Block_bandwidth = -1;

      // The preconditioner has been created but it hasn't been set up yet
      Preconditioner_has_been_setup = false;

      // By default, don't store the memory statistics of this preconditioner
      Compute_memory_statistics = false;

      // Initialise the value of Memory_usage_in_bytes
      Memory_usage_in_bytes = 0.0;
    } // End of BandedBlockTriangularPreconditioner


    /// Destructor - delete the preconditioner matrices
    virtual ~BandedBlockTriangularPreconditioner()
    {
      // Forward the call to a helper clean-up function
      this->clean_up_memory();
    } // End of ~BandedBlockTriangularPreconditioner


    /// Clean up the memory
    virtual void clean_up_memory()
    {
      // Delete anything in Off_diagonal_matrix_vector_products
      for (unsigned i = 0, ni = Off_diagonal_matrix_vector_products.nrow();
           i < ni;
           i++)
      {
        for (unsigned j = 0, nj = Off_diagonal_matrix_vector_products.ncol();
             j < nj;
             j++)
        {
          // Delete the matrix-vector product
          delete Off_diagonal_matrix_vector_products(i, j);

          // Make it a null pointer
          Off_diagonal_matrix_vector_products(i, j) = 0;
        }
      } // for (unsigned i=0,ni=Off_diagonal_matrix_vector_products.nrow();...

      // Clean up the base class too
      GeneralPurposeBlockPreconditioner<MATRIX>::clean_up_memory();
    } // End of clean_up_memory


    /// Broken copy constructor
    BandedBlockTriangularPreconditioner(
      const BandedBlockTriangularPreconditioner&) = delete;

    /// Broken assignment operator
    void operator=(const BandedBlockTriangularPreconditioner&) = delete;

    /// Apply preconditioner to r
    void preconditioner_solve(const DoubleVector& r, DoubleVector& z);


    /// Setup the preconditioner
    void setup();


    /// Set the block bandwidth of the preconditioner
    void set_block_bandwidth(const int& block_bandwidth)
    {
      // Store it
      Block_bandwidth = block_bandwidth;
    } // End of set_block_bandwidth


    /// Get the block bandwidth of the preconditioner
    int block_bandwidth()
    {
      // Store it
      return Block_bandwidth;
    } // End of block_bandwidth


    /// Use as an upper triangular preconditioner
    void upper_triangular()
    {
      // Update the Upper_triangular flag
      Upper_triangular = true;
    } // End of upper_triangular


    /// Use as a lower triangular preconditioner
    void lower_triangular()
    {
      // Update the Upper_triangular flag
      Upper_triangular = false;
    } // End of lower_triangular


    /// Is this being used as an upper triangular preconditioner?
    bool is_upper_triangular()
    {
      // Return the value of the Upper_triangular flag
      return Upper_triangular;
    } // End of is_upper_triangular


    /// Document the memory usage
    void enable_doc_memory_usage()
    {
      /// Set the appropriate flag to true
      Compute_memory_statistics = true;
    } // End of enable_doc_memory_usage


    /// Don't document the memory usage!
    void disable_doc_memory_usage()
    {
      /// Set the appropriate flag to false
      Compute_memory_statistics = false;
    } // End of disable_doc_memory_usage


    /// Get the memory statistics
    double get_memory_usage_in_bytes()
    {
      // Has the preconditioner even been set up yet?
      if (Preconditioner_has_been_setup)
      {
        // Were we meant to compute the statistics?
        if (Compute_memory_statistics)
        {
          // Return the appropriate variable value
          return Memory_usage_in_bytes;
        }
        else
        {
          // Allocate storage for an output stream
          std::ostringstream warning_message_stream;

          // Create a warning message
          warning_message_stream
            << "The memory statistics have not been calculated "
            << "so I'm returning\nthe value zero." << std::endl;

          // Give the user a warning
          OomphLibWarning(warning_message_stream.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);

          // Return the value zero
          return 0.0;
        }
      }
      // If the preconditioner hasn't been set up yet
      else
      {
        // Allocate storage for an output stream
        std::ostringstream warning_message_stream;

        // Create a warning message
        warning_message_stream
          << "The preconditioner hasn't even been set up yet "
          << "so I'm returning\nthe value zero." << std::endl;

        // Give the user a warning
        OomphLibWarning(warning_message_stream.str(),
                        OOMPH_CURRENT_FUNCTION,
                        OOMPH_EXCEPTION_LOCATION);

        // Return the value zero
        return 0.0;
      } // if (Preconditioner_has_been_setup)
    } // End of get_memory_usage_in_bytes

  protected:
    /// Matrix of matrix-vector product operators for the off diagonals
    DenseMatrix<MatrixVectorProduct*> Off_diagonal_matrix_vector_products;

  private:
    /// Storage for the block bandwidth of the matrix. Defaults to -1
    /// indicating that we assume every off-diagonal block is non-empty. If
    /// the matrix is block diagonal then the value is zero
    int Block_bandwidth;

    /// Boolean indicating upper or lower triangular
    bool Upper_triangular;

    /// Control flag is true if the preconditioner has been setup
    /// (used so we can wipe the data when the preconditioner is called again)
    bool Preconditioner_has_been_setup;

    /// Flag to indicate whether or not to record the memory statistics
    /// this preconditioner
    bool Compute_memory_statistics;

    /// Storage for the memory usage of the solver if the flag above
    /// is set to true (in bytes)
    double Memory_usage_in_bytes;
  };
} // End of namespace oomph
#endif
