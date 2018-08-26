/*
 * sourceFileWav.hpp
 *
 *  Created on: Oct 7, 2016
 *      Author: evert
 */

#ifndef SOURCEFILEWAV_HPP_
#define SOURCEFILEWAV_HPP_

/*
 * sourceFile.hpp
 *
 *  Created on: Sep 21, 2016
 *      Author: evert
 */

// http://www.dreamincode.net/forums/topic/334091-reading-wave-file-cc/
// http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/
// http://www-mmsp.ece.mcgill.ca/documents/audioformats/wave/wave.html

#include "../Main/IBlock.hpp"

// WAVE PCM soundfile format - https://ccrma.stanford.edu/courses/422/projects/WaveFormat
typedef struct
{
	char		chunk_id[4];
	int			chunk_size;
	char		format[4];
	char		subchunk1_id[4];
	int			subchunk1_size;
	short int	audio_format;
	short int	num_channels;
	int			sample_rate;		// sample_rate denotes the sampling rate.
	int			byte_rate;
	short int	block_align;
	short int	bits_per_sample;
	char		subchunk2_id[4];
	unsigned int data_size;			// NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read
	//char         dummy;
} wav_header;
/*
int main()
{
	int nb;
	header meta;
	int count = 0;
	int BUFSIZE = 256;
	short int buff16[BUFSIZE * 2];

	FILE * infile = fopen("src/160105_40WPM.wav","rb");

	if (infile)
	{
		fread(&meta, 1, sizeof(header), infile);

		cout << "first chunk is :" << sizeof(meta.chunk_id) << " bytes in size" << endl;
		cout << "The file is a :" << meta.chunk_id << " format" << endl;
		cout << " Size of Header file is "<<sizeof(meta)<<" bytes" << endl;
		cout << " Sampling rate of the input wave file is "<< meta.sample_rate <<" Hz" << endl;
		cout << " Size of data in the audio is: " << sizeof(meta.subchunk2_size)<< " bytes" << endl;
		cout << " The number of channels of the file is "<< meta.num_channels << " channels" << endl;
		cout << " The audio format is PCM:"<< meta.audio_format << endl;

		while ((nb = fread(buff16, 2, BUFSIZE, infile)) > 0)
		{
			for (int i = 0; i<nb; i += 2)
			{
				double val = ((buff16[i]<<8) | buff16[i+1]) / 32768.0;
			}
			count++;
		}

		cout << " Number of frames in the input wave file are " <<count << endl;
		return 0;
	}
}
*/

class sourceFileWav : public blkSource<SampleVector>
{
public:
	sourceFileWav(string nFile) : blkSource(0.0), m_nFile(nFile), m_fp(0), m_frameSize(65536) {}
	virtual ~sourceFileWav()
	{
		stop();
	}

	virtual std::string getBlkName() { return "sourceFileWav"; }

	Err virtual init()
	{
		blkSource::init();

		if (m_out == 0)
			return Err::eErrWrongCast;

		m_fp = fopen(m_nFile.c_str(), "rb");
		if (m_fp == 0)
			return Err::eErrCantOpenFile;

		memset(&m_wav_header, 0, sizeof(wav_header));
		if (m_nFile.find(".wav") > 0)
		{
			fread(&m_wav_header, 1, sizeof(wav_header), m_fp);
			m_sampleRate = m_wav_header.sample_rate / 2.0;
			m_frameSize   = m_wav_header.sample_rate;

			if ( strncmp(m_wav_header.chunk_id, "RIFF", 4) != 0)
				return Err::eErrWrongFileFormat;

			if ( strncmp(m_wav_header.format, "WAVE", 4) != 0)
				return Err::eErrWrongFileFormat;

			if ( strncmp(m_wav_header.subchunk1_id, "fmt ", 4) != 0)
				return Err::eErrWrongFileFormat;

			if ( strncmp(m_wav_header.subchunk2_id, "data", 4) != 0)
				return Err::eErrWrongFileFormat;

			if (m_wav_header.bits_per_sample != 16 && m_wav_header.bits_per_sample != 8)
				return Err::eErrWrongFileFormat;
		}
		else
			return Err::eErrWrongFileFormat;

		m_out->resize(m_frameSize);
		return blkSource::init();
	}

	Err virtual process()
	{
		if (m_fp != 0)
		{
			// Wave file
			int nb;
			int cnt = 0;
			int BUFSIZE = 256;

			if (m_wav_header.bits_per_sample == 16)
			{
				uint8_t buff16[BUFSIZE * 2];

				m_out->clear();
				while ((nb = fread(buff16, 1, sizeof(buff16), m_fp)) > 0 && cnt < m_frameSize )
				{
					for (int i = 0; i<nb; i += 2)
					{
						int16_t tmp = (buff16[i+1]<<8) | buff16[i];
						double  val = (tmp - 32767.0) / 32767.0;

						m_out->push_back(val);
					}
					cnt += nb;
				}
			}
			if (m_wav_header.bits_per_sample == 8)
			{
				uint8_t buff16[BUFSIZE];

				m_out->clear();
				while ((nb = fread(buff16, 1, BUFSIZE, m_fp)) > 0 && cnt < m_frameSize )
				{
					for (int i = 0; i<nb; i += 2)
					{
						double  val = (buff16[i] - 128) / 128.0;

						m_out->push_back(val);
					}
					cnt += nb;
				}
			}

			if (nb < BUFSIZE)
				bTerminate.store(true);

			return blkSource::process();
		}
		else
		{
			bTerminate.store(true);
			return Err::eErrCantOpenFile;
		}
	}

	Err virtual stop()
	{
		if (m_fp != 0)
		{
			fclose(m_fp);
			m_fp = 0;
		}
		bTerminate.store(true);
		return Err::eOK;
	}

private:
	string	m_nFile;
	FILE*	m_fp;
	int		m_frameSize;
	wav_header m_wav_header;
};

#endif /* SOURCEFILEWAV_HPP_ */
