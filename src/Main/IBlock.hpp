/*
 * IBlock.hpp
 *
 *  Created on: Sep 9, 2016
 *      Author: evert
 */

#ifndef IBLOCK_HPP_
#define IBLOCK_HPP_

#include <vector>
#include <typeinfo>
#include <atomic>
#include <complex>

#include "../Main/Err.hpp"
#include "../Main/tVar.hpp"

using namespace std;

typedef float SampleType;
typedef std::vector<SampleType>   SampleVector;

typedef std::complex<SampleType>  IQSample;
typedef std::vector<IQSample>     IQSampleVector;


extern atomic_bool bTerminate;

class IBlock
{
public:

	IBlock() {} //m_blkList.push_back(this); }
	virtual ~IBlock();

	virtual Err init(vector<tVar>& args) = 0;  // Initialize everything in the current block
	virtual Err start()   = 0;  // Start threads or anything else just before collecting or processing the data buffers.
	virtual Err update()  = 0;  // Called when variables, sample frequency cutoff frequencies etc, are changes.
	virtual Err process() = 0;  // Process one buffer.
	virtual Err stop()    = 0;  // Close, disconnect, free memory etc.

	virtual IBlock* copyBlock() = 0;

	virtual Err setInputBlk(IBlock* ptr) = 0;
	virtual string getBlkName() { return m_blkName; }
	virtual string getName() { return m_name; }
	virtual void   setName(string name) { m_name = name; }

	virtual double getSampleRate() = 0;

	virtual void getOutBuff(SampleVector*& vec) = 0;
	virtual void getOutBuff(IQSampleVector*& vec) = 0;

private:
	string m_name;
	string m_blkName;
};

template< class T_OUT>
class blkSource : public IBlock
{
public:

	blkSource(double sampleRatio, int bufSize=1) : m_out(0), m_sampleRate(sampleRatio), m_bufSize(bufSize) {}

	virtual ~blkSource()
	{
		if (m_out != 0)
			delete m_out;
	}

	virtual Err init(vector<tVar>& args)
	{
		m_out = new T_OUT( m_bufSize );
		return (m_out == 0) ? Err(Err::eErrWrongCast) : Err(Err::eOK);
	}

	virtual Err setInputBlk(IBlock* ptr) { return Err::eOK; }

	virtual double getSampleRate() { return m_sampleRate; }

	virtual Err start()   { return Err::eOK; }
	virtual Err update()  { return Err::eOK; }
	virtual Err stop()    { return Err::eOK; }

	virtual void getOutBuff(SampleVector*& vec)    { vec = (typeid(T_OUT) == typeid(SampleVector))   ? (SampleVector*)m_out   : (SampleVector*)0; }
	virtual void getOutBuff(IQSampleVector*& vec)  { vec = (typeid(T_OUT) == typeid(IQSampleVector)) ? (IQSampleVector*)m_out : (IQSampleVector*)0; }

protected:

	T_OUT*	m_out;
	double	m_sampleRate;
	int		m_bufSize;
};

template<class T_IN, class T_OUT>
class blkCommon : public IBlock
{
public:

	blkCommon() : m_blkIn(0), m_in(0), m_out(0), m_sampleRate(1.0), m_enabled(true) {}

	// @# TODO: Next line should be removed.
	blkCommon(IBlock* blkIn) : m_blkIn(blkIn), m_in(0), m_out(0), m_sampleRate(1.0), m_enabled(true) {}

	virtual ~blkCommon()
	{
		if (m_out != 0)
			delete m_out;
	}

	virtual Err setInputBlk(IBlock* ptr) { m_blkIn = ptr; return Err::eOK; }

	Err init()
	{
		m_blkIn->getOutBuff( m_in );
		if (m_in != 0)
		{
			m_out = new T_OUT( m_in->size() );
			return (m_out == 0) ? Err(Err::eErrWrongCast) : Err(Err::eOK);
		}
		else
			return Err(Err::eErrWrongCast);
	}

	virtual Err process()
	{
		m_sampleRate = (double)m_out->size() / (double)m_in->size();
		return Err::eOK;
	}

	virtual double getSampleRate() { return m_sampleRate * m_blkIn->getSampleRate(); }

	virtual Err start()   { return Err::eOK; }
	virtual Err update()  { return Err::eOK; }
	virtual Err stop()    { return Err::eOK; }

	virtual void getOutBuff(SampleVector*& vec)    { vec = (typeid(T_OUT) == typeid(SampleVector))   ? (SampleVector*)m_out   : (SampleVector*)0; }
	virtual void getOutBuff(IQSampleVector*& vec)  { vec = (typeid(T_OUT) == typeid(IQSampleVector)) ? (IQSampleVector*)m_out : (IQSampleVector*)0; }

	bool& getEnabled() { return m_enabled; }

protected:

	IBlock* m_blkIn;
	T_IN*	m_in;
	T_OUT*	m_out;
	double	m_sampleRate;
	bool	m_enabled;      // This flag can be read by the next block. E.g. to enable writing to file or output audio after squelch block.
};

template<class T_IN>
class blkSink : public IBlock
{
public:

	blkSink(IBlock* blkIn) : m_blkIn(blkIn), m_in(0), m_sampleRate(0.0) {}

	virtual ~blkSink()
	{
	}

	virtual Err setInputBlk(IBlock* ptr) { m_blkIn = ptr; return Err::eOK; }

	Err init()
	{
		m_blkIn->getOutBuff( m_in );
		m_sampleRate = m_blkIn->getSampleRate();
		return (m_in == 0) ? Err(Err::eErrWrongCast) : Err(Err::eOK);
	}

	virtual Err process()
	{
		return Err::eOK;
	}

	virtual double getSampleRate() { return m_sampleRate; }

	virtual Err start()   { return Err::eOK; }
	virtual Err update()  { return Err::eOK; }
	virtual Err stop()    { return Err::eOK; }

	virtual void getOutBuff(SampleVector*& vec)    { vec = 0; }
	virtual void getOutBuff(IQSampleVector*& vec)  { vec = 0; }

protected:

	IBlock* m_blkIn;
	T_IN*	m_in;
	double	m_sampleRate;
};

#endif /* IBLOCK_HPP_ */
