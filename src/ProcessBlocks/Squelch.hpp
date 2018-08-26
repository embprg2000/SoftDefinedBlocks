/*
 * Squelch.hpp
 *
 *  Created on: Sep 23, 2016
 *      Author: evert
 */

#ifndef SQUELCH_HPP_
#define SQUELCH_HPP_

#include "../Main/IBlock.hpp"

template <class T>
class Squelch
{
public:

	Squelch(double threshold, double alpha=0.98) : m_threshold(0), m_mag(0.0), m_magMax(0.0), m_alpha(alpha), m_beta(1-alpha), m_cnt(0), m_idx(0), m_nAVG(64)
	{
		m_vec.resize(0x03FF);
		m_threshold = calMag(threshold);
	}

	double getRMS()
	{
		double ret =  20*log10(sqrt(m_magMax / m_nAVG));;

		m_magMax = 0.0;
		return ret;
	}

	bool enabled()  { return true; }  // At least three samples over threshold
	void setThreshold(double val) { m_threshold = calMag(val); }

	bool processSamples(const T& in, T& out)
	{
		double	sum  = 0.0;

		out = m_vec[m_idx];

		m_idx++;
		m_idx = m_idx & 0x03FF;
		m_vec[m_idx] = in;

		for (short i=0; i < m_nAVG; ++i)
		{
			complex<float> v = m_vec[(m_idx + i) & 0x03FF];
			float re = v.real(), im = v.imag();
			sum += re * re + im * im;
		}

		m_mag = m_mag * m_alpha + m_beta * sum;
		if (m_mag > m_magMax)
			m_magMax = m_mag;

		// Hysteresis
		if (m_mag > m_threshold)
		{
			if (m_cnt < 100)
				m_cnt += 10;
		}
		else
		{
			if (m_cnt > 0)
				m_cnt--;
		}
		return m_cnt > 30;
	}

private:
	double 		m_threshold;
	double      m_mag;
	double      m_magMax;
	double      m_alpha;
	double      m_beta;    // (1 - m_alpha)
	short       m_cnt;
	short		m_idx;
	const short m_nAVG;
	vector<T>	m_vec;

	double calMag(double rms)
	{
		double m = pow(10,(rms/20.0));
		return m * m * (0x1FF / 4);
	}
};

#endif /* SQUELCH_HPP_ */
