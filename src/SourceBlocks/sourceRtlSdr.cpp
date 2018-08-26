/*
 * sourceRtlSdr.cpp
 *
 *  Created on: Feb 8, 2017
 *      Author: evert
 */

#include <climits>
#include <string.h>
#include <rtl-sdr.h>


#include "sourceRtlSdr.hpp"
#include "../Main/ProcessScript.hpp"
#include "../Widgets/WidgetContainer.hpp"

static sourceRtlSdr obj;

sourceRtlSdr::sourceRtlSdr()
	: blkSource(0.0)
	, m_devidx(0)
	, m_agcmode(true)
	, m_lnagain(0)
	, m_inbufWarning(false)
	, m_pSampl(0)
	, m_freqIdx(0)
	, m_pFreqSlider(0)
	, m_freq(0)
	, m_bStopThread(false)
	, m_block_length(5 * 1024)
	, m_dev(0)
{
	ProcessScript::getInstance().attach(this, "DDI");
}

sourceRtlSdr::~sourceRtlSdr()
{
    if (m_dev)
        rtlsdr_close(m_dev);
}

double sourceRtlSdr::getSampleRate()
{
	return m_sampleRate;
}

Err sourceRtlSdr::openRtlSdr(int dev_index)
{
	int r;

	const char *devname = rtlsdr_get_device_name(dev_index);
	if (devname != NULL)
		m_devname = devname;

	r = rtlsdr_open(&m_dev, dev_index);
	if (r < 0) {
		ostringstream error;
		error <<  "Failed to open RTL-SDR device (";
		error <<  strerror(-r) << ")";
		DBG_ERR(Err::eErrCantOpenDevice, error.str() );
		return Err::eErrCantOpenDevice;
	}
	return Err::eOK;
}

// Configure RTL-SDR tuner and prepare for streaming.
bool sourceRtlSdr::configure(uint32_t sample_rate,
                             uint32_t frequency,
                             int tuner_gain,
                             int block_length,
                             bool agcmode)
{
    int r;

    if (!m_dev)
        return false;

    r = rtlsdr_set_sample_rate(m_dev, sample_rate);
    if (r < 0) {
    	DBG_ERR(Err::eDriverError, "rtlsdr_set_sample_rate failed");
        return false;
    }

    r = rtlsdr_set_center_freq(m_dev, frequency);
    if (r < 0) {
    	DBG_ERR(Err::eDriverError, "rtlsdr_set_center_freq failed");
        return false;
    }

    if (tuner_gain == INT_MIN) {
        r = rtlsdr_set_tuner_gain_mode(m_dev, 0);
        if (r < 0) {
        	DBG_ERR(Err::eDriverError, "rtlsdr_set_tuner_gain_mode could not set automatic gain");
            return false;
        }
    } else {
        r = rtlsdr_set_tuner_gain_mode(m_dev, 1);
        if (r < 0) {
        	DBG_ERR(Err::eDriverError, "rtlsdr_set_tuner_gain_mode could not set manual gain");
            return false;
        }

        r = rtlsdr_set_tuner_gain(m_dev, tuner_gain);
        if (r < 0) {
        	DBG_ERR(Err::eDriverError, "rtlsdr_set_tuner_gain failed");
            return false;
        }
    }

    // set RTL AGC mode
    r = rtlsdr_set_agc_mode(m_dev, int(agcmode));
    if (r < 0) {
    	DBG_ERR(Err::eDriverError, "rtlsdr_set_agc_mode failed");
        return false;
    }

    // set block length
    m_block_length = (block_length < 4096) ? 4096 :
                     (block_length > 1024 * 1024) ? 1024 * 1024 :
                     block_length;
    m_block_length -= m_block_length % 4096;

    // reset buffer to start streaming
    if (rtlsdr_reset_buffer(m_dev) < 0) {
    	DBG_ERR(Err::eDriverError, "rtlsdr_reset_buffer failed");
        return false;
    }

    return true;
}

bool sourceRtlSdr::updateGain(int gain)
{
	int r;

	if (gain == INT_MIN) {
		r = rtlsdr_set_tuner_gain_mode(m_dev, 0);
		if (r < 0) {
			DBG_ERR(Err::eDriverError, "rtlsdr_set_tuner_gain_mode could not set automatic gain");
			return false;
		}
	} else {
		r = rtlsdr_set_tuner_gain_mode(m_dev, 1);
		if (r < 0) {
			DBG_ERR(Err::eDriverError, "rtlsdr_set_tuner_gain_mode could not set manual gain");
			return false;
		}

		r = rtlsdr_set_tuner_gain(m_dev, gain);
		if (r < 0) {
			DBG_ERR(Err::eDriverError, "rtlsdr_set_tuner_gain failed");
			return false;
		}
	}
}


//double freq, double ifrate, bool bAGC=false)
Err sourceRtlSdr::init(vector<tVar>& args)
{
	Err err = Err::eOK;
	m_devidx = 0;
	m_agcmode = (bool)args[2];
	m_lnagain = 0;
	m_inbufWarning = false;
	m_bStopThread = false;

	m_pFreqSlider   = new WidgetSlider("Sliders");
	m_freqIdx = m_pFreqSlider->addSlider( {"Frequency", "MHz", 87.1, 109.1, 0.2, 107.5} );
	WidgetContainer::getInst().add( m_pFreqSlider );

	m_varList = new WidgetVarList("idVarsSourceRTL", "clssVars", {"Freq", "LNA Gain", "AGC","Fs"} );
	WidgetContainer::getInst().add( m_varList );

	m_buttAGC = new WidgetButton("ButtAGC", "AGC", true);
	WidgetContainer::getInst().add( m_buttAGC );

	m_text = new WidgetText("RTL_Info", "");
	WidgetContainer::getInst().add( m_text );

	m_sampleRate  = args[1];

	vector<string> devnames;

    int device_count = rtlsdr_get_device_count();
    if (device_count > 0)
    {
		devnames.reserve(device_count);
		for (int i = 0; i < device_count; i++) {
			devnames.push_back( string(rtlsdr_get_device_name(i)) );
		}
    }

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

	ostringstream text;

	text << "Device: " << m_devidx << " - " << devnames[m_devidx].c_str() << "<br>";

	enum rtlsdr_tuner {
		RTLSDR_TUNER_UNKNOWN = 0,
		RTLSDR_TUNER_E4000,
		RTLSDR_TUNER_FC0012,
		RTLSDR_TUNER_FC0013,
		RTLSDR_TUNER_FC2580,
		RTLSDR_TUNER_R820T,
		RTLSDR_TUNER_R828D
	};

	switch (rtlsdr_get_tuner_type(m_dev) )
	{
	case RTLSDR_TUNER_UNKNOWN: text << "RTLSDR_TUNER_UNKNOWN"; break;
	case RTLSDR_TUNER_E4000:   text << "RTLSDR_TUNER_E4000;";  break;
	case RTLSDR_TUNER_FC0012:  text << "RTLSDR_TUNER_FC0012";  break;
	case RTLSDR_TUNER_FC0013:  text << "RTLSDR_TUNER_FC0013";  break;
	case RTLSDR_TUNER_FC2580:  text << "RTLSDR_TUNER_FC2580";  break;
	case RTLSDR_TUNER_R820T:   text << "RTLSDR_TUNER_R820T";   break;
	case RTLSDR_TUNER_R828D:   text << "RTLSDR_TUNER_R828D";   break;
	}
	m_text->setText( text.str() );

	fprintf(stderr, "using device %d: %s\n", m_devidx, devnames[m_devidx].c_str());

	// Open RTL-SDR device.
	err = openRtlSdr(m_devidx);
	if (err)
		return err;

	// Check LNA gain.
    int num_gains = rtlsdr_get_tuner_gains(m_dev, NULL);
    if (num_gains > 0)
    {

		m_gains.resize(num_gains);
		if (rtlsdr_get_tuner_gains(m_dev, m_gains.data()) == num_gains)
		{
			// m_gains   = m_sdr->get_tuner_gains();
			// m_lnagain = new mInt(this, "LNA gain", -1, -1, m_gains.size(), "%d");
			m_gainIdx = m_pFreqSlider->addSlider( {"Gain", "", 0, num_gains, 1, 16} );
		}
		else
		{
			m_gains.clear();
			return Err::eDriverError;
		}
    }

	// Force auto

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

	m_out     = new IQSampleVector( m_block_length );
	m_freq    = m_pFreqSlider->getValue(m_freqIdx) * 1e6;
	m_lnagain = m_gains[m_pFreqSlider->getValue(m_gainIdx)];
	m_agcmode = m_buttAGC->getSelected();
	return update();
}

Err sourceRtlSdr::update()
{
	// Configure RTL-SDR device and start streaming.
	if (configure(m_sampleRate, m_freq, m_lnagain, m_block_length, m_agcmode) == false) {
		DBG_ERR(Err::eErrSrcError, "Error: Configure SDR :");
		return Err(Err::eErrUpdate);
	}

	m_freq = rtlsdr_get_center_freq(m_dev);
	fprintf(stderr, "Device tuned for:  %.6f MHz\n", m_freq * 1.0e-6);

	if (m_lnagain == INT_MIN)
		fprintf(stderr, "LNA gain:          auto\n");
	else
		fprintf(stderr, "LNA gain:          %.1f dB\n", 0.1 * rtlsdr_get_tuner_gain(m_dev));

	m_sampleRate = rtlsdr_get_sample_rate(m_dev);
	fprintf(stderr, "IF sample rate:    %.0f Hz\n", (double)m_sampleRate);
	fprintf(stderr, "m_agcmode mode:      %s\n", m_agcmode ? "enabled" : "disabled");

	return Err(Err::eOK);
}

Err sourceRtlSdr::start()
{
	m_thread = std::thread( &sourceRtlSdr::readData, this );

	//m_buf.wait_buffer_fill(32000);
	return Err(Err::eOK);
}

Err sourceRtlSdr::process()
{
	if (m_pFreqSlider->isDirty()) // If frequency changed update.
	{
		m_freq = m_pFreqSlider->getValue(m_freqIdx) * 1e6;
		rtlsdr_set_center_freq(m_dev, m_freq);

		m_lnagain = m_gains[m_pFreqSlider->getValue(m_gainIdx)];

		if (m_lnagain == 0)
			m_lnagain = INT_MIN;

		updateGain(m_lnagain);

		m_varList->setValues( {(double)m_freq/1e6, (double) m_lnagain/10.0, (double)m_agcmode, (double)m_sampleRate} );
	}
	else if (m_buttAGC->isDirty())
	{
		m_agcmode = m_buttAGC->getSelected();
		rtlsdr_set_agc_mode(m_dev, int(m_agcmode));

//		m_bStopThread.store(true);
//		m_thread.join();
//
//		update();
//
//		m_bStopThread.store(false);
//		m_thread = std::thread( &sourceRtlSdr::readData, this );
	}
	else
	{
		// "Freq", "LNA Gain", "AGC","Fs"
		m_varList->setValues( {(double)m_freq/1e6, (double) m_lnagain/10.0, (double)m_agcmode, (double)m_sampleRate} );
		if (m_buf.queued_samples() > 10 * m_sampleRate && m_inbufWarning == false) {
			DBG_LOG("\nWARNING: Input buffer is growing (system too slow)");
			m_inbufWarning = true;
		}

		while (m_buf.queued_samples() < 30e3)
		{
			// usleep(100);
			WidgetContainer::getInst().msecSleep(1);
		}

		// Pull next block from source buffer.
		*m_out = m_buf.pull();
	}
	return Err(Err::eOK);
}

Err sourceRtlSdr::stop()
{
	m_bStopThread.store(true);
	m_thread.join();

	DBG_LOG("Stopped sourceRTL thread.\n");
	return Err(Err::eOK);
}

// Fetch a bunch of samples from the device.
bool sourceRtlSdr::get_samples(IQSampleVector& samples)
{
    int r, n_read;

    if (!m_dev)
        return false;

    vector<uint8_t> buf(2 * m_block_length);

    r = rtlsdr_read_sync(m_dev, buf.data(), 2 * m_block_length, &n_read);
    if (r < 0) {
    	DBG_ERR(Err::eDriverError, "rtlsdr_read_sync failed");
        return false;
    }

    if (n_read != 2 * m_block_length) {
    	DBG_ERR(Err::eDriverError, "short read, samples lost");
        return false;
    }

    samples.resize(m_block_length);
    for (int i = 0; i < m_block_length; i++) {
        int32_t re = buf[2*i];
        int32_t im = buf[2*i+1];
        samples[i] = IQSample( (re - 128) / IQSample::value_type(128), (im - 128) / IQSample::value_type(128) );
    }
    return true;
}

/**
 * Read data from source device and put it in a buffer.
 *
 * This code runs in a separate thread.
 * The RTL-SDR library is not capable of buffering large amounts of data.
 * Running this in a background thread ensures that the time between calls
 * to RtlSdrSource::get_samples() is very short.
 */
void sourceRtlSdr::readData()
{
	IQSampleVector iqsamples;

	while (m_bStopThread.load() == false)
	{
		if (get_samples(iqsamples) == false)
		{
			DBG_ERR(Err::eErrSrcError, "ERROR: Reading SDR samples.");
			bTerminate.store(true);
		}
		m_buf.push(move(iqsamples));
	}
	m_buf.push_end();
}


