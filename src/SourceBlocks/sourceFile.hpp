/*
 * sourceFile.hpp
 *
 *  Created on: Sep 21, 2016
 *      Author: evert
 */

// http://www.dreamincode.net/forums/topic/334091-reading-wave-file-cc/

#ifndef sourceFile_HPP_
#define sourceFile_HPP_

#include "../Main/IBlock.hpp"

class sourceFile : public blkSource<IQSampleVector>
{
public:
	//sourceFile(string nFile, int sampleRate) : blkSource(sampleRate, 65536), m_nFile(nFile), m_fp(0) {}
	sourceFile(string nFile, int sampleRate) : blkSource(sampleRate, 4096), m_nFile(nFile), m_fp(0) {}
	virtual ~sourceFile()
	{
		stop();
	}

	virtual std::string getBlkName() { return "sourceFile"; }

	Err virtual init(vector<tVar>& args)
	{
		blkSource::init(args);

		if (m_out == 0)
			return Err::eErrWrongCast;

		m_fp = fopen(m_nFile.c_str(), "rb");
		if (m_fp == 0)
			return Err::eErrCantOpenFile;

		return Err::eOK;
	}

	Err virtual process()
	{
		if (m_fp != 0)
		{
			// Binary format
			int i = 0;
			int n = m_out->size();
			float buffer[2];

			m_out->clear();
			while (fread(&buffer[0], sizeof(buffer), 1, m_fp) && i<n)
			{
				IQSample tmp = complex<float>((buffer[0] - 128) / 128.0, (buffer[1] - 128) / 128.0);

				m_out->push_back(tmp);
				i++;
			}

			if (i<n)
				bTerminate.store(true);

			//return blkSource::process();
			return Err::eOK;
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
};

#endif /* sourceFile_HPP_ */
