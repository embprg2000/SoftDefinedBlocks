/*
 * ProcessScript.cpp
 *
 *  Created on: Jan 17, 2017
 *      Author: evert
 */


#include "../Main/ProcessScript.hpp"
#include "../Main/SeqControl.hpp"
#include "tVar.hpp"

//ProcessScript::ProcessScript()
//{
//	_blkArgs["srcSDR"]      = "III";
//	_blkArgs["approxRMSIQ"] = "SSII";
//	_blkArgs["lowPass"]     = "ID";
//	_blkArgs["downsample"]  = "DDDI";
//	_blkArgs["highPassIIR"] = "D";
//	_blkArgs["lowPassRC"]   = "D";
//	_blkArgs["sinkAudio"]   = "";
//}

ProcessScript& ProcessScript::getInstance()
{
	static ProcessScript obj;
	return obj;
}

void ProcessScript::attach(IBlock* ptr, string args)
{
	_blkArgs[ptr->getBlkName()] = tBlk(ptr,args);
}

bool ProcessScript::instantiateBlk(string name, vector<tVar> args)
{
	cout << "Instantiate: " << name << "(";
	if (args.size() > 0)
	{
		cout << args[0].getStr();
		for_each(args.begin()+1,args.end(), [=](tVar& it) { cout << ", " << it.getStr(); });
	}
	cout << ")";

	map<string,tBlk>::iterator it;

	it = _blkArgs.find(name);
	if (it != _blkArgs.end())
	{
		tBlk& ptr = it->second;
		IBlock* pBlk = ptr._ptr->copyBlock();

		pBlk->setInputBlk( m_prevBlk );
		pBlk->init(args);
		cout << " - Sample rate:" << pBlk->getSampleRate();

		SeqControl::getInstance().addBlock(name, pBlk);

		m_prevBlk = pBlk;
	}
	cout << endl;
	return true;
}

bool ProcessScript::processBlock(string& line)
{
	bool bRet = false;
	string::size_type p1 = line.find("(");

	if (p1 != string::npos)
	{
		string blk = line.substr(0,p1);

		for (auto it = _blkArgs.begin(); it != _blkArgs.end(); ++it)
		{
			if (it->first == blk)
			{
				unsigned int nArgs = it->second._args.length();
				string::size_type p2 = line.find_last_of(")");
				string allArgs = line.substr(p1+1,p2-p1-1);
				vector<string> argStr;

				if (allArgs.length() > 0)
				{
					p1 = 0;
					while ((p2 = allArgs.find(",",p1)) != string::npos)
					{
						argStr.push_back( allArgs.substr(p1,p2-p1) );
						p1 = p2+1;
					}
					argStr.push_back( allArgs.substr(p1) );
				}

				if (argStr.size() == nArgs)
				{
					vector<tVar> args;

					for (unsigned int i=0; i<nArgs; ++i)
					{
						char type = it->second._args[i];
						if (type == 'D')
							args.push_back( (double) evalL1(argStr[i]) );
						else if (type == 'I')
							args.push_back( (int) evalL1(argStr[i]) );
						else if (type == 'S')
						{
							string tmp = argStr[i];

							if (tmp[0] == '"' && tmp[tmp.size()-1] == '"')
								args.push_back( tmp.substr(1,tmp.size() - 2) );
							else
								throw Error("Error: String expected.");
						}
					}
					instantiateBlk(blk, args);
				}
				else
				{
					throw Error(string("Block ") + blk + " argument mismatch.");
				}
				bRet = true;
			}
		}
		if (bRet == false)
			throw Error(string("Block ") + blk + " not found.");
	}
	return bRet;
}

bool ProcessScript::processFile(const string file)
{
	bool  bRet = false;

	try {
		string line;
		ifstream fs(file);
		int lineNum = 1;

		if (fs.is_open() == false)
			throw Error(string("File ") + file + " not found.");

		while(!fs.eof())
		{
			Error::setLineNum(lineNum);

			getline(fs,line);

			line.erase( remove_if(line.begin(), line.end(),
		                    [](char x){return isspace(x);}), line.end());

			if (processLine(line) == false)
				processBlock(line);
			++lineNum;
		}
		bRet = true;
		fs.close();
	}
	catch (Error& e) {
		cout << e << endl;
	}
	return bRet;
}



