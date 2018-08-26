/*
 * FFT.hpp
 *
 *  Created on: Sep 19, 2016
 *      Author: evert
 */

#ifndef FFT_HPP_
#define FFT_HPP_

#include "../Main/IBlock.hpp"
#include "../Widgets/WidgetGraph.hpp"

// https://github.com/samyk/inspectrum/blob/master/fft.h -- For better performance
// https://github.com/keenerd/rtl-sdr-misc/blob/master/rtl-sdl/rtl_power_lite.c

class FFT : public blkCommon<SampleVector,SampleVector>
{
public:
	FFT();
	virtual ~FFT();

	virtual Err init(vector<tVar>& args);

	virtual Err process();

	virtual std::string getBlkName() { return "FFT"; }

	virtual IBlock* copyBlock()
	{
		FFT* obj = new FFT;
		*obj = *this;
		return obj;
	}

private:

    double  m_samples;  // Number of samples. Must be power of 2
    WidgetGraph* m_pGraph;

    void tSimpleFFT(SampleType *fftBuffer, long fftFrameSize, short sign);
};

#endif /* FFT_HPP_ */
