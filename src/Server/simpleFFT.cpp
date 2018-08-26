#include <math.h>
#include "../Server/simpleFFT.hpp"

template<typename T>
void tSimpleFFT(T *fftBuffer, long fftFrameSize, long sign)
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
	T wr, wi, arg, *p1, *p2, temp;
	T tr, ti, ur, ui, *p1r, *p1i, *p2r, *p2i;
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

void fftTest()
{
	short frameSize = 1024;
	float magnitude[frameSize];
	float gFFTworksp[2*frameSize];
	double window, imag, real;

	float buffer[frameSize];

	for (int i=0; i<frameSize; ++i)
	{
		buffer[i] = sin(((double)i/frameSize) * 64 * M_PI);
	}


	/* do windowing and re,im interleave */
	for (int k = 0; k < frameSize;k++) {
		window = -.5*cos(2.*M_PI*(double)k/(double)frameSize)+.5;
		gFFTworksp[2*k] = buffer[k] * window;
		gFFTworksp[2*k+1] = 0.;
	}

	simpleFFT(gFFTworksp, frameSize, -1);

	for (int k = 0; k <= (0.5*frameSize); k++) {
		real = gFFTworksp[2*k];
		imag = gFFTworksp[2*k+1];

		magnitude[k] = 2.*sqrt(real*real + imag*imag);
	}
}

void simpleFFT(double *fftBuffer, long fftFrameSize, long sign)
{
	tSimpleFFT<double>(fftBuffer, fftFrameSize, sign);
}

void simpleFFT(float *fftBuffer, long fftFrameSize, long sign)
{
	tSimpleFFT<float>(fftBuffer, fftFrameSize, sign);
}


