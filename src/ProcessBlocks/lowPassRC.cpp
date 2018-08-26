/*
 * lowPassRC.cpp
 *
 *  Created on: Sep 19, 2016
 *      Author: evert
 */

#include "../Main/ProcessScript.hpp"
#include "../ProcessBlocks/lowPassRC.hpp"

// Construct 1st order low-pass IIR filter.
lowPassRC::lowPassRC()
    : blkCommon(0)
	, m_timeconst(0)
    , m_y1(0)
{
	ProcessScript::getInstance().attach(this, "D");
}

Err lowPassRC::init(vector<tVar>& args)
{
	m_timeconst = args[0];
	return blkCommon::init();
}

// Process samples.
Err lowPassRC::process()
{
    /*
     * Continuous domain:
     *   H(s) = 1 / (1 - s * timeconst)
     *
     * Discrete domain:
     *   H(z) = (1 - exp(-1/timeconst)) / (1 - exp(-1/timeconst) / z)
     */
    SampleType a1 = - exp(-1/m_timeconst);;
    SampleType b0 = 1 + a1;

    unsigned int n = m_in->size();
    m_out->resize(n);

    SampleType y = m_y1;
    for (unsigned int i = 0; i < n; i++)
    {
        SampleType x = (*m_in)[i];

        y = b0 * x - a1 * y;
        (*m_out)[i] = y;
    }

    m_y1 = y;

    return blkCommon::process();
}

static lowPassRC obj;
