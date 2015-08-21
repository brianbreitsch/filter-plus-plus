
#ifndef __FILTER_HPP__
#define __FILTER_HPP__

#include <iostream>
#include <type_traits>
#include <iterator>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

namespace fpp {

template<
    typename T, //real type
    typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
>
struct IIRFilter {

    typedef std::vector<T> Array;
    typedef typename Array::iterator ArrayIter;
    typedef typename Array::reverse_iterator ArrayRevIter;

    // coefficient arrays
    Array input_coeffs;
    Array output_coeffs;

    // input and output queues
    Array inputs;
    Array outputs;

    // iterators to keep track of position in input/output circular arrays
    ArrayIter current_input;
    ArrayIter last_output;
    
    /*
     * Constructs an IIR filter object.
     * `input_coeff_begin` and `output_coeff_begin` point to coefficient
     * arrays of size M and N respectively.
     */
    template<class IterType1, class IterType2>
    IIRFilter(IterType1 input_coeff_begin, IterType1 input_coeff_end,
            IterType2 output_coeff_begin, IterType2 output_coeff_end) {
        // set filter coefficients
        std::copy(input_coeff_begin, input_coeff_end, std::back_inserter(input_coeffs));
        std::copy(output_coeff_begin, output_coeff_end, std::back_inserter(output_coeffs));
        // set inputs/outputs to zero
        inputs.resize(input_coeffs.size());
        outputs.resize(output_coeffs.size());
        std::fill(inputs.begin(), inputs.end(), 0);
        std::fill(outputs.begin(), outputs.end(), 0);
        // set input/output iterators to some position ?? TODO
        current_input = inputs.begin();
        last_output = outputs.begin();
    }

    template<class ContainerType1, class ContainerType2>
    IIRFilter(ContainerType1& input_coeffs_, ContainerType2& output_coeffs_) :
        IIRFilter(input_coeffs_.begin(), input_coeffs_.end(), output_coeffs_.begin(), output_coeffs_.end()) {}

    /*
     * Adds input to input queue and performs inner product of filter queues with
     * filter coefficients to produce filter output, which it appends to output queue.
     * Returns filter output.
     */
    T filter(T input) {
        int n;
        T output;
        // set input
        if (++current_input == inputs.end()) {
            current_input = inputs.begin();
        }
        *current_input = input;
        // perform input inner product
        ArrayRevIter r_current = std::reverse_iterator<ArrayIter>(current_input);
        n = static_cast<int>(inputs.rend() - r_current);
        output = std::inner_product(r_current, inputs.rend(), input_coeffs.begin(), 0);
        output = std::inner_product(inputs.rbegin(), r_current, input_coeffs.begin() + n, output);
        // perform output inner product
        ArrayRevIter r_last = std::reverse_iterator<ArrayIter>(last_output);
        n = static_cast<int>(outputs.rend() - r_last);
        output = std::inner_product(r_last, outputs.rend(), output_coeffs.begin(), output);
        output = std::inner_product(outputs.rbegin(), r_last, output_coeffs.begin() + n, output);
        // set output
        ++last_output;
        if (last_output == outputs.end()) {
            last_output = outputs.begin();
        }
        *last_output = output;
        return output;
    }

}; // class IIRFilter



} // namespace fpp

#endif  // __FILTER_HPP__
