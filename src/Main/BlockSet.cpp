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
#include "../Main/SeqControl.hpp"
#include "../SinkBlocks/sinkAudio.hpp"

atomic_bool bTerminate(false); // Set when user presses ctrl-c


template<class T_OUT>
class srcSin : public IBlock
{
public:
	srcSin() : m_freq(0), m_ampl(0), m_idx(0), m_out(0)  {}
	virtual ~srcSin() {}

	virtual Err init(vector<tVar>& args)
	{
		m_idx  = 0;
		m_freq = 20;
		m_out  = new T_OUT(20);
		return Err();
	}

	virtual void process()
	{
		for (int i=0; i<m_out->size(); ++i)
			(*m_out)[i] = m_idx++;
	}

	virtual void stop()
	{
		if (m_out != 0)
			delete m_out;
	}

	virtual void getOutBuff(SampleVector*& vec)   { vec = (typeid(T_OUT) == typeid(SampleVector))  ? (SampleVector*)m_out : (SampleVector*)0; }
	virtual void getOutBuff(IQSampleVector*& vec)  { vec = (typeid(T_OUT) == typeid(IQSampleVector)) ? (IQSampleVector*)m_out : (IQSampleVector*)0; }

private:
	double  m_freq;
	double	m_ampl;
	int     m_idx;
	T_OUT*  m_out;
};

template<class T_OUT>
class srcReadFile : public IBlock
{
public:
	srcReadFile(string name) : m_name(name), m_bufSize(0), m_idx(0), m_out(0)  {}
	virtual ~srcReadFile() {}

	virtual Err init(vector<tVar>& args)
	{
		m_ifs.open(m_name.c_str(), ifstream::in | ifstream::binary);

		if ( m_ifs.is_open() )
		{
			m_idx  = 0;
			m_bufSize = 10e3;
			m_out  = new T_OUT(m_bufSize);
			return Err();
		}
		else
		{
			return Err( Err::eErrCantOpenFile );
		}
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
		float re;
		float im;

		for (unsigned int m_idx=0; m_idx<m_out->size() && m_ifs.good(); ++m_idx)
		{
			m_ifs.read((char*) &re, 4);
			m_ifs.read((char*) &im, 4);
			(*m_out)[m_idx++] = IQSample(re,im);
		}
		return Err::eOK;
	}

	virtual Err stop()
	{
		if (m_out != 0)
			delete m_out;
		return Err::eOK;
	}

	virtual void getOutBuff(SampleVector*& vec)   { vec = (typeid(T_OUT) == typeid(SampleVector))  ? (SampleVector*)m_out : (SampleVector*)0; }
	virtual void getOutBuff(IQSampleVector*& vec)  { vec = (typeid(T_OUT) == typeid(IQSampleVector)) ? (IQSampleVector*)m_out : (IQSampleVector*)0; }

private:
	string	  m_name;
	ifstream  m_ifs;
	int       m_bufSize;
	int       m_idx;
	T_OUT*    m_out;
};


template<class T_IN, class T_OUT>
class blkADD : public IBlock
{
public:

	blkADD(IBlock* blkIn, int add) : m_blkIn(blkIn), m_in(0), m_out(0), m_add(add) {}
	virtual ~blkADD() {}

	virtual Err init(vector<tVar>& args)
	{
		m_blkIn->getOutBuff( m_in );
		if (m_in != 0)
		{
			m_out = new T_OUT( m_in->size() );
			return Err(Err::eOK);
		}
		else
			return Err(Err::eErrWrongCast);
	}

	virtual Err update()
	{
		return Err(Err::eOK);
	}

	virtual Err start()
	{
		return Err(Err::eOK);
	}

	virtual Err process()
	{
		for (unsigned int i=0; i<m_out->size(); ++i)
			(*m_out)[i] = (*m_in)[i] + m_add;
		return Err::eOK;
	}

	virtual Err stop()
	{
		if (m_out != 0)
			delete m_out;
		return Err::eOK;
	}

	virtual void getOutBuff(SampleVector*& vec)   { vec = (typeid(T_OUT) == typeid(SampleVector))  ? (SampleVector*)m_out : (SampleVector*)0; }
	virtual void getOutBuff(IQSampleVector*& vec)  { vec = (typeid(T_OUT) == typeid(IQSampleVector)) ? (IQSampleVector*)m_out : (IQSampleVector*)0; }

private:
	IBlock* m_blkIn;
	T_IN*   m_in;
	T_OUT*  m_out;
	float   m_add;
};

template<class T_IN, class T_OUT>
class blkMULT : public IBlock
{
public:

	blkMULT(IBlock* blkIn, int mult) : m_blkIn(blkIn), m_in(0), m_out(0), m_mult(mult) {}
	virtual ~blkMULT() {}

	virtual Err init(vector<tVar>& args)
	{
		m_blkIn->getOutBuff( m_in );
		if (m_in != 0)
		{
			m_out = new T_OUT( m_in->size() );
			return Err(Err::eOK);
		}
		else
			return Err(Err::eErrWrongCast);
	}

	virtual Err update()
	{
		return Err(Err::eOK);
	}

	virtual Err start()
	{
		return Err(Err::eOK);
	}

	virtual Err process()
	{
		for (unsigned int i=0; i<m_out->size(); ++i)
			(*m_out)[i] = (*m_in)[i] * m_mult;
		return Err::eOK;
	}

	virtual Err stop()
	{
		if (m_out != 0)
			delete m_out;
		return Err::eOK;
	}

	virtual void getOutBuff(SampleVector*& vec)   { vec = (typeid(T_OUT) == typeid(SampleVector))  ? (SampleVector*)m_out : (SampleVector*)0; }
	virtual void getOutBuff(IQSampleVector*& vec)  { vec = (typeid(T_OUT) == typeid(IQSampleVector)) ? (IQSampleVector*)m_out : (IQSampleVector*)0; }

private:
	IBlock* m_blkIn;
	T_IN*   m_in;
	T_OUT*  m_out;
	float   m_mult;
};

template<class T_IN>
class sinkShow : public IBlock
{
public:

	sinkShow(IBlock* blkIn) : m_blkIn(blkIn), m_in(0) {}
	virtual ~sinkShow() {}

	virtual Err init(vector<tVar>& args)
	{
		m_blkIn->getOutBuff( m_in );
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

	virtual void process()
	{
		cout << "Val=" << *m_in << endl;
	}

	virtual void stop() {}

	virtual void getOutBuff(SampleVector*& vec)  { vec = 0; }
	virtual void getOutBuff(IQSampleVector*& vec) { vec = 0; }

private:
	IBlock* m_blkIn;
	T_IN*   m_in;
};

IBlock::~IBlock()
{
}

std::list<Err::tMsg>  Err::m_err;
//vector<IBlock *> IBlock::m_blkList;

