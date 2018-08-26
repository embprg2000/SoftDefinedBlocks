/*
 * sourceRTL.h
 *
 *  Created on: Sep 11, 2016
 *      Author: evert
 */

#ifndef SOURCERTL_HPP_
#define SOURCERTL_HPP_
#include <cstdint>
#include <vector>
#include <thread>
#include <climits>
#include <unistd.h>
#include <ctype.h>
#include <atomic>
#include <algorithm>

#include "../Main/DataBuffer.hpp"
#include "../Main/IBlock.hpp"
#include "../Main/Variables.hpp"
#include "../Main/ProcessScript.hpp"
#include "../SourceBlocks/RtlSdrsource.hpp"

class sourceRTL : public blkSource<IQSampleVector>
{
public:
	sourceRTL()
		: blkSource(0.0)
		, m_devidx(0)
		, m_agcmode(true)
		, m_lnagain(0)
		, m_inbufWarning(false)
		, m_sdr(0)
		, m_pFreq(0)
		, m_pSampl(0)
		, m_bufSize(0)
		, m_bStopThread(false)
	{
		ProcessScript::getInstance().attach(this, "DDI");
	}

	virtual ~sourceRTL()
	{
		if (m_sdr)
			delete m_sdr;
	}

	virtual std::string getBlkName() { return "sourceRTL"; }

	virtual double getSampleRate() { return *m_pSampl; }

	//double freq, double ifrate, bool bAGC=false)
	virtual Err init(vector<tVar>& args)
	{
		m_devidx = 0;
		m_agcmode = (bool)args[2];
		m_lnagain = 0;
		m_inbufWarning = false;
		m_sdr = 0;
		m_bStopThread = false;

		m_pFreq   = new mDbl(this, "Freq", args[0],  24e6, 1.6e9, "%d");
		m_pSampl  = new mDbl(this, "Fc",  args[1],   0.2e6, 4e6, "%5.2f");
		m_bufSize = new mInt(this, "#S", 0,  0, 10e9, "%d");

		//*m_lnagain = INT_MIN;
	    vector<string> devnames = RtlSdrSource::get_device_names();

	    m_out = new IQSampleVector( RtlSdrSource::default_block_length );

	    if (m_devidx < 0 || (unsigned int)m_devidx >= devnames.size())
	    {
	        if (m_devidx != -1)
	            fprintf(stderr, "ERROR: invalid device index %d\n", m_devidx);
	        fprintf(stderr, "Found %u devices:\n", (unsigned int)devnames.size());

	        for (unsigned int i = 0; i < devnames.size(); i++)
	            fprintf(stderr, "%2u: %s\n", i, devnames[i].c_str());

	        DBG_ERR(Err::eErrSrcError, "Error opening SDR device");
	        return Err(Err::eErrSrcError);
	    }
	    fprintf(stderr, "using device %d: %s\n", m_devidx, devnames[m_devidx].c_str());

	    // Open RTL-SDR device.
	    m_sdr = new RtlSdrSource(m_devidx);
	    if (m_sdr == 0)
	    {
	        DBG_ERR(Err::eErrSrcError, m_sdr->error().c_str());
	        return Err(Err::eErrSrcError);
	    }

	    if (m_sdr->error() != "")
		{
			DBG_ERR(Err::eErrSrcError, m_sdr->error().c_str());
			return Err(Err::eErrSrcError);
		}

	    // Check LNA gain.
	    m_gains   = m_sdr->get_tuner_gains();
	    m_lnagain = new mInt(this, "LNA gain", -1, -1, m_gains.size(), "%d");

	    // Force auto
	    *m_lnagain = m_gains[16];
/*
	    if (*m_lnagain != INT_MIN) {
	        if (find(m_gains.begin(), m_gains.end(), *m_lnagain) == m_gains.end())
	        {
	            if (*m_lnagain != INT_MIN + 1)
	                fprintf(stderr, "ERROR: LNA gain %.1f dB not supported by tuner\n", *m_lnagain * 0.1);
	            fprintf(stderr, "Supported LNA gains: ");
	            for (int g: m_gains)
	                fprintf(stderr, " %.1f dB ", 0.1 * g);
	            fprintf(stderr, "\n");
		        DBG_ERR(Err::eErrSrcError, "LNA gain not supported by tuner");
		        return Err(Err::eErrSrcError);
	        }
	    }
*/
	    m_sampleRate = *m_pSampl;
	    return update();
	}

	virtual Err update()
	{
	    // Configure RTL-SDR device and start streaming.
	    if (m_sdr->configure(*m_pSampl, *m_pFreq, *m_lnagain, RtlSdrSource::default_block_length, m_agcmode) == false) {
	        DBG_ERR(Err::eErrSrcError, std::string("Error: Configure SDR :") + m_sdr->error());
	        return Err(Err::eErrUpdate);
	    }

	    *m_pFreq = m_sdr->get_frequency();
	    fprintf(stderr, "device tuned for:  %.6f MHz\n", *m_pFreq * 1.0e-6);

	    if (*m_lnagain == INT_MIN)
	        fprintf(stderr, "LNA gain:          auto\n");
	    else
	        fprintf(stderr, "LNA gain:          %.1f dB\n", 0.1 * m_sdr->get_tuner_gain());

	    *m_pSampl = m_sdr->get_sample_rate();
	    fprintf(stderr, "IF sample rate:    %.0f Hz\n", (double)*m_pSampl);

	    fprintf(stderr, "RTL AGC mode:      %s\n", m_agcmode ? "enabled" : "disabled");

	    return Err(Err::eOK);
	}

	virtual Err start()
	{
		m_thread = std::thread( &sourceRTL::readData, this );

		//m_buf.wait_buffer_fill(32000);
		return Err(Err::eOK);
	}

	virtual Err process()
	{
		if (m_pFreq->isDirty()) // If frequency changed update.
		{
			m_sdr->set_frequency(*m_pFreq);
/*
			m_bStopThread.store(true);
			m_thread.join();

			update();

			m_bStopThread.store(false);
			m_thread = std::thread( &sourceRTL::readData, this );
*/
			return Err(Err::eOK);
		}
		else
		{
			if (m_buf.queued_samples() > 10 * *m_pSampl && m_inbufWarning == false) {
				DBG_LOG("\nWARNING: Input buffer is growing (system too slow)");
				m_inbufWarning = true;
			}

			while (m_buf.queued_samples() < 30e3)
			{
				usleep(100);
			}

			// Pull next block from source buffer.
			*m_out = m_buf.pull();
			*m_bufSize  = m_out->size();
			return Err(Err::eOK);
		}
	}

	virtual Err stop()
	{
		m_bStopThread.store(true);
		m_thread.join();

		DBG_LOG("Stopped sourceRTL thread.\n");
		return Err(Err::eOK);
	}

	virtual void getOutBuff(SampleVector*& vec)  { vec = 0; }
	virtual void getOutBuff(IQSampleVector*& vec) { vec = m_out; }

private:

	int 		m_devidx;
	bool     	m_agcmode = false;
	int         m_lnagain;
	bool		m_inbufWarning;
	RtlSdrSource* m_sdr;
	vector<int> m_gains;
	DataBuffer<IQSample> m_buf;
	std::thread  m_thread;
	mDbl*		m_pFreq;
	mDbl*		m_pSampl;
	mInt*		m_bufSize;
	atomic_bool m_bStopThread;

	/**
	 * Read data from source device and put it in a buffer.
	 *
	 * This code runs in a separate thread.
	 * The RTL-SDR library is not capable of buffering large amounts of data.
	 * Running this in a background thread ensures that the time between calls
	 * to RtlSdrSource::get_samples() is very short.
	 */
	void readData()
	{
	    IQSampleVector iqsamples;

	    while (m_bStopThread.load() == false)
	    {
	        if (!m_sdr->get_samples(iqsamples)) {
	            fprintf(stderr, "ERROR: RtlSdr: %s\n", m_sdr->error().c_str());
	            DBG_ERR(Err::eErrSrcError, "ERROR: Reading SDR samples.");
	            bTerminate.store(true);
	        }
	        m_buf.push(move(iqsamples));
	    }
	    m_buf.push_end();
	}
};

#endif /* SOURCERTL_HPP_ */
