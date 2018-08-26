/*
 * PhaseDiscriminator.cpp
 *
 *  Created on: Sep 18, 2016
 *      Author: evert
 */

#include "../Main/ProcessScript.hpp"
#include "../ProcessBlocks/PhaseDiscriminator.hpp"

#include <cmath>


/* ****************  class PhaseDiscriminator  **************** */

// Construct phase discriminator.
PhaseDiscriminator::PhaseDiscriminator()
    : blkCommon(0)
	, m_freq_scale_factor(0)
{
	ProcessScript::getInstance().attach(this, "D");
}

// IBlock* blkIn, double max_freq_dev
Err PhaseDiscriminator::init(vector<tVar>& args)
{
	m_freq_scale_factor = (1.0 / ((double)args[0] * 2.0 * M_PI));

	return blkCommon::init();
}


/** Fast approximation of atan function. */
SampleType PhaseDiscriminator::fast_atan(SampleType x)
{
    // http://stackoverflow.com/questions/7378187/approximating-inverse-trigonometric-funcions

    SampleType y = 1;
    SampleType p = 0;

    if (x < 0) {
        x = -x;
        y = -1;
    }

    if (x > 1) {
        p = y;
        y = -y;
        x = 1 / x;
    }

    const SampleType b = 0.596227;
    y *= (b*x + x*x) / (1 + 2*b*x + x*x);

    return (y + p) * SampleType(M_PI_2);
}

// Process samples.
Err PhaseDiscriminator::process()
{
    unsigned int n = m_in->size();
    IQSample s0 = m_last_sample;

    m_out->resize(n);

    for (unsigned int i = 0; i < n; i++)
    {
        IQSample s1((*m_in)[i]);
        IQSample d(conj(s0) * s1);

// TODO : implement fast approximation of atan2
        SampleType w = atan2(d.imag(), d.real());

        (*m_out)[i] = w * m_freq_scale_factor;
        s0 = s1;
    }

    m_last_sample = s0;

    return blkCommon::process();
}

static PhaseDiscriminator obj;

