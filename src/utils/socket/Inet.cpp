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
 * $Id: Inet.cpp,v 1.7 2004/05/04 15:16:28 rtlammer Exp $
 *
 */

#include "Inet.hpp"

#ifndef DEBUG__INET_CPP
#define DEBUG__INET_CPP 0
#endif

#if DEBUG__INET_CPP || (ANT_HAS_WIN_ENV && (! defined __CYGWIN__))
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#endif

#include <cstdio>


#if ANT_HAS_WIN_ENV && (! defined __CYGWIN__)
WSADATA winsockStartupData;
bool winsockStartupFlag =
# if DEBUG__INET_CPP
  (
# endif
     (WSAStartup (MAKEWORD(1,1), &winsockStartupData) == 0) /*: version 1.1 and higher */
# if DEBUG__INET_CPP
      && (cout << "ReadWriteSocket::open: winsock startup ok!" << endl, true)
  ) || (cerr << "ReadWriteSocket::open: winsock startup failure!" << endl, false)
# endif
;
#endif

string getStandardHostname ()
{
#define MAXHOSTNAME 255
  char result[MAXHOSTNAME+1];
  int hostnameFlag = -1;
  hostnameFlag = gethostname (result, MAXHOSTNAME);
  assert (hostnameFlag == 0);

  return result;
}

// static
Port::Number Port::toNumber (const string& portNrStr)
{
  return toNumber (portNrStr.c_str ());
}

// static
Port::Number Port::toNumber (const char* chars)
{
  //    int i = atoi (chars);
  //    Port::Number portNr = static_cast<Port::Number> (i);
  //    if (i != portNr) {
  //      std::cerr << "WARNING: wrong port number '"
  //  	      << chars << "'!" << std::endl;
  //      return 0;
  //    }
  //    return portNr;

  Port::Number portNr = 0;
  int status = sscanf (chars, "%d", &portNr);
  if (status != 1) {
    std::cerr << "WARNING: wrong port number '"
	      << chars << "'!" << std::endl;
    return 0;
  }
  return portNr;
}

Port::Port (Port::Number portNr)
  : portNumber (portNr)
{}

Port::Port (const string& portNrStr)
  : portNumber (Port::toNumber (portNrStr))
{}

Port::Port (const char* chars)
  : portNumber (Port::toNumber (chars))
{}

/* ********************************************************************** */

// static
void ReadWriteSocket::debugStatus ()
{
#if ANT_HAS_WIN_ENV && ( ! defined __CYGWIN__)
  std::cerr << "ReadWriteSocket: unknown error occured!"
	    << std::endl;
#else
  std::cerr << "errno = " << errno << std::endl
	    << std::endl;

  std::cerr << "ETIMEDOUT = " << ETIMEDOUT << std::endl;
  std::cerr << "EWOULDBLOCK = " << EWOULDBLOCK << std::endl;
  std::cerr << "EALREADY = " << EALREADY << std::endl;
  std::cerr << "EINPROGRESS = " << EINPROGRESS << std::endl;
  std::cerr << "EADDRINUSE = " << EADDRINUSE << std::endl;
  std::cerr << "ECONNREFUSED = " << ECONNREFUSED << std::endl;
  std::cerr << "EISCONN = " << EISCONN << std::endl;
  std::cerr << "EINVAL = " << EINVAL << std::endl;
#endif
}

SocketFD ReadWriteSocket::getSocketFD () const
{
  return sockfd;
}

bool ReadWriteSocket::isOpen () const
{
  return (sockfd >= 0);
}

ReadWriteSocket::ReadWriteSocket ()
  : sockfd (NO_SOCKET_FD)
{
  assert (! isOpen ());
}


ReadWriteSocket::ReadWriteSocket (SocketFD aSocketFD)
  : sockfd (aSocketFD)
{}

ssize_t ReadWriteSocket::read (char* buf, size_t buflen)
{
  if (isOpen ()) {
#if ANT_HAS_WIN_ENV && ( ! defined __CYGWIN__)
    return recv (sockfd, buf, buflen, 0);
    /*: the last argument (flags) is 0, this should do as seen in
      other programs... */
#else
    return ::read (sockfd, buf, buflen);
#endif
  } else {
    throw &ReadWriteSocket::read;
    // return 0;
  }
}

ssize_t ReadWriteSocket::write (const char* buf, size_t buflen)
{
  if (isOpen ()) {
#if ANT_HAS_WIN_ENV && ( ! defined __CYGWIN__)
    return send (sockfd, buf, buflen, 0);
    /*: the last argument (flags) is 0, this should do as seen in
      other programs... */
#else
    return ::write (sockfd, buf, buflen);
#endif
  } else {
    throw &ReadWriteSocket::write;
    // return 0;
  }
}

bool ReadWriteSocket::open (sa_family_t addrFamily)
{
  assert (! isOpen ());

  sockfd = ::socket (addrFamily, SOCK_STREAM, 0);
#if DEBUG__INET_CPP
  std::cout << "'ReadWriteSocket::open': sockfd = "
       << sockfd << std::endl;
#endif

  if (sockfd < 0) {
    std::cerr << "'ReadWriteSocket': socket descriptor initialization failed!"
	      << std::endl;
    sockfd = NO_SOCKET_FD;
  }

  return isOpen ();
}

bool ReadWriteSocket::close ()
{
  if (isOpen ()) {
#if ANT_HAS_WIN_ENV && ( ! defined __CYGWIN__)
    int status = closesocket (sockfd);
#else
    int status = ::close (sockfd);
#endif
#if DEBUG__INET_CPP
    std::cout << "'ReadWriteSocket::close': sockfd = "
	      << sockfd << std::endl;
#endif

    if (status == 0) { // 0 means success
      sockfd = NO_SOCKET_FD;
    }
  }

  return (! isOpen ());
}

