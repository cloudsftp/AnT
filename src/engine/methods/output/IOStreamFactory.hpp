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
 * $Id: IOStreamFactory.hpp,v 1.7 2004/09/02 11:13:55 wackengg Exp $
 *
 */

#ifndef IOSTREAM_FACTORY_HPP
#define IOSTREAM_FACTORY_HPP

#include "utils/GlobalConstants.hpp"

using std::ostream;

#include <time.h>
void printTimeOfDay (ostream* f);

class ScanData;

/**
 * IOStreamFactory is the interface for writing data that is generated
 * in AnT-methods
 */
class IOStreamFactory
{
public:
  /**
   * open a new (logical) output file
   *
   * @param fileName the filename associated with the data of this stream
   * 
   * @param scanData_ptr the second parameter is optional. If it is given,
   * the description of the dynSysData will be written to the file header (VA).
   * @warning do not delete the default value! Maybe in some cases the 
   * header has not to be written!
   * 
   * @returns stream where output can be written to
   */
  virtual ostream *getOStream ( const char *fileName,
				ScanData* scanData_ptr = NULL ) = 0;
  
  /**
   * similar to 'getOStream (const char *fileName)',
   * but using 'string' instead of 'char*'
   */
  // inline 
  ostream *getOStream ( const string& fileName,
			ScanData* scanData_ptr = NULL );

  /**
   * Force flushing of the data that has been put into all ostreams
   * that were generated by getOStream since the last call of commit().
   * (This will only have an effect in the networked variant)
   */
  virtual void commit () = 0;

  /**
   * Closes and destroys the given stream. The stream must have been created 
   * using getOStream. Is only needed to be called if one wants to close a 
   * stream explicitly. Normally the destructor handles open streams.
   * @param stream the stream to be closed
   */
  virtual void closeOStream (ostream* stream) = 0; 
  
  /**
   * destructor
   * closes and destroys all open streams
   */
  virtual ~IOStreamFactory ();

public:
    /**
     * set the output format and the output precision
     * for streams, which will be created in sub--classes.
     *
     * @note instead of 'ostream::scientific' one can use
     * 'ostream::fixed'.
     * @warning Don't forget the bitmask 'ostream::floatfield'!
     */
  static void setPrecision (ostream* f);
  
  static void printHeader (ostream* stream,
			   ScanData* scanData_ptr);
};

//
extern IOStreamFactory* ioStreamFactory;

#endif