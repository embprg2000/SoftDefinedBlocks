/*
 * PhaseDiscriminator.hpp
 *
 *  Created on: Sep 18, 2016
 *      Author: evert
 */

#ifndef PHASEDISCRIMINATOR_HPP_
#define PHASEDISCRIMINATOR_HPP_

#include "../Main/IBlock.hpp"

/* Detect frequency by phase discrimination between successive samples. */
class PhaseDiscriminator : public blkCommon<IQSampleVector,SampleVector>
{
public:

    /**
     * Construct phase discriminator.
     *
     * max_freq_dev :: Full scale frequency deviation relative to the
     *                 full sample frequency.
     */
    PhaseDiscriminator();
    virtual ~PhaseDiscriminator() {}

    // IBlock* blkIn, double max_freq_dev
    virtual Err init(vector<tVar>& args);

    /**
     * Process samples.
     * Output is a sequence of frequency estimates, scaled such that
     * output value +/- 1.0 represents the maximum frequency deviation.
     */
	virtual Err process();

	virtual std::string getBlkName() { return "PhaseDiscriminator"; }

	virtual IBlock* copyBlock()
	{
		PhaseDiscriminator* obj = new PhaseDiscriminator;
		//*obj = *this;
		return obj;
	}

private:

    SampleType    m_freq_scale_factor;
    IQSample  m_last_sample;

    SampleType fast_atan(SampleType x);
};


#endif /* PHASEDISCRIMINATOR_HPP_ */
