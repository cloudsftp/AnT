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
 * $Id: symbolicimageoutputwriter.cpp,v 1.6 2005/10/05 20:00:53 fundindy Exp $
 *
 */


#include "symbolicimageoutputwriter.hpp"

SymbolicImageOutputWriter::SymbolicImageOutputWriter(){

  m_bWriteEverySubdivision = false;
  fout = NULL;
  scanData = NULL;
}

SymbolicImageOutputWriter::~SymbolicImageOutputWriter(){

  // close all output streams
  /*
  if((fout) && (!m_bWriteEverySubdivision)){
        ioStreamFactory->closeOStream (fout);
  }
  else if(m_bWriteEverySubdivision) {
       cout << "Checl str" << endl;
       for(int i=0;i<m_pSubdivisionOutStreams.getTotalSize();i++){
           if(m_pSubdivisionOutStreams[i])
             ioStreamFactory->closeOStream(m_pSubdivisionOutStreams[i]);
       }
  }
  */
}

bool SymbolicImageOutputWriter::initOutputFile(string filename, string ext){

  if((fout) || (m_bWriteEverySubdivision)){
    cout << "Outputwriter can not be initialised for " << filename
    << ". Writer is already initialised." << endl;

    return false;
  }
  else{

    // get a new outputstream from the stream factory
     string file = filename + "." + ext;
     fout = ioStreamFactory->getOStream (file.c_str());
     return true;
  }
}

bool SymbolicImageOutputWriter::initSubdivisionOutputFiles(string filename,string ext, Array< bool > &writeInSubdivisionStep){

  if((fout) || (m_bWriteEverySubdivision)){
      cout << "Outputwriter can not be initialised for " << filename
      << ". Writer is already initialised." << endl;

      return false;
    }
    else{

      // create all output streams put them into the array
        m_bWriteEverySubdivision = true;
        m_pSubdivisionOutStreams.alloc(writeInSubdivisionStep.getTotalSize());
        for(int i=0; i < writeInSubdivisionStep.getTotalSize();i++){
          if(writeInSubdivisionStep[i]){
            string file = filename + "_" + toString(i) + "." + ext;
            m_pSubdivisionOutStreams[i] = ioStreamFactory->getOStream(file.c_str());
          }
          else{
            m_pSubdivisionOutStreams[i] = NULL;
          }
        }

        return true;
    }
}

bool SymbolicImageOutputWriter::writesForEverySubdivision(){

  return m_bWriteEverySubdivision;
}

void SymbolicImageOutputWriter::writeSubdivisionOutput(int subdivisionDepth){

  // if this is a subdivision output writer always set fout to the subdivision phase-specific
  // output stream.
  if(m_bWriteEverySubdivision){
      fout = m_pSubdivisionOutStreams[subdivisionDepth];
      if(m_pSubdivisionOutStreams[subdivisionDepth]){
        writeOutput();
      }
  }
}

void SymbolicImageOutputWriter::writeFinalOutput(){

  if(!m_bWriteEverySubdivision){
    writeOutput();
  }
}

bool SymbolicImageOutputWriter::parseOutputFileInformation(Configuration& fileDescription, int numberOfSubdivisions){

  if(!(fileDescription.checkForKey("FILENAME")) ){
     cout << "No filename specified for writer." << endl;
     return false;
   }

  string filename = fileDescription.getString("FILENAME");
  string ext = fileDescription.getString("FILE_EXTENSION");

  
  if( fileDescription.checkForKey("ONLY_SUBDIVISIONS") || ((fileDescription.checkForKey("EVERY_SUBDIVISION")) && (fileDescription.getBool("EVERY_SUBDIVISION"))) ){

    Array< bool > writeInSubdivisionStep;

    Array< long > subs;
    if(fileDescription.checkForKey("ONLY_SUBDIVISIONS") && fileDescription.isArrayAssignable("ONLY_SUBDIVISIONS",subs)){

      Array< long > subs;
      fileDescription.getArray("ONLY_SUBDIVISIONS", subs);

      writeInSubdivisionStep.alloc(numberOfSubdivisions+1,false);
      
      for(int i=0; i< subs.getTotalSize(); i++){
       int step = subs[i];
       if( (step >= 0) && ( step < writeInSubdivisionStep.getTotalSize()) ){
         writeInSubdivisionStep[step] = true;
       }
      }
    }
    else writeInSubdivisionStep.alloc(numberOfSubdivisions+1,true);

    return initSubdivisionOutputFiles(filename,ext,writeInSubdivisionStep);
  }
  else{
    return initOutputFile(filename,ext);
  }
}

void SymbolicImageOutputWriter::setScanData(ScanData & sData){

    scanData = &sData;
}
