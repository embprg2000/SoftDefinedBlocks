/*
 * Misc.hpp
 *
 *  Created on: Sep 14, 2016
 *      Author: evert
 */

#ifndef MISC_HPP_
#define MISC_HPP_

#include "../Main/tVar.hpp"
#include "../Main/IBlock.hpp"
#include "../Main/Variables.hpp"
#include "../Main/ProcessScript.hpp"

string val2str(const char* fmt, double val);

std::vector<std::string> split(const std::string text, char sep);

/*
template<class T_IN>
class MeanRms : public blkMeas<T_IN>
{
public:
	MeanRms(IBlock* blkIn) : blkMeas(blkIn), m_main(0.0), m_rms(0.0)
	{
		mStr fName(this, "m-r", "0.0-0.0");
	}
	virtual ~MeanRms() {}

	virtual Err process()
	{
	    Sample vsum = 0;
	    Sample vsumsq = 0;

	    unsigned int n = m_in->size();
	    for (unsigned int i = 0; i < n; i++) {
	        Sample v = (*m_in)[i];
	        vsum   += v;
	        vsumsq += v * v;
	    }

	    m_mean = vsum / n;
	    m_rms  = sqrt(vsumsq / n);

	    return Err::eOK;
	}
private:
	double m_main;
	double m_rms;
};
*/

class RMS_IQ : public blkCommon<IQSampleVector,IQSampleVector>
{
public:
	RMS_IQ()
		: m_RMS(0)
		, m_bLog(0)
		, m_alpha(0)
		, m_mag(-1e28)
	{
		ProcessScript::getInstance().attach(this, "SSID");
	}

	virtual ~RMS_IQ()
	{
		//if (m_RMS != 0)
		//	delete m_RMS;
	}

	// (IBlock* blkIn, string name, string fmt, bool bLog, double alpha=0.0)
	virtual Err init(vector<tVar>& args)
	{
		setName(args[0]);
		if (m_RMS != 0)
			delete m_RMS;
		m_RMS = new mDbl(this, args[0],  0, 0, 1e32, args[1]);
		m_bLog = (bool) args[2];
		m_alpha = args[3];

		m_blkIn->getOutBuff( m_in );
		if (m_in != 0)
		{
			m_sampleRate = m_blkIn->getSampleRate();
			m_out = m_in;
		}
		else
			return Err::eErrWrongCast;
		return Err::eOK;
	}

	virtual std::string getBlkName() { return "RMS_IQ"; }

	virtual double getSampleRate() { return 1.0 * m_blkIn->getSampleRate(); }

	virtual Err process()
	{
		double mag = 0;
		unsigned int n = m_in->size();

		if (n == 0)
		{
			return Err::eOK; // Don't return an error to stop the program.
		}

		n = (n + 63) / 64;

		for (unsigned int i = 0; i < n; i++)
		{
			IQSampleVector::value_type& s = (*m_in)[i];
			double re = s.real(), im = s.imag();
			mag += re * re + im * im;
		}

		if (m_mag == -1e28)
			m_mag = mag;
		else
			m_mag = (1-m_alpha) * m_mag + m_alpha * mag;  // Simple lowpass filter
		if (m_bLog == true)
			*m_RMS = 20 * log10(sqrt(m_mag / n));
		else
			*m_RMS = sqrt(m_mag / n);
		return blkCommon::process();
	}

	virtual IBlock* copyBlock()
	{
		RMS_IQ* obj = new RMS_IQ;
		*obj = *this;
		return obj;
	}
private:
	mDbl*	m_RMS;
	bool	m_bLog;
	double	m_alpha;
	double	m_mag;
};

class RMS : public  blkCommon<SampleVector,SampleVector>
{
public:
	RMS()
		: m_RMS(0)
		, m_bLog(0)
		, m_alpha(0)
		, m_mag(-1e28)
	{
		ProcessScript::getInstance().attach(this, "SSID");
	}
	virtual ~RMS() { }

	// (string name, string fmt, bool bLog, double alpha)
	virtual Err init(vector<tVar>& args)
	{
		m_bLog  = args[2];
		m_alpha = args[3];
		m_RMS   = new mDbl(this, args[0], 0, 0, 1e32, args[1]);

		m_blkIn->getOutBuff( m_in );
		if (m_in != 0)
		{
			m_sampleRate = m_blkIn->getSampleRate();
			m_out = m_in;
		}
		else
			return Err::eErrWrongCast;
		return Err::eOK;
	}

	virtual std::string getBlkName() { return "RMS"; }

	virtual double getSampleRate() { return 1.0 * m_blkIn->getSampleRate(); }

	virtual Err process()
	{
		double mag = 0;
		unsigned int n = m_in->size();

		if (n < 10)
			return Err::eOK;

		n = (n + 63) / 64;

		for (unsigned int i = 0; i < n; i++)
		{
			double x = (*m_in)[i];

			mag += x * x;
		}

		if (m_mag == -1e28)
			m_mag = mag;
		else
			m_mag = (1-m_alpha) * m_mag + m_alpha * mag;  // Simple lowpass filter
		if (m_bLog == true)
			*m_RMS = 20 * log10(sqrt(m_mag / n));
		else
			*m_RMS = sqrt(m_mag / n);
		return blkCommon::process();
	}

	virtual IBlock* copyBlock()
	{
		RMS* obj = new RMS;
		*obj = *this;
		return obj;
	}
private:
	string  m_fmt;
	mDbl*   m_RMS;
	bool	m_bLog;
	double  m_alpha;
	double	m_mag;
};

#endif /* MISC_HPP_ */
