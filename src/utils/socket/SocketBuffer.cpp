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
 * $Id: SocketBuffer.cpp,v 1.5 2004/05/04 14:25:28 rtlammer Exp $
 *
 */

#include "SocketBuffer.hpp"

#include <cstddef>
#include <cstdio>

#if 0 /* commented out */
/* not needed anymore, because 'Inet.hpp' is included in
   'SocketBuffer.hpp': */
#include <sys/types.h>
// #include <sys/uio.h>
#include <sys/socket.h>
#endif /* commented out */

#include <cassert>
#include <string>

#include <cstring>
using std::strncpy;

using std::cout;
using std::cerr;
using std::endl;



#ifndef DEBUG__SOCKET_BUFFER_CPP
#define DEBUG__SOCKET_BUFFER_CPP 0
#endif

#if GCC2_COMPILER
// static 
SocketBuffer::int_type 
SocketBuffer::traits_type::eof ()
{
  return EOF;
}

// static 
SocketBuffer::int_type 
SocketBuffer::traits_type::to_int_type
(const SocketBuffer::char_type& c)
{
  return c; // implicit conversion
}

// static 
bool SocketBuffer::traits_type::eq_int_type
( const SocketBuffer::int_type& c1,
  const SocketBuffer::int_type& c2 )
{
  return (c1 == c2);
}
#endif // GCC2_COMPILER



// public
void SocketBuffer::close ()
{
  int syncStatus = sync ();
  assert (syncStatus == 0);
#if DEBUG__SOCKET_BUFFER_CPP
  cout << "SocketBuffer::close..." << endl;
#endif
  delete rwSocket;
  rwSocket = NULL;
}

// public
bool SocketBuffer::isOpen () const
{
  return (rwSocket != NULL)
    && rwSocket->isValid ()
    && (*rwSocket)->isOpen ();
}

// public
bool SocketBuffer::reopen
(const MainPtr<ReadWriteSocket>::SubPtr& aRWSocket)
{
  if (rwSocket != NULL)
    close ();

  rwSocket = new MainPtr<ReadWriteSocket>::SubPtr (aRWSocket);
  // invoking the copy constructor

  return isOpen ();
}

// public
SocketBuffer::SocketBuffer
(const MainPtr<ReadWriteSocket>::SubPtr& aRWSocket)
  : rwSocket (NULL)
{
  bool openStatus = reopen (aRWSocket);
  assert (openStatus);

  setg (gBuf, gBuf, gBuf); // empty input buffer
  /* causes an 'underflow' on the next call and makes calls to
     'sungetc' invalid */

  setp (pBuf, pBuf + pBufSize);
}

// public
SocketBuffer::~SocketBuffer ()
{
  close ();
}


// private
streamsize SocketBuffer::send
(const SocketBuffer::char_type* aPtr, streamsize n)
{
  assert (n > 0);
  if (isOpen ()) {
#if DEBUG__SOCKET_BUFFER_CPP
    cout << "SocketBuffer::send() --> "
	 << "sending the " << n << " characters \""
	 << std::string (aPtr, n)
	 << "\" to the other peer..."
	 << endl;
#endif

    return (*rwSocket)->write (aPtr, n);
  }
  // throw &SocketBuffer::send;
  return -1;
}

// virtual
int SocketBuffer::sync ()
{
  assert (pptr() >= pbase());
  assert (pptr() <= epptr());
  assert (epptr() == pBuf + pBufSize);
  assert ((epptr() - pbase()) <= pBufSize);

  streamsize n = pptr() - pbase(); // n >= 0; see assert above
  while (n > 0) {
    streamsize howMany = send (pbase(), n);

    if (howMany <= 0) {
      cerr << "WARNING 'SocketBuffer::sync': sending " 
	   << howMany << " out of "
	   << n << " characters!" << endl;
      return -1; // 'sync' error status
    }

    n -= howMany;
    setp (pbase() + howMany, epptr());
  } // while

  if (pbase() == epptr()) {
#if DEBUG__SOCKET_BUFFER_CPP
    cout << "'SocketBuffer::sync': enlarging empty put buffer..."
	 << endl;
#endif

    setp (pBuf, pBuf + pBufSize);
  }

  assert (pptr() == pbase());
  return 0; // 'sync' successfully executed
}

// private
streamsize SocketBuffer::receive
(SocketBuffer::char_type* aPtr, streamsize n)
{
  assert (n > 0);
  if (isOpen ()) {
    streamsize howMany = (*rwSocket)->read (aPtr, n);

#if DEBUG__SOCKET_BUFFER_CPP
    cout << "SocketBuffer::receive() --> receiving the "
	 << howMany << " characters \"";
    cout.write (aPtr, howMany);
    cout << "\" from the other peer..." << endl;
#endif

    return howMany;
  }
  // throw &SocketBuffer::receive;
  return -1;
}

// virtual
SocketBuffer::int_type SocketBuffer::underflow ()
{
  assert (gptr() == egptr());
  assert (eback() == gBuf);
  assert ((egptr() - eback()) <= gBufSize);
//    if (! isOpen ()) {
//      return traits_type::eof ();
//    }

  if (egptr() < (eback() + gBufSize)) {
    streamsize howMany = receive (egptr(), (eback() + gBufSize) - egptr());
    if (howMany <= 0) {
      return traits_type::eof ();
    } else {
      setg (eback(), egptr(), egptr() + howMany);
      int_type result = traits_type::to_int_type (*gptr());
      assert (! traits_type::eq_int_type (result, traits_type::eof ()));
      return result;
    }
  }

  assert (egptr() - eback() == gBufSize);
  streamsize howMany = receive (eback(), gBufSize);
  assert (howMany <= gBufSize);
  if (howMany > 0) {
    setg (eback(), eback(), eback() + howMany);
    assert (gptr() == eback());
    int_type result = traits_type::to_int_type (*gptr());
    assert (! traits_type::eq_int_type (result, traits_type::eof ()));
    return result;
  } else {
    return traits_type::eof ();
  }
}

// virtual
SocketBuffer::int_type SocketBuffer::overflow (SocketBuffer::int_type c)
{
#if DEBUG__SOCKET_BUFFER_CPP
  cout << "SocketBuffer::overflow..." << endl;
#endif
  assert (pptr() == epptr());

  int syncStatus = sync (); // send the characters to the other peer

  if (syncStatus != 0) {
    return traits_type::eof();
  }

  assert (! traits_type::eq_int_type (c, traits_type::eof ()));
  return sputc (c);
}

// virtual
streamsize SocketBuffer::xsputn
(const SocketBuffer::char_type* s, streamsize n)
{
#if DEBUG__SOCKET_BUFFER_CPP
  cout << "SocketBuffer::xsputn() --> "
       << "sending " << n
       << " characters to the other peer..."
       << endl;
#endif

  streamsize availPSeq = epptr() - pptr();
  if (n <= availPSeq) {
    strncpy (pptr(), s, n);
    pbump (n);
    return n;
  }

  strncpy (pptr(), s, availPSeq); // fill the put buffer
  pbump (availPSeq);
  assert (pptr() == epptr());

  streamsize rest = n - availPSeq;
  const SocketBuffer::char_type* sPtr = s + availPSeq;

  int syncStatus = sync (); // send and empty the whole pBuf to the other peer
  if (syncStatus != 0) {
    cerr << "WARNING 'SocketBuffer::xsputn': sending only " << (n - rest)
	 << " out of " << n << " characters!" << endl;
    return (n - rest);
  }

  assert ((epptr() - pbase()) > 0);
  assert (pptr() == pbase()); // due to 'sync'
  while (rest > 0) {
    assert (sPtr - s + rest == n); // the loop invariant

    streamsize howMany = send (sPtr, std::min((streamsize) pBufSize, rest));
    if (howMany <= 0) {
      /* some error occured while trying to send the characters */
      cerr << "WARNING 'SocketBuffer::xsputn': sending only " << (n - rest)
	   << " out of " << n << " characters!" << endl;
      return (n - rest);
    }

    sPtr += howMany;
    rest -= howMany;
  } // while

  assert (rest == 0);
  return n;
}

// virtual
streamsize SocketBuffer::xsgetn
(SocketBuffer::char_type* s, streamsize n)
{
#if DEBUG__SOCKET_BUFFER_CPP
  cout << "SocketBuffer::xsgetn() --> "
       << "receiving " << n
       << " characters from the other peer..."
       << endl;
#endif

  if (n <= 0) {
    cerr << "WARNING 'SocketBuffer::xsgetn': n < 0, n = "
	 << n << endl;
    return -1;
  }
  // assert (n > 0);

  SocketBuffer::char_type* sPtr = s;
  streamsize rest = n;

  streamsize availGSeq = egptr() - gptr();
  assert (availGSeq >= 0);
  if (availGSeq >= n) {
    strncpy (s, gptr(), n);
    gbump (n);
    return n;
  }

  if (availGSeq > 0) {
    strncpy (s, gptr(), availGSeq);

    sPtr += availGSeq;
    rest -= availGSeq;
  }
  setg (gBuf, gBuf, gBuf);
  /* causes an 'underflow' on the next call and makes calls to
     'sungetc' invalid */

  while (rest > 0) {
    assert (sPtr - s + rest == n); // the loop invariant

    streamsize howMany = receive (sPtr, std::min((streamsize) gBufSize, rest));
    if (howMany <= 0)  {
      /* some error occured while trying to read the characters */
      cerr << "WARNING 'SocketBuffer::xsgetn': receiving only " << (n - rest)
	   << " out of " << n << " characters!" << endl;
      assert (egptr() == eback());
      return (n - rest);
    }

    sPtr += howMany;
    rest -= howMany;
  } // while

  assert (rest == 0); // @see while condition above
  assert (egptr() == eback());
  return n;
}
