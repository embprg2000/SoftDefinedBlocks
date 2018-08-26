/*
 * IQFFT.hpp
 *
 *  Created on: Feb 14, 2017
 *      Author: evert
 */

#ifndef IQFFT_HPP_
#define IQFFT_HPP_

#include "../Main/IBlock.hpp"
#include "../Widgets/WidgetGraph.hpp"

// https://github.com/samyk/inspectrum/blob/master/fft.h -- For better performance
// https://github.com/keenerd/rtl-sdr-misc/blob/master/rtl-sdl/rtl_power_lite.c

class IQFFT : public blkCommon<IQSampleVector, IQSampleVector>
{
public:
	IQFFT();
	virtual ~IQFFT();

	virtual Err init(vector<tVar>& args);

	virtual Err process();

	virtual std::string getBlkName() { return "IQFFT"; }

	virtual IBlock* copyBlock()
	{
		IQFFT* obj = new IQFFT;
		*obj = *this;
		return obj;
	}

private:

    double  m_samples;  // Number of samples. Must be power of 2
    WidgetGraph* m_pGraph;

    void tSimpleFFT(SampleType *fftBuffer, long fftFrameSize, short sign);
};

#endif /* IQFFT_HPP_ */
