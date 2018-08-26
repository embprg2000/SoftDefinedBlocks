/*
 * LanczosCoeff.hpp
 *
 *  Created on: Sep 18, 2016
 *      Author: evert
 */

#ifndef LANCZOSCOEFF_HPP_
#define LANCZOSCOEFF_HPP_

#include <cmath>
#include <vector>


/** Prepare Lanczos FIR filter coefficients. */
template <class T>
static void LanczosCoeff(unsigned int filter_order, double cutoff,  std::vector<T>& coeff)
{
    coeff.resize(filter_order + 1);

    // Prepare Lanczos FIR filter.
    //   t[i]     =  (i - order/2)
    //   coeff[i] =  Sinc(2 * cutoff * t[i]) * Sinc(t[i] / (order/2 + 1))
    //   coeff    /= sum(coeff)

    double ysum = 0.0;

    // Calculate filter kernel.
    for (int i = 0; i <= (int)filter_order; i++)
    {
    	double y;
    	int t2 = 2 * i - filter_order;

    	if (t2 == 0)
    		y = 1.0;
    	else
    	{
    		double x1 = cutoff * t2;
    		double x2 = t2 / double(filter_order + 2);
    		y = ( sin(M_PI * x1) / M_PI / x1 ) * ( sin(M_PI * x2) / M_PI / x2 );
    	}
    	coeff[i] = y;
    	ysum += y;
    }

    // Apply correction factor to ensure unit gain at DC.
    for (unsigned i = 0; i <= filter_order; i++) {
        coeff[i] /= ysum;
    }
}

#endif /* LANCZOSCOEFF_HPP_ */
