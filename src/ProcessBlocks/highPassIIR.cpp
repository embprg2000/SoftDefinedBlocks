/*
 * highPassIIR.cpp
 *
 *  Created on: Sep 18, 2016
 *      Author: evert
 */


#include "../Main/ProcessScript.hpp"
#include "../ProcessBlocks/highPassIIR.hpp"

/* ****************  class highPassIIR  **************** */

// Construct 2nd order high-pass IIR filter.
highPassIIR::highPassIIR()
    : blkCommon(0), b0(0), b1(0), b2(0), a1(0), a2(0), x1(0), x2(0), y1(0), y2(0)
{
	ProcessScript::getInstance().attach(this, "D");
}

highPassIIR::~highPassIIR() {
	// TODO Auto-generated destructor stub
}

Err highPassIIR::init(vector<tVar>& args)
{
	double cutoff = args[0];
    typedef complex<double> CDbl;

    // Angular cutoff frequency.
    double w = 2 * M_PI * cutoff;

    // Poles 1 and 2 are a conjugate pair.
    // Continuous-domain:
    //   p_k = w / exp( (2*k + n - 1) / (2*n) * pi * j)
    CDbl p1s = w / exp((2*1 + 2 - 1) / double(2 * 2) * CDbl(0, M_PI));

    // Map poles to discrete-domain via matched Z transform.
    CDbl p1z = exp(p1s);

    // Both zeros are located in s = 0, z = 1.

    // Discrete-domain transfer function:
    //   H(z) = g * (1 - 1/z) * (1 - 1/z) / ( (1 - p1/z) * (1 - p2/z) )
    //        = g * (1 - 2/z + 1/z**2) / (1 - (p1+p2)/z + (p1*p2)/z**2)
    //
    // Note that z2 = conj(z1).
    // Therefore p1+p2 == 2*real(p1), p1*2 == abs(p1*p1), z4 = conj(z1)
    //
    b0 = 1;
    b1 = -2;
    b2 = 1;
    a1 = -2 * real(p1z);
    a2 = abs(p1z*p1z);

    // Adjust b coefficients to get unit gain at Nyquist frequency (z=-1).
    double g = (b0 - b1 + b2) / (1 - a1 + a2);
    b0 /= g;
    b1 /= g;
    b2 /= g;

    return blkCommon::init();;
}

// Process samples.
Err highPassIIR::process()
{
    unsigned int n = m_in->size();

    m_out->resize(n);

    for (unsigned int i = 0; i < n; i++)
    {
        SampleType x = (*m_in)[i];
        SampleType y = b0 * x + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
        x2 = x1; x1 = x;
        y2 = y1; y1 = y;
        (*m_out)[i] = y;
    }
    blkCommon::process();
    return Err(Err::eOK);
}

static highPassIIR obj;
