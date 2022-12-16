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
 * $Id: symbolicimageoutputwriter.hpp,v 1.6 2004/03/22 13:19:04 wackengg Exp $
 *
 */

#ifndef SYMBOLICIMAGEOUTPUTWRITER_HPP
#define SYMBOLICIMAGEOUTPUTWRITER_HPP

#include "../../data/ScanData.hpp"
#include "../../methods/output/IOStreamFactory.hpp"
// #include "../utils/strconv/StringConverter.hpp"

/**
    Abstract class for output writers. Cares about all file handling.
    Derived classes only have to call fout to write to the appropriate file.

  *@author Danny Fundinger
  */

class SymbolicImageOutputWriter {

public:

  /** Constructor. */
  SymbolicImageOutputWriter();

  /** Constructor. */
  virtual ~SymbolicImageOutputWriter();
  
  /**
      Writes the final output after the subdivision process has finished.
      Internally writeOutput() is called.

      \sa writeOuput() and writeSubdivisionOutput()
  */
  void writeFinalOutput();

  /**
      Writes the output for the given subdivision depth.
      Internally writeOutput() is called.

      \sa writeOutput() and writeFinalOutput()
  */
  void writeSubdivisionOutput(int subdivisionDepth);

 

  /**
    Returns true if this writer initialised for writing in every subdivision phase.

    \sa initOutputFile() and initSubdivisionOutputFile()
   */
  bool writesForEverySubdivision();
  
  /** Sets the scan data object. */
  void setScanData(ScanData & sData);
  
protected:

  /** The output stream. */
  ostream* fout;

  /** The scan data object */
  ScanData * scanData;

  /** The writeOutput() method. Has to be implemented by a subclass. */
  virtual void writeOutput () =0;

  /** The parse method for initialisation. */
  virtual bool parseOutputFileInformation(Configuration& fileDescription, int numberOfSubdivisions);

   /**
    Creates a new output file with the given name and extension (".tna").

    \sa writeSubdivisionOutputFile()
  */
  bool initOutputFile(string filename, string ext);

   /**
    Initialises output files for every subdivision using the specified
    name and adds the number of the subdivision and ".tna".

    \sa initOutputFile()

    @param writeInSubdivisionStep true, if the writer should write for the designated subdivision step.
  */
  bool initSubdivisionOutputFiles(string filename, string ext, Array< bool > &writeInSubdivisionStep );
  
private:

    /** Subdivision output streams. Only needed if writer writes every subdivision. */
   Array< ostream* > m_pSubdivisionOutStreams;

   /** Does writer write in every subdivision phase? */
   bool m_bWriteEverySubdivision;

};

#endif
