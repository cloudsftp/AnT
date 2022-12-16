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
 * $Id: symbolicimagemorsespectrumlayer.cpp,v 1.8 2004/05/17 12:55:12 schanzml Exp $
 *
 */
 
#include "symbolicimagemorsespectrumlayer.hpp"
#include <climits>

const char * SymbolicImageMorseSpectrumLayer::key = "MORSE_SPECTRUM";
const char*  SymbolicImageMorseSpectrumLayer::MorseSpectrumOutputWriter::key = "MORSE_SPECTRUM_WRITER";

SymbolicImageMorseSpectrumLayer::MorseSpectrumOutputWriter::MorseSpectrumOutputWriter(SymbolicImageMorseSpectrumLayer* morseLayer):
        SymbolicImageOutputWriter(){

        m_pMorseLayer = morseLayer;
        m_bWriteInterpolated = m_bWriteMin = m_bWriteMax =
        m_bWriteInvariantSet = m_bWritePeriodSize = false;
}

SymbolicImageMorseSpectrumLayer::MorseSpectrumOutputWriter::~MorseSpectrumOutputWriter(){
}

bool SymbolicImageMorseSpectrumLayer::MorseSpectrumOutputWriter::parse(Configuration & morseSpectrumWriterDescription,
                                                                                      int numberOfSubdivisions){

  //string typeStr = morseSpectrumWriterDescription.getString("TYPE_KEY");

  if(!(morseSpectrumWriterDescription.checkForEnumValue("TYPE_KEY",SymbolicImageMorseSpectrumLayer::MorseSpectrumOutputWriter::key)) ){

      cout << "Unknown output writer type description." << endl;
      return false;
  }

  if(morseSpectrumWriterDescription.checkForKey("MORSE_MAX_VALUES") &&
     morseSpectrumWriterDescription.getBool("MORSE_MAX_VALUES")){

     m_bWriteMax = true;
  }

  if(morseSpectrumWriterDescription.checkForKey("MORSE_MIN_VALUES") &&
     morseSpectrumWriterDescription.getBool("MORSE_MIN_VALUES")){

     m_bWriteMin = true;
  }

  if(morseSpectrumWriterDescription.checkForKey("MORSE_INTERPOLATED") &&
      morseSpectrumWriterDescription.getBool("MORSE_INTERPOLATED")){
    m_bWriteInterpolated = true;
  }

   if(  (morseSpectrumWriterDescription.checkForKey("INVARIANT_SET")) && (morseSpectrumWriterDescription.getBool("INVARIANT_SET")) ){
      m_bWriteInvariantSet = true;
  }

  if( (morseSpectrumWriterDescription.checkForKey("PERIOD_SIZE")) && (morseSpectrumWriterDescription.getBool("PERIOD_SIZE")) ){
    m_bWritePeriodSize = true;
  }

  if( !(m_bWriteMax || m_bWriteMin || m_bWriteInterpolated)){

    cout << "Neither min, max nor interpolated values are written with this morse spectrum writer. "
     << "Activate at least one of them." << endl;

     return false;
  }
  return parseOutputFileInformation(morseSpectrumWriterDescription,numberOfSubdivisions);
}

void SymbolicImageMorseSpectrumLayer::MorseSpectrumOutputWriter::writeOutput(){

  if(!m_pMorseLayer->isLinearExtensionActive()){
   (*fout) << "Linear extension not activated for this subdivision step." << endl;
   return;
  }

  SymbolicImageRecurrentCellSet::RecurrentCellSetEnumerator*  pRecSet = m_pMorseLayer->getRecurrentSetList();
  
  while(pRecSet->hasMoreElements()){

    MorseSpectrumRecurrentCellSet* set = DOWN_CAST< MorseSpectrumRecurrentCellSet* > (pRecSet->getNextElement());
    
    bool m_bScanWritten = false;

    // write the nonstationary exponents of the recurrent cell sets
    MorseSpectrumRecurrentCellSet::NonstationaryExponent* nonstationaryExponent = set->getNonstationaryExponent();

    // Write min, max and/or interpolated values.
    // Because the values can be out of range this
    // must be checked first. Only write the scan data if at least one value is properly defined !!

    //numeric_limits<real_t> real_t_info;
    
    if(m_bWriteMin){
      if( (!nonstationaryExponent) ||
          (nonstationaryExponent->min == DBL_MAX) ||
          (nonstationaryExponent->min == -DBL_MAX)){
      //  (*fout) << (*scanData) << " " << "0.0" << endl;
      }
      else{
         (*fout) << (*scanData) << " " << nonstationaryExponent->min;
         m_bScanWritten = true;
      }
    }
    if(m_bWriteMax){
      if( (!nonstationaryExponent) ||
          (nonstationaryExponent->max == DBL_MAX) ||
          (nonstationaryExponent->max == -DBL_MAX)){
      //  (*fout) << (*scanData) << " " << "0.0" << endl;
      }
      else{
         if(!m_bScanWritten){
            (*fout) << (*scanData);
            m_bScanWritten = true;
         }
         
         (*fout) << " " << nonstationaryExponent->max ;
      }
    }

    if(m_bWriteInterpolated){
      if(   (!nonstationaryExponent)  ||
            (nonstationaryExponent->min == DBL_MAX) ||
            (nonstationaryExponent->min == -DBL_MAX) ||
            (nonstationaryExponent->max == DBL_MAX) ||
            (nonstationaryExponent->max == -DBL_MAX) ){

      }
      else{
         if( !m_bScanWritten ){
           (*fout) << (*scanData);
           m_bScanWritten = true;
          };
          real_t inter = nonstationaryExponent->min +
            ((nonstationaryExponent->max - nonstationaryExponent->min) *0.5);

          (*fout) << " " << inter;
      }
    }

    if((m_bWriteInvariantSet) && (m_bScanWritten))  (*fout) << " I" << set->getSetNumber();
    if( m_bWritePeriodSize && m_bScanWritten) (*fout) << " P" << nonstationaryExponent->period;
  
    if(m_bScanWritten) (*fout) << endl;
  }
  delete pRecSet;
}

SymbolicImageMorseSpectrumLayer::SymbolicImageMorseSpectrumLayer(int numberOfSubdivisions):SymbolicImageDefaultLayer(numberOfSubdivisions){

  m_pLinearExtendedBoxManager = NULL;
}

SymbolicImageMorseSpectrumLayer::~SymbolicImageMorseSpectrumLayer(){

  m_pLinearExtendedBoxManager = NULL;
}


bool SymbolicImageMorseSpectrumLayer::init(SymbolicImageAbstractBoxManager * boxManager){

  SymbolicImageAbstractLinearExtendedBoxManager* extendedBoxManager =
          dynamic_cast< SymbolicImageAbstractLinearExtendedBoxManager* >(boxManager);
          
  if(!extendedBoxManager){

    cerr << "MorseSpectrumLayer can not be initialised with an abstract box manager."
    << " Use linear extended box manager object." << endl;

    return false;
  }
  else return initExtended(extendedBoxManager);
}

bool SymbolicImageMorseSpectrumLayer::initExtended(SymbolicImageAbstractLinearExtendedBoxManager* extendedBoxManager){

  // the morse spectrum specific object are instantiated.
  
  m_pLinearExtendedBoxManager = extendedBoxManager;

  // a morse spectrum cell set creator
  if(!m_pCellSetCreator) m_pCellSetCreator = new MorseSpectrumRecurrentCellSetCreator(m_pLinearExtendedBoxManager);
  else{
      cout << "CellSetCreator already initialised while calling init of SymbolicImageMorseSepctrumLayer!!"
      << endl;
      return false;
  }

  // a framed cell creator
  if(!m_pCellCreator) m_pCellCreator = new SymbolicImageFramedCellCreator();
  else{
      cout << "CellCreator already initialised while calling init of SymbolicImageMorseSpectrumLayer!!"
      << endl;
      return false;
  }

  // ... and a check if the linear extension was activated     
 if( (!m_pLinearExtendedBoxManager) || (!m_pLinearExtendedBoxManager->isLinearExtensionActivated()) ){

    cout << "Linear extension not activated for box manager." << endl;
    return false;
 }

 int startAt = -1;
 if(m_pLinearExtendedBoxManager) startAt = m_pLinearExtendedBoxManager->linearExtensionIsActivatedAt();

  cout << "Start at : " << startAt << endl;;
  if(startAt >= 0){
    for(int i=startAt; i< m_periodFindType.getTotalSize(); i++){

      if(m_periodFindType[i] == CONNECTED_COMPONENTS ){
        cout << "Morse spectrum can not be detected with period finder connected_components "
        << "in subdivision step " << i << "."
        << endl;
        return false;
      }
    }
  }

 return SymbolicImageDefaultLayer::init(extendedBoxManager);
}

bool SymbolicImageMorseSpectrumLayer::isLinearExtensionActive(){

  if((m_pLinearExtendedBoxManager) &&
     (m_pLinearExtendedBoxManager->isLinearExtensionActivated()) ){

     int startAt = m_pLinearExtendedBoxManager->linearExtensionIsActivatedAt();
     if((startAt < 0) ||(startAt > getSubdivisionDepth()) ) return false;
     else return true;
  }
  else return false;
}

bool SymbolicImageMorseSpectrumLayer::parse(Configuration & coverDescription, int numberOfSubdivisions){

  bool correct = SymbolicImageDefaultLayer::parse(coverDescription,numberOfSubdivisions);

  return correct;
}

void SymbolicImageMorseSpectrumLayer::buildEdges(){

  if(!m_pDefaultCellVector->empty()){

    // get the mapping for every cell and add the cells of the boxes, if not NULL,
    // as new edges.

    int size = m_pDefaultCellVector->size();

    ProgressCounter progressCounter(size);
    progressCounter.init();

    SymbolicImageDefaultCellVector::iterator it = m_pDefaultCellVector->begin();
    SymbolicImageDefaultCellVector::iterator end = m_pDefaultCellVector->end();

    while( it != end ){

      progressCounter.increase();

      SymbolicImageFramedCell* cell = DOWN_CAST<SymbolicImageFramedCell*> (*it);

      SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector targetBoxes;

      // now calculate extended box mapping + framing
      m_pLinearExtendedBoxManager->getLinearExtendedBoxMapping(cell->getBox(),targetBoxes, cell->m_framing);

      SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector::iterator tarit    = targetBoxes.begin();
      SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector::iterator tarend = targetBoxes.end();

      while(tarit != tarend){

        SymbolicImageDefaultCell* newEdge  =  DOWN_CAST< SymbolicImageDefaultCell* >((*tarit)->getCell());
        cell->addNewEdge(newEdge);
        tarit++;
      }

      it++;
    }
  }
}

void SymbolicImageMorseSpectrumLayer::locatePeriodic(){

    if(!isLinearExtensionActive()) return SymbolicImageDefaultLayer::locatePeriodic();

    // In this method the recurrent cell sets will
    // be created by the StronglyConnectedComponentsFinder object.

    SymbolicImagePeriodFinder *periodFinder;
    if(m_periodFindType[m_iCurrentSubdivisionDepth] == SHORTEST_PATH){
      
      StronglyConnectedComponentsFinder finder( (*m_pRecurrentSetVector), (*m_pCellSetCreator),  false);
      finder.init(new VectorCellEnumerator(m_pDefaultCellVector));
      finder.locatePeriodic();
      
      // Important !! Here is the change!!
      periodFinder = new GreedyNonstationaryExponentsFinder(m_iMaxPeriods[m_iCurrentSubdivisionDepth],true);
    }
    else if(m_periodFindType[m_iCurrentSubdivisionDepth] == CONNECTED_COMPONENTS){

      cout << "Wrong period finder method for morse spectrum layer. Use 'shortest_path' "
           << "instead of 'connected_components'" << endl;
    }

    // start the period and nonstationary exponents location process
    periodFinder->init(new VectorCellEnumerator(m_pDefaultCellVector));
    periodFinder->locatePeriodic();
    m_iCurrentPeriodicCells = periodFinder->getNumberOfPeriodicCells();
    delete periodFinder;

    cout << "Number of periodic cells: " << m_iCurrentPeriodicCells << endl;
}


SymbolicImageOutputWriter* SymbolicImageMorseSpectrumLayer::parseOutputWriter(Configuration & writerDescription, int numberOfSubdivisions){

    //string typeStr = writerDescription.getString("TYPE_KEY");

    if (writerDescription.checkForEnumValue("TYPE_KEY", SymbolicImageMorseSpectrumLayer::MorseSpectrumOutputWriter::key))
    {
      MorseSpectrumOutputWriter* output = new MorseSpectrumOutputWriter(this);

      if(!output->parse(writerDescription,numberOfSubdivisions)){
        delete output;
        return NULL;
      };

      return output;
     }
     return NULL;
}
