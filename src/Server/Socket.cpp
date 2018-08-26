#include "../Server/Socket.hpp"

//#include "string.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>

// http://tldp.org/LDP/LG/issue74/tougher.html#3.1

// /usr/include/asm/errno.h
//#define EPERM            1      /* Operation not permitted */
//#define ENOENT           2      /* No such file or directory */
//#define ESRCH            3      /* No such process */
//#define EINTR            4      /* Interrupted system call */
//#define EIO              5      /* I/O error */
//#define ENXIO            6      /* No such device or address */
//#define E2BIG            7      /* Arg list too long */
//#define ENOEXEC          8      /* Exec format error */
//#define EBADF            9      /* Bad file number */
//#define ECHILD          10      /* No child processes */
//#define EAGAIN          11      /* Try again */
//#define ENOMEM          12      /* Out of memory */

Socket::Socket() : m_sock ( -1 )
{
	memset ( &m_addr, 0, sizeof ( m_addr ) );
}

Socket::~Socket()
{
	if ( is_valid() )
		::close ( m_sock );
}

bool Socket::create()
{
	m_sock = socket ( AF_INET, SOCK_STREAM, 0);

	if ( ! is_valid() )
		return false;


	// TIME_WAIT - argh
	int on = 1;
	if ( setsockopt ( m_sock, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &on, sizeof ( on ) ) == -1 )
		return false;

	return true;
}

bool Socket::bind ( const int port )
{

	if ( ! is_valid() )
		return false;

	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.s_addr = INADDR_ANY;
	m_addr.sin_port = htons ( port );

	int bind_return = ::bind ( m_sock, ( struct sockaddr * ) &m_addr,
			sizeof ( m_addr ) );


	if ( bind_return == -1 )
		return false;

	return true;
}

bool Socket::listen() const
{
	if ( ! is_valid() )
	{
		return false;
	}

	int listen_return = ::listen ( m_sock, MAXCONNECTIONS );

	if ( listen_return == -1 )
		return false;

	return true;
}


bool Socket::accept ( Socket& new_socket ) const
{
	fd_set readfds;
	struct timeval tv;
	int addr_length = sizeof ( m_addr );

	FD_ZERO(&readfds);
	FD_SET(m_sock, &readfds);

	// Initialize time out struct
	tv.tv_sec = 0;
	tv.tv_usec = 10 * 1000;

	//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
	int activity = select( m_sock+1, &readfds , NULL , NULL , &tv);

	if ((activity < 0) && (errno!=EINTR))
	{
		printf("select error");
	}

	//If something happened on the master socket , then its an incoming connection
	if (FD_ISSET(m_sock, &readfds))
	{
		new_socket.m_sock = ::accept ( m_sock, ( sockaddr * ) &m_addr, ( socklen_t * ) &addr_length );

		if ( new_socket.m_sock > 0 )
			return true;
	}
	return false;
}

bool Socket::sendFile(std::string& path)
{
	FILE *from = fopen(path.c_str(), "r");

	if (from) {
		int n;
		FILE *to;
		char data[4096];

		to = fdopen(m_sock, "a+");
		if (to)
		{
			while ((n = fread(data, 1, sizeof(data), from)) > 0)
				fwrite(data, 1, n, to);
			fclose(to);
			return true;
		}
		fclose(from);
	}
	return false;
}

bool Socket::send ( const std::string s ) const
{
	int status = ::send ( m_sock, s.c_str(), s.size(), MSG_NOSIGNAL );
	if ( status == -1 )
	{
		std::cout << "status == -1   errno == " << errno << " '" << strerror(-status) << "'  in Socket::send\n";
		return false;
	}
	else
		return true;
}

int Socket::recv ( std::string& s ) const
{
	char buf [ MAXRECV + 1 ];

	s = "";

	memset ( buf, 0, MAXRECV + 1 );

	int status = ::recv ( m_sock, buf, MAXRECV, 0 );

	if ( status == -1 )
	{
		std::cout << "status == -1   errno == " << errno << " '" << strerror(-status) << "'  in Socket::recv\n";
		return 0;
	}
	else if ( status == 0 )
	{
		return 0;
	}
	else
	{
		s = buf;
		return status;
	}
}

bool Socket::connect ( const std::string host, const int port )
{
	if ( ! is_valid() ) return false;

	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons ( port );

	int status = inet_pton ( AF_INET, host.c_str(), &m_addr.sin_addr );

	if ( errno == EAFNOSUPPORT ) return false;

	status = ::connect ( m_sock, ( sockaddr * ) &m_addr, sizeof ( m_addr ) );

	if ( status == 0 )
		return true;
	else
		return false;
}

void Socket::set_non_blocking ( const bool b )
{

	int opts;

	opts = fcntl ( m_sock, F_GETFL );

	if ( opts < 0 )
		return;

	if ( b )
		opts = ( opts | O_NONBLOCK );
	else
		opts = ( opts & ~O_NONBLOCK );

	fcntl ( m_sock, F_SETFL, opts );
}
