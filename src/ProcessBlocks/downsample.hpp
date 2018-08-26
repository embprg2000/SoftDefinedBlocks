/*
 * downsample.hpp
 *
 *  Created on: Sep 18, 2016
 *      Author: evert
 */

#ifndef DOWNSAMPLE_HPP_
#define DOWNSAMPLE_HPP_

#include "../Main/IBlock.hpp"

class downsample : public blkCommon<SampleVector,SampleVector>
{
public:
    /**
     * Construct low-pass filter with optional downsampling.
     *
     * filter_order :: FIR filter order
     * cutoff       :: Cutoff frequency relative to the full input sample rate
     *                 (valid range 0.0 .. 0.5)
     * downsample   :: Decimation factor (>= 1) or 1 to disable
     * integer_factor :: Enables a faster and more precise algorithm that
     *                   only works for integer downsample factors.
     *
     * The output sample rate is (input_sample_rate / downsample)
     */
    downsample();
	virtual ~downsample();

	virtual Err init(vector<tVar>& args);

	virtual Err process();

	virtual std::string getBlkName() { return "downsample"; }

	virtual IBlock* copyBlock()
	{
		downsample* obj = new downsample;
		*obj = *this;
		return obj;
	}

private:

	static short    m_ref;
    double          m_downsample;
    unsigned int    m_downsample_int;
    unsigned int    m_pos_int;
    SampleType      m_pos_frac;
    SampleVector    m_coeff;
    SampleVector    m_state;
};

#endif /* DOWNSAMPLE_HPP_ */
