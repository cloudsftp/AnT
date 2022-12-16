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
 * $Id: symbolicimageabstractboxmanager.cpp,v 1.6 2004/10/20 08:33:28 fundindy Exp $
 *
 */

#include "symbolicimageabstractboxmanager.hpp"
/*
bool SymbolicImageAbstractBoxManager::parseModificationPart(Configuration &modificationDescription, string modificationString, string modificationNameString, Array< int > &modificationArray ){

  //string itModStr = "NUMBER_OF_ITERATIONS_ARRAY_KEY_MODIFICATION";
  //string itModNameStr = "number_of_iterations_modification";
  string itModStr = modificationString + "_MODIFICATION";
  string itModNameStr = modificationNameString + "_modification";
  int i=0;
  while(modificationDescription.checkForKey(itModStr + "[" + toString(i) + "]")){

    string itModNumberStr = itModStr + "[" + toString(i) + "]";
    string itModNameNumberStr = itModNameStr  + "[" + toString(i) + "]";
    Configuration numberOfIterationsDescription = modificationDescription.getSubConfiguration(itModNumberStr);
    int iSubdivisionStep = -1;
    int iModifiedValue;
    bool bOnlyThisSubdivision = true;
    if(numberOfIterationsDescription.checkForKey("SUBDIVISION_STEP_KEY")){

      iSubdivisionStep = numberOfIterationsDescription.getInteger("SUBDIVISION_STEP_KEY");
      if( iSubdivisionStep < 0){
        cout << "Invalid subdivision step definition in " <<  itModNameNumberStr
        << endl;
        return false;
      }
      else if( iSubdivisionStep >= modificationArray.getTotalSize() ){
        cout << "Warning!! Subdivision step in " <<  itModNameNumberStr
        << " never reached."
        << endl;
      }

    }
    else{
      cout << "No subdivision step defined in " <<  itModNameNumberStr
      << endl;
      return false;
    }

    if(numberOfIterationsDescription.checkForKey("APPLICATION_MODE_KEY")){
     if(numberOfIterationsDescription.checkForEnumValue("APPLICATION_MODE_KEY",
            "ALL_FOLLOWING_SUBDIVISION_STEPS")){
       bOnlyThisSubdivision = false;
     }

    }

    if(numberOfIterationsDescription.checkForKey("MODIFIED_VALUE_KEY")){

      iModifiedValue = numberOfIterationsDescription.getInteger("MODIFIED_VALUE_KEY");
    }
    else{
      cout << "No modified value defined in " <<  itModNameNumberStr
      << endl;
      return false;
    }

    int end;
    if(bOnlyThisSubdivision) end = iSubdivisionStep+1;
    else end = modificationArray.getTotalSize();

    for(int j=iSubdivisionStep; j < end; j++){
      modificationArray[j] = iModifiedValue;
    }

    i++;
  }
  return true;
}

bool SymbolicImageAbstractBoxManager::parseBooleanModificationPart(Configuration &modificationDescription, string modificationString, string modificationNameString, Array< bool > &modificationArray ){

  //string itModStr = "NUMBER_OF_ITERATIONS_ARRAY_KEY_MODIFICATION";
  //string itModNameStr = "number_of_iterations_modification";
  string itModStr = modificationString + "_MODIFICATION";
  string itModNameStr = modificationNameString + "_modification";
  int i=0;
  while(modificationDescription.checkForKey(itModStr + "[" + toString(i) + "]")){

    string itModNumberStr = itModStr + "[" + toString(i) + "]";
    string itModNameNumberStr = itModNameStr  + "[" + toString(i) + "]";
    Configuration numberOfIterationsDescription = modificationDescription.getSubConfiguration(itModNumberStr);
    int iSubdivisionStep = -1;
    bool bModifiedValue;
    bool bOnlyThisSubdivision = true;
    if(numberOfIterationsDescription.checkForKey("SUBDIVISION_STEP_KEY")){

      iSubdivisionStep = numberOfIterationsDescription.getInteger("SUBDIVISION_STEP_KEY");
      if( iSubdivisionStep < 0){
        cout << "Invalid subdivision step definition in " <<  itModNameNumberStr
        << endl;
        return false;
      }
      else if( iSubdivisionStep >= modificationArray.getTotalSize() ){
        cout << "Warning!! Subdivision step in " <<  itModNameNumberStr
        << " never reached."
        << endl;
      }

    }
    else{
      cout << "No subdivision step defined in " <<  itModNameNumberStr
      << endl;
      return false;
    }

    if(numberOfIterationsDescription.checkForKey("APPLICATION_MODE_KEY")){
     if(numberOfIterationsDescription.checkForEnumValue("APPLICATION_MODE_KEY",
            "ALL_FOLLOWING_SUBDIVISION_STEPS")){
       bOnlyThisSubdivision = false;
     }

    }

    if(numberOfIterationsDescription.checkForKey("MODIFIED_VALUE_KEY")){

      bModifiedValue = numberOfIterationsDescription.getBool("MODIFIED_VALUE_KEY");
    }
    else{
      cout << "No modified value defined in " <<  itModNameNumberStr
      << endl;
      return false;
    }

    int end;
    if(bOnlyThisSubdivision) end = iSubdivisionStep+1;
    else end = modificationArray.getTotalSize();

    for(int j=iSubdivisionStep; j < end; j++){
      modificationArray[j] = bModifiedValue;
    }

    i++;
  }
  return true;
}

bool SymbolicImageAbstractBoxManager::parseRealModificationPart(Configuration &modificationDescription, string modificationString, string modificationNameString, Array< real_t > &modificationArray ){

  //string itModStr = "NUMBER_OF_ITERATIONS_ARRAY_KEY_MODIFICATION";
  //string itModNameStr = "number_of_iterations_modification";
  string itModStr = modificationString + "_MODIFICATION";
  string itModNameStr = modificationNameString + "_modification";
  int i=0;
  while(modificationDescription.checkForKey(itModStr + "[" + toString(i) + "]")){

    string itModNumberStr = itModStr + "[" + toString(i) + "]";
    string itModNameNumberStr = itModNameStr  + "[" + toString(i) + "]";
    Configuration numberOfIterationsDescription = modificationDescription.getSubConfiguration(itModNumberStr);
    int iSubdivisionStep = -1;
    real_t rModifiedValue;
    bool bOnlyThisSubdivision = true;
    if(numberOfIterationsDescription.checkForKey("SUBDIVISION_STEP_KEY")){

      iSubdivisionStep = numberOfIterationsDescription.getInteger("SUBDIVISION_STEP_KEY");
      if( iSubdivisionStep < 0){
        cout << "Invalid subdivision step definition in " <<  itModNameNumberStr
        << endl;
        return false;
      }
      else if( iSubdivisionStep >= modificationArray.getTotalSize() ){
        cout << "Warning!! Subdivision step in " <<  itModNameNumberStr
        << " never reached."
        << endl;
      }

    }
    else{
      cout << "No subdivision step defined in " <<  itModNameNumberStr
      << endl;
      return false;
    }

    if(numberOfIterationsDescription.checkForKey("APPLICATION_MODE_KEY")){
     if(numberOfIterationsDescription.checkForEnumValue("APPLICATION_MODE_KEY",
            "ALL_FOLLOWING_SUBDIVISION_STEPS")){
       bOnlyThisSubdivision = false;
     }

    }

    if(numberOfIterationsDescription.checkForKey("MODIFIED_VALUE_KEY")){

      rModifiedValue = numberOfIterationsDescription.getReal("MODIFIED_VALUE_KEY");
    }
    else{
      cout << "No modified value defined in " <<  itModNameNumberStr
      << endl;
      return false;
    }

    int end;
    if(bOnlyThisSubdivision) end = iSubdivisionStep+1;
    else end = modificationArray.getTotalSize();

    for(int j=iSubdivisionStep; j < end; j++){
      modificationArray[j] = rModifiedValue;
    }

    i++;
  }
  return true;
}
*/

SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxManager(){

  m_pIterData = NULL;

}
SymbolicImageAbstractBoxManager::~SymbolicImageAbstractBoxManager(){
}

void SymbolicImageAbstractBoxManager::setCurrentIterData(IterData &iterData){

  m_pIterData = &iterData;
}

SymbolicImageAbstractLinearExtendedBoxManager::SymbolicImageAbstractLinearExtendedBoxManager(){}

SymbolicImageAbstractLinearExtendedBoxManager::~SymbolicImageAbstractLinearExtendedBoxManager(){}

void SymbolicImageAbstractLinearExtendedBoxManager::getLinearExtendedBoxMapping(SymbolicImageAbstractBox* box,
                            SymbolicImageAbstractBoxVector &targetBoxes, SymbolicImageFraming &framing){

      cerr << "Function for linearly extended box mapping not implemented for this class."
      << endl << Error::Exit;                      
}

bool SymbolicImageAbstractLinearExtendedBoxManager::isLinearExtensionActivated(){

  return false;
}


int SymbolicImageAbstractLinearExtendedBoxManager::linearExtensionIsActivatedAt(){

  return -1;
}
