/*
 * ProcessScript.hpp
 *
 *  Created on: Jan 17, 2017
 *      Author: evert
 */

#ifndef PROCESSSCRIPT_HPP_
#define PROCESSSCRIPT_HPP_

#include "SimpleParser.hpp"
#include "tVar.hpp"
#include "IBlock.hpp"

class ProcessScript : SimpleParser
{
	struct tBlk {
		tBlk() : _ptr(nullptr), _args("") {}
		tBlk(IBlock* ptr, string args) : _ptr(ptr), _args(args) {}
		tBlk(const tBlk& blk ) : _ptr(blk._ptr), _args(blk._args) {}
		//tBlk& operator = (IBlock& ptr, string args) { return *this; }
		IBlock*  _ptr;
		string   _args;
	};

	IBlock* m_prevBlk;
	map<string, tBlk> _blkArgs;

	bool instantiateBlk(string blk, vector<tVar> args);
	bool processBlock(string& line);

	ProcessScript(): m_prevBlk(0) {}
	virtual ~ProcessScript() {}

public:

	static ProcessScript& getInstance();

	void attach(IBlock* ptr, string args);
	bool processFile(const string file);

};

#endif /* PROCESSSCRIPT_HPP_ */
