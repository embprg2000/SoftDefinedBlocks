/*
 * ClientConnect.hpp
 *
 *  Created on: Jan 10, 2017
 *      Author: evert
 */

#ifndef ClientConnect_HPP_
#define ClientConnect_HPP_

// http://stackoverflow.com/questions/17529421/sending-server-sent-events-through-a-socket-in-c
// http://www.howopensource.com/2014/12/introduction-to-server-sent-events/
// https://www.html5rocks.com/en/tutorials/eventsource/basics/
// http://www.howtocreate.co.uk/loadingExternalData.html
// http://dygraphs.com/ - Not used but maybe someday.

#include <mutex>
#include <queue>
#include <thread>
#include <atomic>

using namespace std;

class ClientConnect
{
public:

	struct tStats {
		int tx1;
		int tx2;
		int rx1;
		int rx2;
	};

	virtual ~ClientConnect();

	static ClientConnect& getInst();

	void setPort(int port) { _port = port; }

	bool start();

	bool stop();

	bool isTerminated() { return _bTerminated; }
	bool isRunning() { return _bSSErunning.load(); }

	void sendCommand(string msg);

	void sendCommand(string cmd, int id, int val);

	void sendCommand(string cmd, string id, double val);

	void sendCommand(string cmd, string id, string txt);

	string msgFromClient();

	void msecSleep(int mSec);

	bool isSendBuffEmpty();

	void getCommstats(tStats& stats) { stats = _stats; }

private:
	int    _port;
	bool   _bTerminated;
	tStats _stats;
	mutex  _mutexSend;
	mutex  _mutexRecv;
	thread _threadReq;
	thread _threadHTTP;
	queue<string> _sendQueue;
	queue<string> _recvQueue;
	atomic_bool _bFirstHTTPconn;
	atomic_bool _bStopThreads;
	atomic_bool _bSSErunning;

	ClientConnect();

	void SSEserver();

	int getFile(const string file, string& data);

	void httpRequests();
};

#endif /* ClientConnect_HPP_ */
