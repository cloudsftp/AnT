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
 * $Id: SocketBuffer.hpp,v 1.3 2004/03/24 13:26:16 wackengg Exp $
 *
 */

#ifndef SOCKET_BUFFER_HPP
#define	SOCKET_BUFFER_HPP

#include "Inet.hpp"

#include <iostream>
using std::streambuf;
using std::locale;
using std::ios;
using std::streamsize;

// socket buffer class
class SocketBuffer : public streambuf {
public:
#define gBufSize 1024
#define pBufSize 1024

#ifdef GCC2_COMPILER
// #error "GCC2_COMPILER already defined!"
#endif
#ifdef __GNUC__
#define GCC2_COMPILER (__GNUC__ < 3)
  /*: not fully standard compliant; see below */
#else
#define GCC2_COMPILER 0
#endif
#if GCC2_COMPILER
  typedef char char_type;
  typedef int int_type;

  class traits_type
    : public std::string::traits_type
  {
  public:
    static int_type eof ();
    static int_type to_int_type (const char_type& c);
    static bool eq_int_type ( const int_type& c1,
			      const int_type& c2 );
  };
#endif
#if 0
  typedef std::streampos pos_type;
  typedef std::streamoff off_type;
  typedef int seekdir;
#endif

private:
  char_type gBuf[gBufSize];
  char_type pBuf[pBufSize];

  MainPtr<ReadWriteSocket>::SubPtr* rwSocket;

  SocketBuffer (const SocketBuffer& other);
  // no copy constructor implementation!!!

  SocketBuffer& operator= (const SocketBuffer& other);
  // no assignment operator implementation!!!

public:
  SocketBuffer (const MainPtr<ReadWriteSocket>::SubPtr& aRWSocket);

  bool reopen (const MainPtr<ReadWriteSocket>::SubPtr& aRWSocket);

  bool isOpen () const;

  void close ();

  ~SocketBuffer ();

protected:
  // here we have the virtual methods of base class 'streambuf'

#if 0
  virtual void imbue (const locale &loc);

  virtual SocketBuffer* setbuf (char_type* s, int_type* n);
  /* covariant return value; in absence of covariance support this
    should be 'basic_streambuf<char_type, traits_type>*' */

  virtual pos_type seekoff (off_type off,
                            seekdir way,
			    ios::openmode which = ios::in | ios::out);

  virtual pos_type seekpos (pos_type sp,
			    ios::openmode which = ios::in | ios::out);
#endif

  virtual int sync ();

#if 0
  virtual int showmanyc ();
#endif

  /* the implementation in the base class 'streambuf' should do,
     but we should provide an optimized implementation... */
  /* default behavior: behaves like 'n' subsequent calls to 'sbumpc' */
  virtual streamsize xsgetn (char_type* s, streamsize n);

  virtual int_type underflow ();

#if 0
  /* the implementation in the base class 'streambuf' should do */
  /* called when 'snextc' -> 'sbumpc' fails */
  /* default behavior: calls 'underflow' and increments the next
     pointer */
  virtual int_type uflow ();

  /* if the putback-methods 'sputbackc' or 'sungetc' fail, then we
     have already filled the 'gBuf' with the next 'gBufSize'
     characters from the other socket peer. 'pbackfail' cannot (and
     should not) undo this step, so we stick with the default
     implementation, hence returning 'traits_type::eof()' */
  virtual int_type pbackfail (int_type c = traits_type::eof ());
#endif

  /* the implementation in the base class 'streambuf' should do,
     but we should provide an optimized implementation... */
  /* default behavior: behaves like 'n' subsequent calls to 'sputc' */
  virtual streamsize xsputn (const char_type* s, streamsize n);

  virtual int_type overflow (int_type c = traits_type::eof ());

private:
  streamsize receive (SocketBuffer::char_type* aPtr, streamsize n);

  streamsize send (const SocketBuffer::char_type* aPtr, streamsize n);
};

#endif	// SOCKET_BUFFER_HPP
