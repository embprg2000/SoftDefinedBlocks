/*
 * LowPassFilter.hpp
 *
 *  Created on: Sep 18, 2016
 *      Author: evert
 */

#ifndef LOWPASSFIR_HPP_
#define LOWPASSFIR_HPP_

#include "../Main/IBlock.hpp"

class lowPassFIR : public blkCommon<IQSampleVector,IQSampleVector>
{
public:

    /**
     * Construct low-pass filter.
     *
     * filter_order :: FIR filter order.
     * cutoff       :: Cutoff frequency relative to the full sample rate
     *                 (valid range 0.0 ... 0.5).
     */
    lowPassFIR();
    virtual ~lowPassFIR() {}

    Err init(vector<tVar>& args);

	virtual Err process();

	virtual std::string getBlkName() { return "lowPassFIR"; }

	virtual IBlock* copyBlock()
	{
		lowPassFIR* obj = new lowPassFIR;
		*obj = *this;
		return obj;
	}

private:

    IQSampleVector	m_coeff;
    IQSampleVector	m_state;
};

#endif /* LOWPASSFIR_HPP_ */
