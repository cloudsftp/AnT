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
 * $Id: LocalIOStreamFactory.hpp,v 1.3 2004/03/12 12:51:40 vravrout Exp $
 *
 */

#ifndef LOCAL_IOSTREAM_FACTORY_HPP
#define LOCAL_IOSTREAM_FACTORY_HPP

#include <fstream>
#include <list>

#include "IOStreamFactory.hpp"

/**
 * LocalIOStreamFactory is the implementation of IOStreamFactory for
 * local operation of AnT. It provides an abstraction for opening and
 * writing data to local files from within methods.
 */
class LocalIOStreamFactory : public IOStreamFactory
{
private:
  std::list<std::ofstream*> openStreams;

public:
  /**
   * open a local file for writing
   * @param fileName the name of the file that should be opened
   * @param scanData_ptr see it the base class description! 
   * @returns stream where output can be written to
   */
  ostream *getOStream (const char* fileName,
		       ScanData* scanData_ptr = NULL);

  /**
   * has no effect in the local variant
   */
  void commit ();

  /**
   * Closes and destroys the given stream. The stream must have been created 
   * using getOStream. Is only needed to be called if one wants to close a 
   * stream explicitly. Normally the destructor handles open streams.
   * @param stream the stream to be closed
   */
  void closeOStream (ostream* stream); 

  /**
   * destructor (closes open files)
   */
  virtual ~LocalIOStreamFactory ();
};

#endif
