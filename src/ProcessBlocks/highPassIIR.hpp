/*
 * highPassIIR.hpp
 *
 *  Created on: Sep 18, 2016
 *      Author: evert
 */

#ifndef HIGHPASSIIR_HPP_
#define HIGHPASSIIR_HPP_

#include "../Main/IBlock.hpp"

class highPassIIR : public blkCommon<SampleVector,SampleVector>
{
public:

	/**
	 * Construct 2nd order high-pass IIR filter.
	 *
	 * cutoff   :: High-pass cutoff relative to the sample frequency
	 *             (valid range 0.0 .. 0.5, 0.5 = Nyquist)
	 */
	highPassIIR();

	virtual ~highPassIIR();

	virtual Err init(vector<tVar>& args);

	virtual Err process();

	virtual std::string getBlkName() { return "highPassIIR"; }

	virtual IBlock* copyBlock()
	{
		highPassIIR* obj = new highPassIIR;
		*obj = *this;
		return obj;
	}

private:

	SampleType b0, b1, b2, a1, a2;
	SampleType x1, x2, y1, y2;
};

#endif /* HIGHPASSIIR_HPP_ */
