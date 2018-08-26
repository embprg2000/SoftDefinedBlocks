// SocketException class
#ifndef SocketException_class
#define SocketException_class

#include <string>

// http://tldp.org/LDP/LG/issue74/tougher.html#3.1

class SocketException
{
public:
	SocketException ( std::string s ) : m_s ( s ) {};
	~SocketException (){};

	std::string description() { return m_s; }

private:

	std::string m_s;

};

#endif
