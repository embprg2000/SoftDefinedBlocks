// Definition of the ServerSocket class

#ifndef ServerSocket_class
#define ServerSocket_class

#include "../Server/Socket.hpp"


class ServerSocket : private Socket
{
public:

	ServerSocket ( int port );
	ServerSocket (){};
	virtual ~ServerSocket();

	const ServerSocket& operator << ( const std::string& ) const;
	const ServerSocket& operator >> ( std::string& ) const;

	bool accept ( ServerSocket& );

	bool sendFile(std::string& path) { return Socket::sendFile(path); }

	void set_non_blocking(const bool b) { Socket::set_non_blocking(b); }

};


#endif
