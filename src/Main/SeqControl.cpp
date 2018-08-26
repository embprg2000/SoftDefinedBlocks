/*
 * SeqControl.cpp
 *
 *  Created on: Sep 17, 2016
 *      Author: evert
 */

#include "../Main/SeqControl.hpp"

#include <csignal>
#include <chrono>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <algorithm>

#include <memory>
#include <utility>

#include "../SourceBlocks/sourceFile.hpp"
#include "../Main/ProcessScript.hpp"

SeqControl::SeqControl()
	: m_sseTime(0)
	, m_sampleRate(1)
	, m_scriptLoaded(false)
	, m_widget( WidgetContainer::getInst() )
	, m_varList( nullptr )
	, m_commStats( nullptr )
	, m_menu( nullptr )
{
	// Catch Ctrl-C and SIGTERM
	struct sigaction sigact;
	sigact.sa_handler = &SeqControl::handle_sigterm;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = SA_RESETHAND;
	if (sigaction(SIGINT, &sigact, NULL) < 0 || sigaction(SIGTERM, &sigact, NULL) < 0)
		DBG_WRN( std::string("WARNING: can not install SIGINT handler") );

	m_varList = new WidgetVarList("idVars", "clssVars", {"Frame", "Delta", "Audio"} );
	m_widget.add( m_varList );

	m_commStats = new WidgetCommStats("idCommStats");
	m_widget.add( m_commStats );

	m_menu = new WidgetMenu("idSideMenu", {"Home", "FM Radio", "NOAA", "ADS", "Morse", "Pager"});
	m_widget.add( m_menu );
}

SeqControl::~SeqControl()
{
	deleteBlocks();

	delete m_varList;
	delete m_commStats;
	delete m_menu;

}

void SeqControl::deleteBlocks()
{
	for (auto& ptr : m_blkList)
	{
		if (ptr != this)
		{
			std::cout << "Deleting: " << ptr->getName() << std::endl;
			delete ptr;
		}
	}
	m_blkList.clear();
}

SeqControl& SeqControl::getInstance()
{
	static SeqControl obj;

	return obj;
}

IBlock* SeqControl::addBlock(string name, IBlock* blk)
{
	blk->setName(name);
	m_blkList.push_back( blk );
	return blk;
}

bool SeqControl::removeBlock(string name)
{
	for(auto ptr : m_blkList)
	{
		if (ptr->getName() == name)
		{
			m_blkList.erase(m_itr);
			return true;
		}
	}
	return false;
}

Err SeqControl::init(vector<tVar>& args)
{
	Err err;

	m_sseTime = 0;
	return err;
}

Err SeqControl::update()
{
	Err err;

	for (m_itr = m_blkList.begin(); m_itr != m_blkList.end() && err == false; ++m_itr)
	{
		if (*m_itr != this)
		{
			err = (*m_itr)->update();
			if (err == true)
				DBG_LOG("Error update(): " + (*m_itr)->getName());
		}	}
	return err;
}

Err SeqControl::start()
{
	Err err;

	for (m_itr = m_blkList.begin(); m_itr != m_blkList.end() && err == false; ++m_itr)
	{
		if (*m_itr != this)
		{
			err = (*m_itr)->start();
			if (err == true)
				DBG_LOG("Error start(): " + (*m_itr)->getName());
		}
	}
	return err;
}

Err SeqControl::process()
{
	Err err;

	m_sampleRate = 1;
	for (m_itr = m_blkList.begin(); m_itr != m_blkList.end() && err == false; ++m_itr)
	{
		if (*m_itr != this)
		{
			err = (*m_itr)->process();
			m_sampleRate = (*m_itr)->getSampleRate();
			if (err == true)
				DBG_LOG("Error process(): " + (*m_itr)->getName());
		}
	}
	return err;
}

Err SeqControl::stop()
{
	Err err;

	for (m_itr = m_blkList.begin(); m_itr != m_blkList.end() && err == false; ++m_itr)
	{
		if (*m_itr != this)
		{
			err = (*m_itr)->stop();
			if (err == true)
				DBG_LOG("Error stop(): " + (*m_itr)->getName());
		}
	}
	return err;
}


/** Handle Ctrl-C and SIGTERM. */
void SeqControl::handle_sigterm(int sig)
{
	bTerminate.store(true);

	string msg = "\nGot signal ";
	msg += strsignal(sig);
	msg += ", stopping ...\n";

	DBG_LOG( msg );
}

bool SeqControl::loadScript()
{
	string file = m_menu->getFileName();

	if (file.size() > 0)
	{
		string msgFromClient;
		ProcessScript& prScript = ProcessScript::getInstance();

		deleteBlocks();
		m_scriptLoaded = prScript.processFile(string("Scripts/") + file + ".sdb");
		m_menu->file2client();
	}
	m_widget.init();
	return true;
}

void SeqControl::run(string file)
{
	Err err(Err::eOK);
	int frame = 1;
	double tSlice = 0;

	m_widget.startServer(1234);

	while (m_widget.isServerConnected() == false)
		;

	string msgFromClient;

	cout << "Waiting for connect confirmation" << endl;
	while (msgFromClient.substr(0,10) != "Connected")
	{
		m_widget.msecSleep(100);
		msgFromClient = m_widget.msgFromClient();
	}

	cout << " +++ CONNECTED +++ " << endl;

	m_widget.init();

//	while (m_widget.isSendBuffEmpty() == false)
//	{
//		m_widget.msecSleep(100);
//	}

	// Main loop
	while (bTerminate.load() == false && err == false)
	{
		if (m_menu->_bIsDirty == true)
		{
			stop();
			loadScript();
			err = start();
			frame = 0;
		}
		else
		{
			if (m_scriptLoaded == false)
			{
				string msgFromClient = m_widget.msgFromClient();

				if (m_widget.process(msgFromClient) == true) {
					cout << "Msg:" << msgFromClient << endl;
				}
				m_widget.msecSleep(500);
				continue;
			}

			const auto start = chrono::system_clock::now();

			err = process();

			const auto stop = chrono::system_clock::now();
			const auto d_actual = chrono::duration_cast<chrono::microseconds>(stop - start).count();

			if (d_actual > 10)
			{
				if (d_actual < 1e6)
				{
					tSlice += d_actual;
				}
				else
				{
					m_widget.msecSleep(10);
					tSlice += 10000;
				}
			}
			else
			{
				bTerminate = true;
				cout << "Loop to fast please add at least a sleep!" << endl;
			}

			if (tSlice > 100000)
			{
				string msgFromClient = m_widget.msgFromClient();

				if (m_widget.process(msgFromClient) == true) {
					cout << "Msg:" << msgFromClient << endl;
				}

				// {"Frame", "Delta", "Audio"}
				m_varList->setValues( {(double)frame++, (double)d_actual, (double)m_sampleRate} );
				tSlice = 0;
			}
		}
	}

	fprintf(stderr,"\n\n");
	Err::showErrors();
}

