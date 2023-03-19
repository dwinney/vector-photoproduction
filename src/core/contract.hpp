// The contract() function assembles different tensors into other structures
// At present these different interacitons must all be specified individually
//
// ------------------------------------------------------------------------------
// Author:       Daniel Winney (2022)
// Affiliation:  Joint Physics Analysis Center (JPAC),
//               South China Normal Univeristy (SCNU)
// Email:        dwinney@iu.alumni.edu
// ------------------------------------------------------------------------------

#ifndef CONTRACT_HPP
#define CONTRACT_HPP

#include "lorentz_tensor.hpp"
#include "dirac_spinor.hpp"
#include "dirac_matrix.hpp"

namespace jpacPhoto
{
    // ---------------------------------------------------------------------------
    // Define contract between the lorentz-scalar types first

    // For complex and dirac_matrix this is just the product
    template<class Type>
    inline Type contract(Type left, Type right)
    {
        return left * right;
    };

    // However for dirac_spinors it acts more like a dot product
    complex contract(dirac_spinor left, dirac_spinor right);

    // ---------------------------------------------------------------------------
    // Function to return a vector of all permutations of N indices
    
    std::vector<std::vector<lorentz_index>> permutations(unsigned N);
    
    // Single function to produce the metric along the diagonal
    int metric(lorentz_index mu);

    // Or given a set of permutations gives the product of arbitrary number of individual metrics
    int metric(std::vector<lorentz_index> permutations);

    // ---------------------------------------------------------------------------
    // I wrote this and it wors fine but you need to specify the template parameter 
    // which is annoying and i dont like it
    // e.g. i want to write contract(q, p) not contract<complex>(q, p);

    template<class LType, class RType, int R>
    inline auto contract(lorentz_tensor<LType,R> left, lorentz_tensor<RType,R> right)
    {
        auto sum = 0 * identity<LType>() * identity<RType>();
        for (auto perm : permutations(R))
        {
            sum += metric(perm) * contract(left(perm), right(perm));
        };
        return sum;
    };

    // So I'd rather just write the interactions between types seperately as needed lol
    
    // Two scalar vectors contracted 
    template<int R>
    inline complex contract(lorentz_tensor<complex,R> left, lorentz_tensor<complex,R> right)
    {
        complex sum = 0;
        for (auto perm : permutations(R))
        {
            sum += metric(perm) * contract(left(perm), right(perm));
        };
        return sum;
    };
};

#endif