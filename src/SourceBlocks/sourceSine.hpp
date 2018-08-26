/*
 * sourceSine.hpp
 *
 *  Created on: Oct 7, 2016
 *      Author: evert
 */

#ifndef SOURCESINE_HPP_
#define SOURCESINE_HPP_

#include "../Main/IBlock.hpp"

class sourceSine : public blkSource<SampleVector>
{
public:
	sourceSine(double freq, double sampleFreq) : blkSource(0.0), m_freq(freq), m_sampleFreq(sampleFreq), m_tableSize(0), m_frameSize(65536), m_idx(0) {}
	virtual ~sourceSine()
	{
		stop();
	}

	virtual std::string getBlkName() { return "sourceSine"; }

	Err virtual init()
	{
		blkSource::init();
		m_sampleRate = m_sampleFreq;

		if (m_out == 0)
			return Err::eErrWrongCast;

		m_out->resize(m_frameSize);

		double samplesPerCycle = m_sampleFreq / m_freq;

		if (samplesPerCycle < 3 || samplesPerCycle > 2000)
			return Err::eErrWrongFrequency;

		double fac  = samplesPerCycle - int(samplesPerCycle);

		if (fac != 0)
			m_tableSize = samplesPerCycle * int(fac * 10.0);
		else
			m_tableSize = samplesPerCycle;

		m_table.resize(m_tableSize);

	    double phase_step = 2.0 * M_PI / samplesPerCycle;

	    for (int i = 0; i < m_tableSize; ++i)
	        m_table[i] = sin(phase_step * (double)i);

		return blkSource::init();
	}

	Err virtual process()
	{
		m_out->clear();
		for (int i=0; i<m_frameSize; ++i)
		{
			double tmp = m_table[m_idx++ % m_tableSize];
			m_out->push_back( tmp );
		}
		m_idx = m_idx % m_tableSize;
		return blkSource::process();
	}

	Err virtual stop()
	{
		return Err::eOK;
	}

private:
	double	m_freq;
	double	m_sampleFreq;
	int		m_tableSize;
	int 	m_frameSize;
	int		m_idx;
	SampleVector	m_table;
};
#endif /* SOURCESINE_HPP_ */
