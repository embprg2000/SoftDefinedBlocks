/*
 * sourceRtlSdr.hpp
 *
 *  Created on: Feb 8, 2017
 *      Author: evert
 */

#ifndef SOURCERTLSDR_HPP_
#define SOURCERTLSDR_HPP_

#include <IBlock.hpp>

#include "../Main/DataBuffer.hpp"
#include "../Main/IBlock.hpp"

#include "../Widgets/WidgetSlider.hpp"
#include "../Widgets/WidgetText.hpp"
#include "../Widgets/WidgetSelect.hpp"
#include "../Widgets/WidgetVarList.hpp"
#include "../Widgets/WidgetButton.hpp"

class sourceRtlSdr : public  blkSource<IQSampleVector> {
public:
	sourceRtlSdr();

	~sourceRtlSdr() override;

	std::string getBlkName()  { return "sourceRTL"; }

	double getSampleRate() override;

	//double freq, double ifrate, bool bAGC=false)
	Err init(vector<tVar>& args) override;
	Err update() override;

	Err start() override;
	Err process() override;
	Err stop() override;
	void getOutBuff(SampleVector*& vec)  { vec = 0; }
	void getOutBuff(IQSampleVector*& vec) { vec = m_out; }

	virtual IBlock* copyBlock()
	{
		sourceRtlSdr* obj = new sourceRtlSdr;
		//*obj = *this;
		return obj;
	}

private:

	int 			m_devidx;
	bool     		m_agcmode = false;
	bool			m_inbufWarning;
	string			m_devname;
	struct rtlsdr_dev * m_dev;
	int             m_block_length;
	vector<int> 	m_gains;
	int				m_gainIdx;
	int				m_lnagain;
	DataBuffer<IQSample> m_buf;
	std::thread  	m_thread;
	WidgetSlider* 	m_pFreqSlider;
	int				m_freqIdx;
	int				m_freq;
	WidgetSelect*	m_pSampl;
	WidgetVarList*  m_varList;
	WidgetButton*   m_buttAGC;
	WidgetText*     m_text;
	atomic_bool     m_bStopThread;

	Err openRtlSdr(int dev_index);

	bool updateGain(int gain);

	bool configure(uint32_t sample_rate, uint32_t frequency,
				int tuner_gain, int block_length, bool agcmode);

	bool get_samples(IQSampleVector& samples);

	/**
	 * Read data from source device and put it in a buffer.
	 *
	 * This code runs in a separate thread.
	 * The RTL-SDR library is not capable of buffering large amounts of data.
	 * Running this in a background thread ensures that the time between calls
	 * to RtlSdrSource::get_samples() is very short.
	 */
	void readData();

};

#endif /* SOURCERTLSDR_HPP_ */
