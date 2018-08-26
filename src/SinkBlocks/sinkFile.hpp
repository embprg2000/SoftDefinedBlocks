/*
 * sinkFile.hpp
 *
 *  Created on: Sep 18, 2016
 *      Author: evert
 */

#ifndef SINKFILE_HPP_
#define SINKFILE_HPP_

#include <csignal>
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <unistd.h>
#include <ctype.h>
#include <cstring>
#include <fstream>
#include <sys/time.h>

#include <sys/stat.h>
#include <fcntl.h>

#include "../Main/IBlock.hpp"
#include "../ProcessBlocks/Squelch.hpp"


template<class T_IN>
class sinkFile : public IBlock
{
public:

	sinkFile(IBlock* blkIn, std::string(fName)) : m_blkIn(blkIn), m_in(0), m_fileName(fName), m_fd(-1), m_squelch(-20.0)
	{
		m_rms = new mDbl(this, "RMS", 0, -1e28, 1e28, "R:%2.3f");
		m_wrt = new mInt(this, "WRT", 0, 0, 1, "WRT:%d");
	}
	virtual ~sinkFile()
	{
		stop();
		delete m_rms;
	}

	virtual std::string getBlkName() { return "sinkFile"; }

	virtual double getSampleRate() { return 1.0; }

	virtual Err init(vector<tVar>& args)
	{
		m_blkIn->getOutBuff( m_in );

        m_fd = open(m_fileName.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (m_fd < 0)
        {
        	DBG_ERR(Err::eErrCantOpenFile, "Can't open file.");
        	return Err::eErrCantOpenFile;
        }
		return (m_in == 0) ? Err(Err::eErrWrongCast) : Err(Err::eOK);
	}

	virtual Err start()
	{
		return Err(Err::eOK);
	}

	virtual Err update()
	{
		return Err(Err::eOK);
	}

	virtual Err process()
	{
	    if (m_fd < 0)
	        return Err::eErrCantOpenFile;

	    if (m_squelch.enabled() == false)
	    {
	    	return writeSamples(*m_in);
	    }
	    else
	    {
	    	T_IN m_buf;
	    	typename T_IN::value_type out;

	    	for (auto it : (*m_in))
	    	{
	    		if (m_squelch.processSamples(it, out) == true)
	    			m_buf.push_back( out );
	    	}

	    	*m_rms = m_squelch.getRMS();
	    	*m_wrt = m_buf.size() > 0;

	    	if (*m_wrt > 0 )
	    		return writeSamples(m_buf);
	    	else
	    		return Err::eOK;
	    }
	}

	virtual Err stop()
	{
	    if (m_fd > 0)
	    {
	        close(m_fd);
	        m_fd = 0;
	    }
	    return Err::eOK;
	}

	virtual void getOutBuff(SampleVector*& vec)  { vec = 0; }
	virtual void getOutBuff(IQSampleVector*& vec) { vec = 0; }

private:
	IBlock*  m_blkIn;
	T_IN*    m_in;
	std::string m_fileName;
	int      m_fd;
	Squelch<typename T_IN::value_type>  m_squelch;
	mDbl*    m_rms;
	mInt*    m_wrt;

	Err writeSamples(T_IN vec)
	{
		size_t p = 0;
		size_t n = vec.size() * sizeof(typename T_IN::value_type);

		while (p < n)
		{
			ssize_t k = ::write(m_fd, vec.data() + p, n - p);
			if (k <= 0)
			{
				if (k == 0 || errno != EINTR)
				{
					stop(); // Close file
					DBG_ERR(Err::eErrCantWriteData, "Can't open file.");
					return Err::eErrCantWriteData;
				}
			}
			else
				p += k;
		}
		return Err::eOK;
	}
};


#endif /* SINKFILE_HPP_ */
