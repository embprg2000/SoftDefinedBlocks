// Implementation of the ServerSocket class

#include "../Server/ServerSocket.hpp"
#include "../Server/SocketException.hpp"

ServerSocket::ServerSocket ( int port )
{
	if ( ! Socket::create() )
	{
		throw SocketException ( "Could not create server socket." );
	}

	if ( ! Socket::bind ( port ) )
	{
		throw SocketException ( "Could not bind to port." );
	}

	if ( ! Socket::listen() )
	{
		throw SocketException ( "Could not listen to socket." );
	}

}

ServerSocket::~ServerSocket()
{
}


const ServerSocket& ServerSocket::operator << ( const std::string& s ) const
{
	if ( ! Socket::send ( s ) )
	{
		throw SocketException ( "Could not write to socket." );
	}
	return *this;
}


const ServerSocket& ServerSocket::operator >> ( std::string& s ) const
{
	if ( Socket::recv ( s ) < 0)
	{
		throw SocketException ( "Could not read from socket." );
	}
	return *this;
}

bool ServerSocket::accept ( ServerSocket& sock )
{
	bool bRet = Socket::accept ( sock );

//	if ( bRet == false )
//	{
//		throw SocketException ( "Could not accept socket." );
//	}
	return bRet;
}
