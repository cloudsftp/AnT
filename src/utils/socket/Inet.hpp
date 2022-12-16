/* 
 * Copyright (C) 1999-2004 the AnT project,
 * Department of Image Understanding,
 * University of Stuttgart, Germany.
 * 
 * This file is part of AnT,
 * a simulation and analysis tool for dynamical systems.
 * 
 * AnT is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * AnT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 *
 * $Id: Inet.hpp,v 1.10 2004/05/10 08:53:34 schanzml Exp $
 *
 */

#ifndef INET_HPP
#define INET_HPP

#ifndef ANT_CONFIG_H
#include "config.h"
#endif

#include <string>
using std::string;
#include <vector>
using std::vector;
#include <iostream>

#if (! (ANT_HAS_WIN_ENV)) || (defined __CYGWIN__)

#include <netinet/in.h> /*:  provides e.g. 'in_port_t', 'in_addr_t' */
#include <netdb.h>      /*: 'gethostbyname', struct 'hostent' */
#include <arpa/inet.h>  /*: 'inet_ntoa', 'inet_top', inet_addr, ... */
//#include <cstdio>

#include <errno.h>
#include <sys/socket.h> /* AF_INET, AF_INET6, ... */

#include <unistd.h>     /*: for 'read', 'write', 'close', 'gethostname', ... */

#endif /* ( ! ANT_HAS_WIN_ENV) */

#if ANT_HAS_WIN_ENV

#ifndef INET_ADDRSTRLEN /* not defined under win. */
#define INET_ADDRSTRLEN 16
#endif

#if (! defined __CYGWIN__)
#include <winsock.h>
typedef typeof(sockaddr_in().sin_port) in_port_t;
typedef typeof(in_addr().s_addr) in_addr_t;
typedef int socklen_t; /* this is the last argument type in ::bind */
#endif

typedef typeof(sockaddr().sa_family) sa_family_t;

#endif /* ANT_HAS_WIN_ENV */


using std::size_t;
#include <cstring> /* 'memset' */

#include <cassert>

#include "MainPtr.hpp"


string getStandardHostname ();


class Port
{
public:
  typedef int Number;

  const Port::Number portNumber;

  static Port::Number toNumber (const string& portNrStr);
  static Port::Number toNumber (const char* chars);

  Port (Port::Number portNr);
  Port (const string& portNrStr);
  Port (const char* chars);
}; // class Port



template <const sa_family_t AddressFamily = AF_INET> class AF;
/* the adress family constants (AF_INET, AF_INET6, ...) are defined in
   <sys/socket.h> */

template<> class AF<AF_INET>
{
public:
  typedef in_addr_t Number;
  typedef struct in_addr Address;
  typedef struct sockaddr_in Socket;
  static const sa_family_t addrFamily = AF_INET;
  static const int addrStrLength = INET_ADDRSTRLEN; // <netinet/in.h>

  static bool getAddress (const char* addrStr, Address& result)
  {
#if ANT_HAS_WIN_ENV
#if 0 /* commented out */
    return (inet_aton (addrStr, &result) != 0);
#else
    result.s_addr = inet_addr (addrStr);
    return (result.s_addr != INADDR_NONE);
#endif
#else
    int flag = inet_pton (addrFamily, addrStr, &result);
    assert (flag != -1); /* would mean unsupported adress family */
    return (flag == 1); /* flag == 0 by failure */
#endif
  }

  static string toString (const Address& inetAddr)
  {
    return inet_ntoa (inetAddr);
  }

  static Socket getSocket (const Address& inetAddr, const Port& port)
  {
    Socket result;
    memset (&result, '\0', sizeof (result));
    result.sin_family = addrFamily;
    result.sin_port = htons (port.portNumber);
    //result.sin_addr.s_addr = /* htonl */ (inetAddr.s_addr);
    result.sin_addr = inetAddr;

    return result;
  }
}; // class AF<AF_INET>

#if (! ANT_HAS_WIN_ENV)
template<> class AF<AF_INET6>
{
public:
  /* typedef in6_addr_t Number; ??? */
  typedef struct in6_addr Address;
  typedef struct sockaddr_in6 Socket;
  static const sa_family_t addrFamily = AF_INET6;
  static const int addrStrLength = INET6_ADDRSTRLEN; // <netinet/in.h>

  static bool getAddress (const char* addrStr, Address& result)
  {
    int flag = inet_pton (addrFamily, addrStr, &result);
    assert (flag != -1); /* would mean unsupported adress family */
    return (flag == 1); /* flag == 0 by failure */
  }

  static string toString (const Address& inetAddr)
  {
    char buf[addrStrLength];
    const char* result = inet_ntop (addrFamily, &inetAddr, buf, addrStrLength);
    if (result == NULL) {
      std::cerr << "WARNIG: 'toString' got invalid adress as argument!"
		<< std::endl;
      return "";
    }

    return result;
  }

#if 0
  // not fully implemented; please check the other fields of 'Socket'
  static Socket getSocket (const Address& inetAddr, const Port& port)
  {
    Socket result;
    memset (&result, '\0', sizeof (result));
    result.sin6_family = addrFamily;
    result.sin6_port = htons (port.portNumber);
    //result.sin6_addr.s6_addr = /* htonl */ (inetAddr.s6_addr);
    result.sin6_addr = inetAddr;

    return result;
  }
#endif
}; /* class AF<AF_INET6> */
#endif



template <const sa_family_t addrFamily = AF_INET>
class InetHost
{
public:
  typedef typename AF<addrFamily>::Address Address;

  string hostName;
  Address address;
  vector<string> aliases;
  vector<Address> addresses;

private:
  void init (struct hostent* aHost)
  {
    assert (aHost != NULL);
    assert (aHost->h_addrtype == addrFamily);
    assert (aHost->h_length == sizeof (Address));
    hostName = aHost->h_name;

    assert (aliases.empty ());
    for (int i = 0; aHost->h_aliases[i] != NULL; ++i) {
      aliases.push_back (aHost->h_aliases[i]);
    }

    assert (aHost->h_addr_list[0] != NULL);
    memcpy (&address, aHost->h_addr_list[0], aHost->h_length);

    assert (addresses.empty ());
    int i;
    for (/* int */ i = 1; aHost->h_addr_list[i] != NULL; ++i) {
      Address addr;
      memcpy (&addr, aHost->h_addr_list[0], aHost->h_length);
      addresses.push_back (addr);
    }
  }

public:
  InetHost (const char* aHostName)
  {
    assert (aHostName != NULL);
    struct hostent* aHost = gethostbyname (aHostName);

    if (aHost == NULL) {
      std::cerr << "WARNING: invalid host name '" << aHostName
		<< "'!" << std::endl;
    } else {
      init (aHost);
    }
  }

  InetHost ()
  {
    const char* aHostName = getStandardHostname ().c_str ();
    assert ( (aHostName != NULL)
	     && (! string (aHostName).empty ()) );
    struct hostent* aHost = gethostbyname (aHostName);
    assert (aHost != NULL);
    init (aHost);
  }

  InetHost (const Address& inetAddr)
  {
    struct hostent* aHost
      = gethostbyaddr ( (const char*) &inetAddr,
			sizeof (inetAddr),
			addrFamily );

    if (aHost == NULL) {
      std::cerr << "WARNING: invalid or unknown host adress '"
		<< AF<addrFamily>::toString (inetAddr)
		<< "'!" << std::endl;
    } else {
      init (aHost);
    }
  }

#if 0 /* commented out */
  InetHost () // provides the local host
  {
    hostName = "localhost";
    address.s_addr = INADDR_ANY; // constant for the local host
  }
#endif /* commented out */
}; // class InetHost<.>



template <const sa_family_t addrFamily = AF_INET>
class InetSockAddr {
public:
  const InetHost<addrFamily> host;
  const Port port;

  InetSockAddr (const InetHost<addrFamily>& aHost, const Port& aPort)
    : host (aHost), port (aPort)
  {}

  InetSockAddr (const Port& aPort) // takes the local host
    : host (), port (aPort)
  {}
}; // class InetSockAddr<.>



typedef int SocketFD;
#define NO_SOCKET_FD -1
#if (NO_SOCKET_FD >= 0)
#error "NO_SOCKET_FD must be < 0!"
#endif


class ReadWriteSocket
{
private:
  SocketFD sockfd;

public:
  ReadWriteSocket ();
  ReadWriteSocket (SocketFD aSocketFD);

public:
  SocketFD getSocketFD () const;

  bool open (sa_family_t addrFamily);

  ssize_t read (char* buf, size_t buflen);

  ssize_t write (const char* buf, size_t buflen);

  bool isOpen () const;

  bool close ();

  static void debugStatus ();
}; // class ReadWriteSocket



template <const sa_family_t addrFam = AF_INET>
class ServerSocket
{
private:
  ReadWriteSocket rwSocket;
  vector<MainPtr<ReadWriteSocket>* > clientSockets;

public:
  static const sa_family_t addrFamily = addrFam;

  const InetSockAddr<addrFamily> socketAddress;

  bool isOpen ()
  {
    return rwSocket.isOpen ();
  }

  /* creates a remote server with the given socket address */
  ServerSocket (const InetSockAddr<addrFamily>& aSockAddr)
    : socketAddress (aSockAddr)
  {
    assert (! isOpen ());
  }

  /* creates a remote server with the given address and port */
  ServerSocket (const InetHost<addrFamily>& aHost, const Port& aPort)
    : socketAddress (aHost, aPort)
  {
    assert (! isOpen ());
  }

  /* creates a local server using the given port */
  ServerSocket (const Port& aPort)
    : socketAddress (aPort)
  {
    assert (! isOpen ());
  }

  bool open ()
  {
    return rwSocket.open (addrFamily);
  }

#ifndef FORCE_SOCKET_REUSE
#define FORCE_SOCKET_REUSE 0
#endif
#if FORCE_SOCKET_REUSE
  enum BindMode {NO_FORCE, FORCE};
  bool bind (BindMode bindMode = FORCE)
#else
    bool bind ()
#endif
  {
    assert (isOpen ());

    typename AF<addrFamily>::Socket socket
      = AF<addrFamily>::getSocket (socketAddress.host.address,
				   socketAddress.port);

    assert (sizeof (sockaddr) == sizeof (typename AF<addrFamily>::Socket));
    int flag = ::bind ( rwSocket.getSocketFD (),
			reinterpret_cast<sockaddr*> (&socket),
			sizeof (socket) );

#if FORCE_SOCKET_REUSE
    std::cerr << "FORCE_SOCKET_REUSE active..." << std::endl;
    if ( (flag < 0) // an internal error occured
	 && (bindMode == FORCE) ) {
#if ANT_HAS_WIN_ENV && (! defined __CYGWIN__)
      ReadWriteSocket::debugStatus ();
#else
      if ( (errno != EINVAL)
	   && (errno != EADDRINUSE) ) {
	ReadWriteSocket::debugStatus ();

	// return false;
      }
#endif

#if ANT_HAS_WIN_ENV && (! defined __CYGWIN__)
      char oldFlag = '\0';
      char reuseFlag = '\0';
#else
      int oldFlag = -1;
      int reuseFlag = -1;
#endif
      socklen_t flagLen = sizeof (oldFlag);
      int status = getsockopt ( rwSocket.getSocketFD (),
				SOL_SOCKET, SO_REUSEADDR,
				&oldFlag, &flagLen );

      if (status != 0) {
	assert (status == -1);
	std::cerr << "bind(): invalid getsockopt status "
		  << status << std::endl;

	// return false;
      } else {
	std::cerr << "bind(): flag SO_REUSEADDR was "
		  << oldFlag << std::endl;
      }

      reuseFlag = 1; // enable the SO_REUSEADDR flag
      status = setsockopt ( rwSocket.getSocketFD (),
			    SOL_SOCKET, SO_REUSEADDR,
			    &reuseFlag, flagLen );
      if (status != 0) {
	assert (status == -1);
	std::cerr << "bind(): invalid setsockopt status "
		  << status << std::endl;

	// return false;
      }

      // try to rebind to the port
      std::cerr << "Trying to rebind to socket "
		<< rwSocket.getSocketFD () << std::endl;
      flag = ::bind ( rwSocket.getSocketFD (),
		      reinterpret_cast<sockaddr*> (&socket),
		      sizeof (socket) );
    }
#endif

    return (flag >= 0); // otherwise an internal error occured
  }

  bool listen ()
  {
    assert (isOpen ());
    int flag = ::listen (rwSocket.getSocketFD (), 5);
    return (flag == 0); // (flag == -1) on failure
  }

  MainPtr<ReadWriteSocket>::SubPtr
  accept (typename AF<addrFamily>::Socket* clientSocket)
  {
    assert (isOpen ());

    socklen_t clientSize = sizeof (*clientSocket);
    SocketFD result = ::accept ( rwSocket.getSocketFD (),
				 reinterpret_cast<sockaddr*> (clientSocket),
				 &clientSize );
    if (result < 0) {
      std::cerr << "'ServerSocket': 'accept' method failed!"
		<< std::endl;
      throw &ServerSocket::accept;
    } else {
      ReadWriteSocket* rws = new ReadWriteSocket (result);
      MainPtr<ReadWriteSocket>* clientRWSocket
	= new MainPtr<ReadWriteSocket> (rws);
      clientSockets.push_back (clientRWSocket);
      return clientRWSocket->newSubPtr ();
    }
  }

  bool close ()
  {
    bool result = rwSocket.close ();

    while (! clientSockets.empty ()) {
      MainPtr<ReadWriteSocket>* clientRWSocket
	= clientSockets.back ();
      clientSockets.pop_back ();

      ReadWriteSocket* rws = clientRWSocket->getPtr ();
      bool closeStatus = rws->close ();
      /*: we make an explicit call to 'close'... */
      assert (closeStatus);
      delete rws;
      delete clientRWSocket;
    }

    return result;
  }

  ~ServerSocket ()
  {
    close ();
  }
}; // class ServerSocket



template <const sa_family_t addrFam = AF_INET>
class ClientSocket
{
private:
  MainPtr<ReadWriteSocket> rwSocket;

public:
  static const sa_family_t addrFamily = addrFam;

  const InetSockAddr<addrFamily> serverSocketAddress;

  ClientSocket (const InetSockAddr<addrFamily>& serverSockAddr)
    : rwSocket (new ReadWriteSocket ()),
      serverSocketAddress (serverSockAddr)
  {}

  ClientSocket (const InetHost<addrFamily>& aHost, const Port& aPort)
    : rwSocket (new ReadWriteSocket ()),
      serverSocketAddress (aHost, aPort)
  {}

  ClientSocket (const Port& aPort)
    : rwSocket (new ReadWriteSocket ()),
      serverSocketAddress (aPort)
  {}

  bool open ()
  {
    return rwSocket->open (addrFamily);
  }

  bool isOpen () const
  {
    return rwSocket->isOpen ();
  }

  MainPtr<ReadWriteSocket>::SubPtr connect ()
  {
    assert (isOpen ());
    if (rwSocket.getCount () > 1) {
      std::cerr << "'ClientSocket::connect': already connected!"
		<< std::endl;
      std::cerr << "Please connect to a new 'ClientSocket'!"
		<< std::endl;
      throw &ClientSocket::connect;
    }

    typename AF<addrFamily>::Socket socket
      = AF<addrFamily>::getSocket (serverSocketAddress.host.address,
				   serverSocketAddress.port);

    int flag = ::connect ( rwSocket->getSocketFD (),
			   reinterpret_cast<sockaddr*> (&socket),
			   sizeof (socket) );

    if (flag < 0) { // an internal error occured
      std::cerr << "'ClientSocket': 'connect' method failed!"
		<< std::endl;
      ReadWriteSocket::debugStatus ();
      throw &ClientSocket::connect;
    }

    return rwSocket.newSubPtr ();
  }

  bool close ()
  {
    return rwSocket->close ();
  }

  ~ClientSocket ()
  {
    close ();
    delete rwSocket.getPtr ();
  }
};

// #undef NDBUG

#endif

