
#ifndef __FILTER_HPP__
#define __FILTER_HPP__

#include <iostream>
#include <type_traits>
#include <iterator>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <sstream>

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
    Array numerator;
    Array denominator;

    // input and output queues
    Array inputs;
    Array outputs;

    // iterators to keep track of position in input/output circular arrays
    ArrayIter current_input;
    ArrayIter last_output;
    
    /*
     * Constructs an IIR filter object.
     * `numerator_begin` and `denominator_begin` point to coefficient
     * arrays of size M and N respectively.
     */
    template<class IterType1, class IterType2>
    IIRFilter(IterType1 numerator_begin, IterType1 numerator_end,
            IterType2 denominator_begin, IterType2 denominator_end) {
        // set filter coefficients
        std::copy(numerator_begin, numerator_end, std::back_inserter(numerator));
        std::copy(denominator_begin, denominator_end, std::back_inserter(denominator));
        // set inputs/outputs to zero
        inputs.resize(numerator.size());
        outputs.resize(denominator.size() - 1);  // outputs size is one less than denominator--first term of denominator is scaling factor...
        std::fill(inputs.begin(), inputs.end(), 0);
        std::fill(outputs.begin(), outputs.end(), 0);
        // set input/output iterators to some position
        current_input = inputs.begin();
        last_output = outputs.begin();
    }

    template<class ContainerType1, class ContainerType2>
    IIRFilter(ContainerType1& numerator, ContainerType2& denominator) :
        IIRFilter(numerator.begin(), numerator.end(), denominator.begin(), denominator.end()) {}

    /*
     * Adds input to input queue and performs inner product of filter queues with
     * filter coefficients to produce filter output, which it appends to output queue.
     * Returns filter output.
     */
    T f(T input) {
        for (auto it = inputs.rbegin(); it != inputs.rend() - 1; ++it) {
            *it = *(it + 1);
        }
        *inputs.begin() = input;
        T output = std::inner_product(numerator.begin(), numerator.end(), inputs.begin(), 0.0);
        output -= std::inner_product(denominator.begin() + 1, denominator.end(), outputs.begin(), 0.0);
        output /= denominator.front();
        for (auto it = outputs.rbegin(); it != outputs.rend() - 1; ++it) {
            *it = *(it + 1);
        }
        *outputs.begin() = output;
        return output;
    }

    /*
     * Return string representation of filter state.
     */
    std::string state() {
        std::stringstream ss;
        ss << "<IIR Filter; numerator: [";
        std::for_each(numerator.begin(), numerator.end(), [&](T coeff) { ss << coeff << ", "; });
        ss << "]; denominator: [";
        std::for_each(denominator.begin(), denominator.end(), [&](T coeff) { ss << coeff << ", "; });
        ss << "]; inputs: [";
        std::for_each(inputs.begin(), inputs.end(), [&](T val) { ss << val << ", "; });
        ss << "]; outputs: [";
        std::for_each(outputs.begin(), outputs.end(), [&](T val) { ss << val << ", "; });
        ss << "]>";
        return ss.str();
    }

}; // class IIRFilter



} // namespace fpp



        //std::rotate(inputs.begin(), inputs.end(), inputs.end() - 1);
        /*
        // perform input inner product
        ArrayRevIter r_current = std::reverse_iterator<ArrayIter>(current_input);
        n = static_cast<int>(inputs.rend() - r_current);
        output += std::inner_product(r_current, inputs.rend(), numerator.begin(), 0.);
        output += std::inner_product(inputs.rbegin(), r_current, numerator.begin() + n, 0.);

        // perform output inner product
        ArrayRevIter r_last = std::reverse_iterator<ArrayIter>(last_output);
        n = static_cast<int>(outputs.rend() - r_last);
        output -= std::inner_product(r_last, outputs.rend(), denominator.begin() + 1, 0.0);
        output -= std::inner_product(outputs.rbegin(), r_last, denominator.begin() + n + 1, 0.0);
        */
/*
    T f(T input) {
        int n;
        T output = 0;
        // set input
        if (++current_input == inputs.end()) {
            current_input = inputs.begin();
        }
        *current_input = input;

        auto num = numerator.begin();
        auto r_input = std::reverse_iterator<ArrayIter>(current_input);
        while (num != numerator.end()) {
            output += *num * *r_input;
            num++; r_input++;
            //output += *num++ * *r_input++;
            if (r_input == inputs.rend()) {
                r_input = inputs.rbegin();
            }
        }
        auto den = denominator.begin()++;
        auto r_output = std::reverse_iterator<ArrayIter>(last_output);
        while (den != denominator.end()) {
            output -= *den * *r_output;
            den++; r_output++;
            //output -= *den++ * *r_output++;
            if (r_output == outputs.rend()) {
                r_output = outputs.rbegin();
            }
        }
        //std::cout << n << " " << output << std::endl;
        std::cout << state() << std::endl;

        // divide by scaling factor (i.e. first term of denominator)
        output /= denominator.front();
        // set output
        ++last_output;
        if (last_output == outputs.end()) {
            last_output = outputs.begin();
        }
        *last_output = output;
        return output;
    }
*/

#endif  // __FILTER_HPP__
