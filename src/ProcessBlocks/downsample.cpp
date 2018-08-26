/*
 * downsample.cpp
 *
 *  Created on: Sep 18, 2016
 *      Author: evert
 */

#include <cassert>
#include "../Main/Variables.hpp"
#include "../Main/ProcessScript.hpp"
#include "../ProcessBlocks/LanczosCoeff.hpp"
#include "../ProcessBlocks/downsample.hpp"

/* ****************  class downsample  **************** */

// Construct low-pass filter with optional downsampling.
downsample::downsample()
	: blkCommon(0)
	, m_downsample(0)
    , m_downsample_int(0)
    , m_pos_int(0)
    , m_pos_frac(0)
    , m_state(0)
{
	if (m_ref == -1)
	{
		ProcessScript::getInstance().attach(this, "IDDI");
		++m_ref;
	}
	++m_ref;
}

downsample::~downsample()
{
	--m_ref;
}
// unsigned int filter_order, double cutoff, double downsample, bool integer_factor
Err downsample::init(vector<tVar>& args)
{
	m_downsample = args[2];
    m_downsample_int = (args[3] ? lrint( (double)args[2] ) : 0);
    m_pos_int  = 0;
    m_pos_frac = 0;
    m_state.resize( (int)args[0] );

    assert((double)args[2] >= 1);
    assert((int)args[0] > 1);

	(void*) new mInt(this, "Order", args[0],  1, 100, "%d");
	(void*) new mDbl(this, "Cutoff", args[1],  0, 10e9, "%.4f");
	(void*) new mDbl(this, "Downsample", args[2],  0, 200, "%3.4f");

    // Force the first coefficient to zero and append an extra zero at the
    // end of the array. This ensures we can always obtain (filter_order+1)
    // coefficients by linear interpolation between adjacent array elements.
    LanczosCoeff((int)args[0] - 1.0, (double)args[1], m_coeff);
    m_coeff.insert(m_coeff.begin(), 0);
    m_coeff.push_back(0);

	m_sampleRate = 1.0 / m_downsample;
	return blkCommon::init();;
}

// Process samples.
Err downsample::process()
{
    unsigned int order = m_state.size();
    unsigned int n = m_in->size();

    if (m_downsample_int != 0)
    {

        // Integer downsample factor, no linear interpolation.
        // This is relatively simple.

        unsigned int p = m_pos_int;
        unsigned int pstep = m_downsample_int;
        unsigned int n_out = int((n - p + pstep - 1) / pstep);

        m_out->resize(n_out);

        // The first few samples need data from m_state.
        unsigned int i = 0;
        for (; p < n && p < order; p += pstep, i++) {
            SampleType y = 0;
            for (unsigned int j = 1; j <= p; j++)
                y += (*m_in)[p-j] * m_coeff[j];
            for (unsigned int j = p + 1; j <= order; j++)
                y += m_state[order+p-j] * m_coeff[j];
            (*m_out)[i] = y;
        }

        // Remaining samples only need data from samples_in.
        for (; p < n; p += pstep, i++) {
            SampleType y = 0;
            for (unsigned int j = 1; j <= order; j++)
                y += (*m_in)[p-j] * m_coeff[j];
            (*m_out)[i] = y;
        }

        assert(i == m_out->size());

        // Update index of start position in text sample block.
        m_pos_int = p - n;

    }
    else
    {

        // Fractional downsample factor via linear interpolation of
        // the FIR coefficient table. This is a bitch.

        // Estimate number of output samples we can produce in this run.
        SampleType p = m_pos_frac;
        SampleType pstep = m_downsample;
        unsigned int n_out = int(2 + n / pstep);

        m_out->resize(n_out);

        // Produce output samples.
        unsigned int i = 0;
        SampleType pf = p;
        unsigned int pi = int(pf);
        while (pi < n) {
            SampleType k1 = pf - pi;
            SampleType k0 = 1 - k1;

            SampleType y = 0;
            for (unsigned int j = 0; j <= order; j++) {
                SampleType k = m_coeff[j] * k0 + m_coeff[j+1] * k1;
                SampleType s = (j <= pi) ? (*m_in)[pi-j] : m_state[order+pi-j];
                y += k * s;
            }
            (*m_out)[i] = y;

            i++;
            pf = p + i * pstep;
            pi = int(pf);
        }

        // We may overestimate the number of samples by 1 or 2.
        assert(i <= n_out && i + 2 >= n_out);
        m_out->resize(i);

        // Update fractional index of start position in text sample block.
        // Limit to 0 to avoid catastrophic results of rounding errors.
        m_pos_frac = pf - n;
        if (m_pos_frac < 0)
            m_pos_frac = 0;
    }

    // Update m_state.
    if (n < order)
    {
        copy(m_state.begin() + n, m_state.end(), m_state.begin());
        copy(m_in->begin(), m_in->end(), m_state.end() - n);
    }
    else
        copy(m_in->end() - order, m_in->end(), m_state.begin());

    return blkCommon::process();;
}

short downsample::m_ref = -1;
static downsample obj;
