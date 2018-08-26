// A simple FFT routine for testing.
#ifndef __SIMPLEFFT__
#define __SIMPLEFFT__

// https://github.com/samyk/inspectrum/blob/master/fft.h -- For better performance
// https://github.com/keenerd/rtl-sdr-misc/blob/master/rtl-sdl/rtl_power_lite.c

void simpleFFT(double *fftBuffer, long fftFrameSize, long sign);
void simpleFFT(float *fftBuffer, long fftFrameSize, long sign);

#endif // __SIMPLEFFT__
