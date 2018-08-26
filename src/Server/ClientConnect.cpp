//============================================================================
// Name        : ClientConnect.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

// http://stackoverflow.com/questions/17529421/sending-server-sent-events-through-a-socket-in-c
// http://www.howopensource.com/2014/12/introduction-to-server-sent-events/
// https://www.html5rocks.com/en/tutorials/eventsource/basics/
// http://www.howtocreate.co.uk/loadingExternalData.html
// http://dygraphs.com/ - Not used but maybe someday.

#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/stat.h>

#include "ClientConnect.hpp"
#include "ServerSocket.hpp"
#include "SocketException.hpp"

#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"  // Time format for http response


ClientConnect::ClientConnect() : _port(0), _bTerminated(false)
{

}

ClientConnect::~ClientConnect()
{
	stop();
}

ClientConnect& ClientConnect::getInst()
{
	static ClientConnect obj;

	return obj;
}

bool ClientConnect::start()
{
	_stats.tx1 = 0;
	_stats.tx2 = 0;
	_stats.rx1 = 0;
	_stats.rx2 = 0;
	_bSSErunning.store(false);
	_bFirstHTTPconn = true;
	_bStopThreads.store(false);
	_threadHTTP = thread( &ClientConnect::httpRequests, this );
	_threadReq  = thread( &ClientConnect::SSEserver, this );
	_bTerminated = false;
	// SSE server is started after first HTTP connection.

	return true;
}

bool ClientConnect::stop()
{
	bool bQueueEmpty = false;

	cout << "Stop threads" << endl;
	while (bQueueEmpty == false)
	{
		unique_lock<mutex> lock(_mutexSend);
		while ( !_sendQueue.empty() )
			_sendQueue.pop();
		bQueueEmpty =_sendQueue.empty();
		lock.unlock();

		lock = unique_lock<mutex>(_mutexRecv);
		while ( !_recvQueue.empty() )
			_recvQueue.pop();
		bQueueEmpty =_recvQueue.empty();
		lock.unlock();

		msecSleep( 500 );
	}
	_bStopThreads.store(true);

	//if (_bSSErunning == true)
		_threadReq.join();
	_threadHTTP.join();
	_bTerminated = true;

	return true;
}

void ClientConnect::sendCommand(string msg)
{
	if (_bSSErunning == true)
	{
		unique_lock<mutex> lock(_mutexSend);
		_sendQueue.push( msg );
		lock.unlock();
		_stats.rx1++;
		_stats.rx2++;
	}
}

void ClientConnect::sendCommand(string cmd, int id, int val)
{
	ostringstream str;

	str << cmd << "," << id << "," << val;
	sendCommand( str.str() );
}

void ClientConnect::sendCommand(string cmd, string id, double val)
{
	ostringstream str;

	str << cmd << "," << id << "," << val;
	sendCommand( str.str() );
}

void ClientConnect::sendCommand(string cmd, string id, string txt)
{
	sendCommand( cmd + "," + id + "," + txt );
}

string ClientConnect::msgFromClient()
{
	string ret;

	unique_lock<mutex> lock(_mutexRecv);
	if ( !_recvQueue.empty() )
	{
		ret = _recvQueue.front();
		_recvQueue.pop();
		_stats.tx1++;
		_stats.tx2++;
	}
	lock.unlock();

	return ret;
}

bool ClientConnect::isSendBuffEmpty()
{
	bool bRet = true;

	unique_lock<mutex> lock(_mutexRecv);
	if ( !_recvQueue.empty() )
	{
		bRet = false;
	}
	lock.unlock();

	return bRet;
}

void ClientConnect::msecSleep(int mSec)
{
	this_thread::sleep_for (chrono::milliseconds(mSec));
}

void ClientConnect::SSEserver()
{
	ServerSocket server ( _port + 1 );
	ServerSocket new_sock;

//	new_sock.set_non_blocking( true );

	cout << "Waiting in ClientConnect::SSEserver() for connection" << endl;

	// http://developerweb.net/viewtopic.php?id=2933
	// http://www.binarytides.com/multiple-socket-connections-fdset-select-linux/
	while ( server.accept ( new_sock ) == false && _bStopThreads.load() == false)
		msecSleep( 100 );

	cout << "*** SSE connected ***" << endl;

	try
	{
		string tmp;
		short  timeout = 0;

		new_sock << "HTTP/1.1 200 OK\r\n";
		new_sock << "Access-Control-Allow-Origin: *\r\n";
		new_sock << "Cache-Control: no-cache\r\n";
		new_sock << "Connection: Keep-Alive\r\n";
		new_sock << "Content-Type: text/event-stream\r\n\r\n";

		_bSSErunning.store( true );
		while (_bStopThreads.load() == false)
		{
			string msg;

			unique_lock<mutex> lock(_mutexSend);
			if ( !_sendQueue.empty() )
			{
				msg = _sendQueue.front();
				_sendQueue.pop();
				_stats.rx2--;
			}
			lock.unlock();

			if (msg.length() > 0)
			{
				ostringstream snd;

				snd << msg.length() << "\r\ndata: " << msg << "\n\r\n";
				new_sock << snd.str();
				msg = "";
				timeout = 0;
			}
			else
			{
				if (timeout < 500)
				{
					msecSleep( 10 );
					timeout += 10;
				}
				else
				{
					timeout = 0;
					new_sock << "1\r\ndata: .\n\r\n";
				}
			}
		}
	}
	catch (SocketException& e)
	{
		cout << "Exception was caught (SSEserver):" << e.description() << "\nExiting.\n";
		stop();
	}
}

int ClientConnect::getFile(const string file, string& data)
{
	ifstream fs(file);
	string line;

	data = "";
	if (fs.is_open() == true)
	{
		while(!fs.eof())
		{
			getline(fs,line);
			data += line + '\n';
		}
		fs.close();
	}
	return data.length();
}

void ClientConnect::httpRequests()
{
	ServerSocket server ( _port );
	map<string,string> mimeMap;

	mimeMap["html"] = "text/html";
	mimeMap["htm"]  = "text/html";
	mimeMap["js"]   = "text/html";
	mimeMap["jpg"]  = "image/jpeg";
	mimeMap["jpeg"] = "image/jpeg";
	mimeMap["gif"]  = "image/gif";
	mimeMap["png"]  = "image/png";
	mimeMap["css"]  = "text/css";
	mimeMap["au"]   = "audio/basic";
	mimeMap["wav"]  = "audio/wav";
	mimeMap["avi"]  = "video/x-msvideo";
	mimeMap["mpeg"] = "video/mpeg";
	mimeMap["mpg"]  = "video/mpeg";
	mimeMap["mp3"]  = "audio/mpeg";

	while (_bStopThreads.load() == false)
	{
		try
		{
			string data;
			ServerSocket new_sock;

			cout << "Waiting for HTTP connection at port: " << _port << endl;
			while ( server.accept ( new_sock ) == false && _bStopThreads.load() == false)
				;

			cout << "*** HTTP connected ***" << endl;
//			if (_bFirstHTTPconn == true)
//			{
//				_threadReq  = thread( &ClientConnect::SSEserver, this );
//				_bFirstHTTPconn = false;
//			}

			new_sock >> data;
			cout << "---" << data.substr(0,45) << endl;

			if (data.compare(0,13,"GET /command/") == 0)
			{
				// A command was send.
				int    idx = data.find(" HTTP/1.1");
				string cmd = data.substr(13, idx - 13);

				unique_lock<mutex> lock(_mutexRecv);
				_recvQueue.push( cmd );
				lock.unlock();
				_stats.tx2--;
				data = "<br><center>Command " + cmd + " received</center></br>";

				ostringstream tmp;

				cmd = "Received: " + cmd + "\r\n";
				tmp << "Content-length: " << cmd.length() << "\r\n";
				new_sock << tmp.str();
				new_sock << "Content-Type: text/plain\r\n";
				new_sock << "Connection: close\r\n\r\n";

				new_sock << cmd;
			}
			else
			{
				if (data.compare(0,5,"GET /") == 0)
				{
					string mime;
					int contentLength = 0;
					bool bNotFound = false;
					string ext, name, file;

					int idx = data.find_first_of(" ",5);

					if (idx > 0)
					{
						file = data.substr(5,idx-5);
						if (file == "")
						{
							file = "index.html";
							name = "index";
							ext  = "html";
						}
						else
						{
							idx  = file.find_first_of(".");
							if (idx > 0)
							{
								name = file.substr(0,idx);
								ext  = file.substr(idx+1);
							}
						}
					}

					map<string,string>::iterator it = mimeMap.find(ext);

					if (it != mimeMap.end())
					{
						struct stat results;

						mime = it->second;
						if (stat(file.c_str(), &results) == 0)
							contentLength = (int) results.st_size;
						else
							bNotFound = true;
					}
					else
						bNotFound = true;

					char timebuf[128];
					ostringstream header;
					time_t now = time(NULL);

					strftime(timebuf, sizeof(timebuf), RFC1123FMT, gmtime(&now));

					if (bNotFound == false)
						header << "HTTP/1.0 200 OK\r\n";
					else
						header << "HTTP/1.0 404 OK\r\n";

					header << "Server: webserver/1.0\r\n";
					header << "Date: " << timebuf << "\r\n";
					header << "Content-length: " << contentLength << "\r\n";
					header << "Content-Type: " << mime << "\r\n";
					header << "Connection: close\r\n\r\n";

					new_sock << header.str();

					if (bNotFound == false)
						new_sock.sendFile(file);
					else
					{
						cout << "File: " << file << " not found." << endl;
					}
				}
				else
				{
					cout << "Should not get here" << endl;
				}
			}
		}
		catch (SocketException& e)
		{
			cout << "Exception was caught (httpRequests):" << e.description() << "\nExiting.\n";
		}
	}
}




