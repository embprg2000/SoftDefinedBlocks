/*
 * sinkAudio.hpp
 *
 *  Created on: Sep 10, 2016
 *      Author: evert
 */

#ifndef SINKAUDIO_HPP_
#define SINKAUDIO_HPP_

#include <thread>

#include "IBlock.hpp"
#include "DataBuffer.hpp"
#include "AudioOutput.hpp"

#include "../Main/ProcessScript.hpp"

class AudioOutput;

class sinkAudio : public IBlock
{
public:
	enum eAudioMode { MODE_RAW, MODE_WAV, MODE_ALSA};

	sinkAudio()
		: m_blkLeft(0)
		, m_blkRight(0)
		, m_left(0)
		, m_right(0)
		, m_mode(MODE_ALSA)
		, m_audio(0)
		, m_pcmrate(48000)
		, m_bStereo(false)
		, m_filename("")
	{
		ProcessScript::getInstance().attach(this, "");
	}

//	sinkAudio(IBlock* blkLeft, IBlock* blkRight, std::string fname="", sinkAudio::eAudioMode mode = sinkAudio::MODE_WAV)
//		: m_blkLeft(blkLeft)
//		, m_blkRight(blkRight)
//		, m_left(0)
//		, m_right(0)
//		, m_mode(mode)
//		, m_audio(0)
//		, m_pcmrate(48000)
//		, m_bStereo(true)
//		, m_filename(fname) {}

	virtual Err setInputBlk(IBlock* ptr) { m_blkLeft = ptr; return Err::eOK; }

	// (string fname="", sinkAudio::eAudioMode mode = sinkAudio::MODE_WAV)
	virtual Err init(vector<tVar>& args)
	{
		std::string  alsadev("default");

		m_blkLeft->getOutBuff( m_left );

		if (m_filename.length() == 0)
			m_mode = MODE_ALSA;

		m_pcmrate = m_blkLeft->getSampleRate();

		switch (m_mode)
		{
			case MODE_RAW:
				DBG_LOG( std::string("writing raw 16-bit audio samples to ")  +  m_filename);
				m_audio = new RawAudioOutput(m_filename);
				break;
			case MODE_WAV:
				DBG_LOG( std::string("writing audio samples to ") +  m_filename);
				m_audio = new WavAudioOutput(m_filename, m_pcmrate, m_bStereo);
				break;
			case MODE_ALSA:
				DBG_LOG( std::string("playing audio to ALSA device ") +  alsadev.c_str());
				m_audio = new AlsaAudioOutput(alsadev, m_pcmrate, m_bStereo);
				break;
		}

		if (!(*m_audio))
		{
			DBG_ERR( Err::eErrSinkAudio, std::string("AudioOutput") );
			return Err(Err::eErrSinkAudio);
		}
		else
			return Err(Err::eOK);
	}

	virtual std::string getBlkName() { return "sinkAudio"; }

	virtual double getSampleRate() { return m_blkLeft->getSampleRate(); }

	virtual Err update()
	{
		return Err(Err::eOK);
	}

	virtual Err start()
	{
		if (m_audio)
		{
			m_threadOut = std::thread( &sinkAudio::WriteOutput, this );
			return Err(Err::eOK);
		}
		else
			return Err(Err::eErrSinkAudio);
	}

	virtual Err process()
	{
		if (m_bStereo == false)
		{
			while (m_buffer.queued_samples() > (uint32_t)(m_pcmrate * 5) && bTerminate.load() == false)
			{
				cout << "+++++++++++++++++++++++++++++++++++++++++++" << m_buffer.queued_samples() << endl;
				bTerminate.store(true);
			}
			m_buffer.push( move(*m_left) );
		}
		else
		{
			if (m_left->size() == m_right->size())
			{
				const unsigned int n = m_left->size();
				m_stereo.resize(n * 2);

				for (unsigned int i=0; i<n; ++i)
				{
					m_stereo[i*2] = (*m_left)[i];
					m_stereo[i*2+1] = (*m_right)[i];
				}
				m_buffer.push( move(m_stereo) );
			}
		}
		return Err(Err::eOK);
	}

	virtual Err stop()
	{
		m_threadOut.join();

		if (m_audio != 0)
		{
			delete m_audio;
			m_audio = 0;
		}

		return Err(Err::eOK);
	}

	virtual void getOutBuff(SampleVector*& vec)  { vec = 0; }
	virtual void getOutBuff(IQSampleVector*& vec) { vec = 0; }

	virtual IBlock* copyBlock()
	{
		sinkAudio* obj = new sinkAudio;
		//*obj = *this;
		return obj;
	}


private:
	IBlock* 	  	m_blkLeft;    // Used for mono
	IBlock*			m_blkRight;
	SampleVector*	m_left;
	SampleVector*	m_right;
	SampleVector    m_stereo;
	eAudioMode		m_mode;
	AudioOutput*	m_audio;
	int				m_pcmrate;
	bool			m_bStereo;
	DataBuffer<SampleType> m_buffer;
	std::thread		m_threadOut;
	std::string		m_filename;

	void WriteOutput()
	{
		while (bTerminate.load() == false)
	    {
	        if (m_buffer.queued_samples() == 0) {
	            // The buffer is empty. Perhaps the output stream is consuming
	            // samples faster than we can produce them. Wait until the buffer
	            // is back at its nominal level to make sure this does not happen
	            // too often.
	        	m_buffer.wait_buffer_fill( m_pcmrate * 2 * ((int)m_bStereo + 1) );
	        }
	        if (m_buffer.pull_end_reached()) {
	            // Reached end of stream.
	            break;
	        }
	        // Get samples from buffer and write to output.
	        SampleVector samples = m_buffer.pull();
	        m_audio->write(samples);

	        if (!(*m_audio))
	            DBG_ERR( Err::eErrSinkAudio, std::string("ERROR: AudioOutput: %s\n") + m_audio->error().c_str() );
	    }
	}
};

#endif /* SINKAUDIO_HPP_ */
