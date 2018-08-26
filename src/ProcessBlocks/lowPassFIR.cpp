/*
 * LowPassFilter.cpp
 *
 *  Created on: Sep 18, 2016
 *      Author: evert
 */

/*
 * FineTuner.cpp
 *
 *  Created on: Sep 17, 2016
 *      Author: evert
 */

#include "lowPassFIR.hpp"

#include <vector>
#include "../Main/Variables.hpp"
#include "../Main/ProcessScript.hpp"
#include "../ProcessBlocks/LanczosCoeff.hpp"


/* ****************  class lowPassFIR  **************** */

// Construct low-pass filter.
lowPassFIR::lowPassFIR()
	:  m_state(0)
{
	ProcessScript::getInstance().attach(this, "ID");
}

//(IBlock* blkIn, unsigned int filter_order, double cutoff)
Err lowPassFIR::init(vector<tVar>& args)
{
	(void*) new mInt(this, "Order", args[0],  1, 100, "%d");
	(void*) new mDbl(this, "Cutoff", args[1],  0, 10e9, "%.4f");
    LanczosCoeff((int)args[0], (double)args[1], m_coeff);

    return blkCommon::init();
}

// Process samples.
Err lowPassFIR::process()
{
    unsigned int order = m_state.size();
    unsigned int n = m_in->size();

    m_out->resize(n);

    if (n == 0)
        return Err::eErrVectorSize;

    // NOTE: We use m_coeff the wrong way around because it is slightly
    // faster to scan forward through the array. The result is still correct
    // because the coefficients are symmetric.

    // The first few samples need data from m_state.
    unsigned int i = 0;
    for (; i < n && i < order; i++)
    {
        IQSample y = 0;
        for (unsigned int j = 0; j < order - i; j++)
            y += m_state[i+j] * m_coeff[j];
        for (unsigned int j = order - i; j <= order; j++)
            y += (*m_in)[i-order+j] * m_coeff[j];
        (*m_out)[i] = y;
    }

    // Remaining samples only need data from samples_in.
    for (; i < n; i++)
    {
        IQSample y = 0;
        IQSampleVector::const_iterator inp = m_in->begin() + i - order;
        for (unsigned int j = 0; j <= order; j++)
            y += inp[j] * m_coeff[j];
        (*m_out)[i] = y;
    }

    // Update m_state.
    if (n < order)
    {
        copy(m_state.begin() + n, m_state.end(), m_state.begin());
        copy(m_in->begin(), m_in->end(), m_state.end() - n);
    }
    else
        copy(m_in->end() - order, m_in->end(), m_state.begin());

    return blkCommon::process();
}

static lowPassFIR obj;
