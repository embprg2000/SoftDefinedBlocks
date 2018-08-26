/*
 * FFT.cpp
 *
 *  Created on: Sep 19, 2016
 *      Author: evert
 */

#include <math.h>
#include "../Main/ProcessScript.hpp"
#include "../ProcessBlocks/FFT.hpp"
#include "../Widgets/WidgetGraph.hpp"
#include "../Widgets/WidgetContainer.hpp"

FFT::FFT()
    : blkCommon(0)
	, m_samples( 0 )
	, m_pGraph( nullptr )
{
	ProcessScript::getInstance().attach(this, "DS");
}

FFT::~FFT()
{
	//delete m_arr;
}

Err FFT::init(vector<tVar>& args)
{
	m_blkIn->getOutBuff( m_in );
	if (m_in != 0)
		m_out = m_in;
	else
		return Err::eErrWrongCast;

	m_samples = pow(2.0, (double)args[0]);
	//m_arr = new vector<double>(m_samples * 2);

	double Fs = m_blkIn->getSampleRate();
	// string divId, short width, short height1
	m_pGraph = new WidgetGraph(string("FFT") + (string)args[1], 800, 150, 10, 200);
	m_pGraph->axis(Fs/(m_samples * 2.0), -20, Fs/2.0, 60, 0, 0);
	WidgetContainer::getInst().add( m_pGraph );
	return Err::eOK;
}

// @TODO: This was just a quick fix for testing. So need to be fixed.
Err FFT::process()
{
	if (m_pGraph->isDirty() == false)
	{
		vector<SampleType> arr(m_samples * 2);

		SampleType* src = m_in->data();
		SampleType* dst = arr.data();

		for (int i=0; i<m_samples; ++i)
		{
			dst[i*2]   = src[i];
			dst[i*2+1] = 0.0;
		}

//		for (int i=0; i<m_samples; ++i)
//		{
//			dst[i]   = src[i];
//		}

		tSimpleFFT(dst, m_samples, -1);

		arr.resize( arr.size() / 4);
		for (unsigned int i=0; i<arr.size(); ++i)
			dst[i] = 20.0 * log10( abs(complex<double>(dst[i*2],dst[i*2+1])) );

		m_pGraph->graphArr("#F00000", 1, arr);
	}

    return blkCommon::process();
}

void FFT::tSimpleFFT(SampleType *fftBuffer, long fftFrameSize, short sign)
/*
	FFT routine, (C)1996 S.M.Bernsee. Sign = -1 is FFT, 1 is iFFT (inverse)
	Fills fftBuffer[0...2*fftFrameSize-1] with the Fourier transform of the
	time domain data in fftBuffer[0...2*fftFrameSize-1]. The FFT array takes
	and returns the cosine and sine parts in an interleaved manner, ie.
	fftBuffer[0] = cosPart[0], fftBuffer[1] = sinPart[0], asf. fftFrameSize
	must be a power of 2. It expects a complex input signal (see footnote 2),
	ie. when working with 'common' audio signals our input signal has to be
	passed as {in[0],0.,in[1],0.,in[2],0.,...} asf. In that case, the transform
	of the frequencies of interest is in fftBuffer[0...fftFrameSize].

	http://downloads.dspdimension.com/smbPitchShift.cpp
*/
{
	SampleType wr, wi, arg, *p1, *p2, temp;
	SampleType tr, ti, ur, ui, *p1r, *p1i, *p2r, *p2i;
	long i, bitm, j, le, le2, k;

	for (i = 2; i < 2*fftFrameSize-2; i += 2) {
		for (bitm = 2, j = 0; bitm < 2*fftFrameSize; bitm <<= 1) {
			if (i & bitm)
				j++;
			j <<= 1;
		}
		if (i < j) {
			p1 = fftBuffer+i;
			p2 = fftBuffer+j;
			temp = *p1;
			*(p1++) = *p2;
			*(p2++) = temp;
			temp = *p1;
			*p1 = *p2;
			*p2 = temp;
		}
	}
	for (k = 0, le = 2; k < (long)(log(fftFrameSize)/log(2.)+.5); k++) {
		le <<= 1;
		le2 = le>>1;
		ur = 1.0;
		ui = 0.0;
		arg = M_PI / (le2>>1);
		wr = cos(arg);
		wi = sign*sin(arg);
		for (j = 0; j < le2; j += 2) {
			p1r = fftBuffer+j; p1i = p1r+1;
			p2r = p1r+le2; p2i = p2r+1;
			for (i = j; i < 2*fftFrameSize; i += le) {
				tr = *p2r * ur - *p2i * ui;
				ti = *p2r * ui + *p2i * ur;
				*p2r = *p1r - tr; *p2i = *p1i - ti;
				*p1r += tr; *p1i += ti;
				p1r += le; p1i += le;
				p2r += le; p2i += le;
			}
			tr = ur*wr - ui*wi;
			ui = ur*wi + ui*wr;
			ur = tr;
		}
	}
}

static FFT obj;
