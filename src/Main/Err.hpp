/*
 * Err.hpp
 *
 *  Created on: Sep 14, 2016
 *      Author: evert
 */

#ifndef ERR_HPP_
#define ERR_HPP_

#include <list>
#include <iostream>

#include "../Main/IBlock.hpp"

class Err
{
public:
	enum eErr {
		eErrSinkAudio,
		eErrVectorSize,
		eErrWrongCast,
		eErrWrongFrequency,
		eErrSrcError,
		eErrWrongFileFormat,
		eErrUpdate,
		eDriverError,
		eErrCantWriteData,
		eErrCantOpenFile,
		eErrDataProcessError,
		eErrCantOpenDevice,
		eOK,
		eWarn,
		eLog
	};

private:
	struct tMsg {
		tMsg(std::string file, int line, eErr err, std::string msg) : _file(file), _line(line), _err(err), _msg(msg) {}
		std::string _file;
		int         _line;
		eErr        _err;
		std::string _msg;
	};
public:

	Err() : m_lastErr(eOK) {}
	Err(eErr e) : m_lastErr(e)
	{
		if (e != eOK)
			std::cout << "Error: " << e << std::endl;
	}
	Err(std::string file, int line, eErr err, std::string msg)
	{
		if (err < eOK)
			m_lastErr = err;
		m_err.push_back(tMsg(file, line, err, msg));
	}

	operator bool() { return m_lastErr < eOK; }  // Return TRUE when error

	operator Err()  { return m_lastErr; }

	static void showErrors()
	{
		for (auto it = m_err.begin(); it != m_err.end(); ++it)
		{
			std::cout << it->_file << ":" << it->_line << ": " << it->_msg << std::endl;
		}
	}

private:
	eErr  m_lastErr;
	static std::list<tMsg> m_err;
};

#define DBG_LOG(msg)  Err(__FILE__,__LINE__, Err::eLog, msg)
#define DBG_WRN(msg)  Err(__FILE__,__LINE__, Err::eWarn, msg)
#define DBG_ERR(err, msg)  Err(__FILE__,__LINE__, err, msg)

#endif /* ERR_HPP_ */
