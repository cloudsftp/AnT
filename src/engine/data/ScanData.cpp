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
 * $Id: ScanData.cpp,v 1.10 2010/11/11 14:53:15 schanzml Exp $
 *
 */

#include "config.h"
#include "data/ScannableObjects.hpp"
#include "methods/output/IOStreamFactory.hpp"
#include "network/ANPClient.hpp"
#include "network/ANPServer.hpp"
#include "ScanData.hpp"
#include "../utils/strconv/StringConverter.hpp"
#include "utils/datareader/ExternalDataTypes.hpp"

#include "../cas/CoexistingAttractorScan.hpp"


#define DEBUG__FROM_FILE_SCAN_ITEM 0

/** globally visible ANPClient object */
extern ANPClient* anpClient;

/* ***************************************************** */

ScanData::ScanData (IterData* data) : 
  finalFlag (false), 
  iterDataPtr (data)
{
}

// virtual 
bool 
ScanData::isFinal (AbstractTransition*)
{
  return finalFlag;
}

// static 
ScanData* 
ScanData::create (Configuration& scanDescription, 
		  IterData* iterData,
		  runMode_t aRunMode)
{
  ScanItemSequence* seq;

  debugMsg1("ScanData: creating...");

  if (scanDescription.checkForEnumValue ("SCAN_TYPE_KEY",
					 "NESTED_ITEMS_KEY"))
  {
    seq = new ScanItemSequence (iterData);
  } 
  else if (scanDescription.checkForEnumValue ("SCAN_TYPE_KEY",
					      "NESTED_ITEMS_CAS_KEY"))
  {
    if ((aRunMode == SERVER) || (aRunMode == CLIENT))
    {
      cerr << "Only STANDALONE run mode supported for coexisting "
	   << "attractor scan." << endl << Error::Exit;
    }
    seq = new CAScanItemSequence (iterData);
  }
  else
  {
    cerr << "Unsupported scan type."
	 << endl << Error::Exit;
  }

  seq->runMode = aRunMode;

  // initialize the attribute 'scanMode' to contain 
  // the dimension of this scan
  if (scanDescription.checkForKey ("MODE_KEY"))
  {
    seq->scanMode = scanDescription.getInteger ("MODE_KEY");
    if (seq->scanMode < 0)
      cerr << "The setting of the field '" 
	   << scanDescription.getOriginalKey ("MODE_KEY")
	   << "' is " << seq->scanMode
	   << ". Only non-negative values"
	   << " are enabled here."
	   << endl << Error::Exit;
  }
  else
  {
    seq->scanMode = 0; // default value.
    // Please let it be correct, if 'scanMode' setting is not given.
    // We use it by creation of poincare maps...
  }

  if ( (seq->scanMode == 0) && 
       ((seq->runMode == SERVER) || (seq->runMode == CLIENT)) )
  {
    cerr << "CLIENT / SERVER run mode is only for scan runs implemented. "
	 << "Unfortunately you perform a single run, please do it in the "
	 << "STANDALONE run mode."
	 << endl
	 << Error::Exit;
  }
    
  return seq;
  debugMsg1("ScanData: created.");
}

ostream& operator<< (ostream& os, const ScanData& scanData)
{
  scanData >> os;
  return os;
}

/* ***************************************************** */

ScanItemSequence::ScanItemSequence (IterData* iterData)
  : ScanData (iterData),
    firstCall (true)
{}

//virtual 
void 
ScanItemSequence::operator>> (ostream& os) const
{
  for (seq_t::const_iterator i = sequence.begin ();
       i != sequence.end ();
       ++i)
  {
    *(*i) >> os;
    os << " ";
  }
} /*: 'operator>>' */

//virtual 
void 
ScanItemSequence::standaloneScanNext ()
{
  if (firstCall) {
    // don't increment yet on the first call of this method
    firstCall = false;
    set ();

    return;
  }

  // go to the next scanpoint, see if it is even valid
  finalFlag = inc (); 

  set ();
}

bool
ScanItemSequence::inc ()
{
#if 0
  bool scanFlag = false;
  for (seq_t::iterator i = sequence.begin ();
       (!scanFlag) && (i != sequence.end ());
       ++i)
  {
    scanFlag = (scanFlag || (*i)->inc ());
  }

  return (!scanFlag); 
  // this is true if the last scanpoint has been reached
#endif

  for (seq_t::iterator i = sequence.begin ();
       i != sequence.end ();
       ++i)
  {
    if ((*i)->inc ()) {
      // found a ScanItem to increase
      return false;
    } else {
      // reset current and try to increase next
      (*i)->reset ();
    }
  }

  // no ScanItem increased, end reached
  return true;
}

void
ScanItemSequence::set ()
{
  for (seq_t::iterator i = sequence.begin ();
       i != sequence.end (); ++i)
  {
    (*i)->set ();
  }
}

void
ScanItemSequence::set (string& scanpoint)
{
  std::istringstream is (scanpoint.c_str ());

  for (seq_t::iterator i = sequence.begin (); 
       i != sequence.end (); ++i)
  {
    (*i)->set (is);
  }
}

void ScanItemSequence::reset ()
{
  firstCall = true;
  finalFlag = false;

  for (seq_t::iterator i = sequence.begin ();
       i != sequence.end (); ++i)
  {
    (*i)->reset ();
  }
}

string*
ScanItemSequence::get ()
{
  std::ostringstream os;

  IOStreamFactory::setPrecision (&os);

  for (seq_t::iterator i = sequence.begin (); 
       i != sequence.end (); ++i)
  {
    (*i)->get (os);
  }

  // use the compiler-dependent define:    
  doFlush (os);

  string* scanpoint = new string (os.str ());

  return scanpoint;
}

//virtual 
void 
ScanItemSequence::netClientScanNext ()
{
  ioStreamFactory->commit();

  // fetch next scanpoint from server
  string* scanPoint = anpClient->getScanPoint ();

  set (*scanPoint);

#if 0 /* commented out */
  // server has finished calculation
  finalFlag = true;
#endif
}


//virtual 
void
ScanItemSequence::netServerScanNext ()
{
#if ANT_HAS_LIBSOCKET
  // This method behaves very differently from the one for client and
  // standalone !
  // It loops until all scanpoints are done, it does not exit before
  // the whole scan is finished.

  ANPServer anpServer (this);
  anpServer.communicationLoop ();

  finalFlag = true;
#endif
}

void 
ScanItemSequence::add (AbstractScanItem* item)
{
  sequence.push_back (item);
}

int ScanItemSequence::getScanMode ()
{
  return scanMode;
}


void 
ScanItemSequence::initialize (Configuration& scanDescription)
{
  debugMsg1("ScanItemSequence: initializing...");
  
  sequence = ScanItemSequence::seq_t ();
  /*: discard old 'sequence' */
  
  for (int itemIndex = 0; itemIndex < scanMode; ++itemIndex)
  {
    add (AbstractScanItem::getItem (scanDescription, itemIndex));
  }
  debugMsg1("ScanItemSequence: initialized");
}

// virtual
ScanItemSequence::~ScanItemSequence ()
{}


/* ***************************************************** */
ScanTransition::ScanTransition (string aName) :
  AbstractTransition (aName)
{}

// virtual 
void 
ScanTransition::execute (AbstractState& s)
{
  ScanData& scanData = DOWN_CAST <ScanData&> (s);
  execute (scanData);
}

/* ***************************************************** */
// static 
AbstractScanItem* 
AbstractScanItem::getItem (Configuration& scanDescription, 
			   int index)
{
  string tmpStr = string("ITEM") + "[" + toString (index) + "]";

  if (! scanDescription.checkForKey (tmpStr))
  {
    cerr << "scan item with index "
	 << index 
	 << " not found."
	 << endl << Error::Exit;
  }
    
  Configuration itemDescription 
    = scanDescription.getSubConfiguration (tmpStr);

  if (itemDescription.checkForEnumValue 
      ("ITEM_TYPE_KEY", "REAL_LIN_KEY") ) 
  {
    return new RealLinearScanItem (itemDescription);
  }
  if (itemDescription.checkForEnumValue 
      ("ITEM_TYPE_KEY", "REAL_LOG_KEY") ) 
  { 
    return new RealLogarithmicScanItem (itemDescription);
  }
  if (itemDescription.checkForEnumValue 
      ("ITEM_TYPE_KEY", "INT_LIN_KEY") ) 
  { 
    return new IntegerLinearScanItem (itemDescription);
  }
  if (itemDescription.checkForEnumValue 
      ("ITEM_TYPE_KEY", "INT_LOG_KEY") ) 
  { 
    return new IntegerLogarithmicScanItem (itemDescription);
  }
  if (itemDescription.checkForEnumValue 
      ("ITEM_TYPE_KEY", "REAL_LIN_TWO_DIM_KEY") ) 
  { 
    return new TwoDimensionalRealLinearScanItem (itemDescription);
  }
  if (itemDescription.checkForEnumValue 
      ("ITEM_TYPE_KEY", "REAL_ELLIPTIC_TWO_DIM_KEY") ) 
  { 
    return new TwoDimensionalRealEllipticScanItem (itemDescription);
  }
  if (itemDescription.checkForEnumValue 
      ("ITEM_TYPE_KEY", "FROM_FILE_KEY") ) 
  { 
    return new FromFileScanItem (itemDescription);
  }
  cerr << "An unrecognized setting '" 
       << itemDescription.getEnum ("ITEM_TYPE_KEY")
       << "' found at the key '"       
       << itemDescription.getOriginalKey ("ITEM_TYPE_KEY")
       << "' of the scan item '"      
       << tmpStr << "'." 
       << endl 
       << Error::Exit;
    
  return NULL;
}

/* ***************************************************** */

long
IndexableScanItem::getNumPoints ()
{ 
  return numPoints; 
}

long
IndexableScanItem::getCurrentIndex ()
{ 
  return index; 
}

void
IndexableScanItem::setCurrentIndex (long newIndex)
{
  assert (index >= 0 && index < numPoints);

  index = newIndex;

  calc ();
}

bool
IndexableScanItem::inc ()
{
  // reached the end (valid: [0:numPoints-1])
  if (index >= numPoints-1) {
    return false;
  }

  // go to next scan point
  ++index;

  // calculate scan point
  calc ();

  return true;
}

bool
IndexableScanItem::dec ()
{
  // reached the start (valid: [0:numPoints-1])
  if (index <= 0) {
    return false;
  }

  // go to previous scan point
  --index;

  // calculate scan point
  calc ();

  return true;
}

bool 
IndexableScanItem::check ()
{
  if (numPoints <= 1)
    cerr << "The number of scan points must be greater then 1. "
	 << "Current setting " 
	 << numPoints
	 << " not accepted."
	 << endl << Error::Exit;

  return true;
}

/* ***************************************************** */
// virtual 
template<typename ITEM_TYPE>
void 
BasicScanItem<ITEM_TYPE>::operator>> (ostream& os) const
{
  os << *objPtr;
}

template<typename ITEM_TYPE>
void 
BasicScanItem<ITEM_TYPE>::set (void)
{
  *objPtr = currentValue;
}

template<typename ITEM_TYPE>
void 
BasicScanItem<ITEM_TYPE>::reset (void)
{
  index = 0;
  currentValue = minValue;
}

template<typename ITEM_TYPE>
void
BasicScanItem<ITEM_TYPE>::get (ostream& os)
{
  os << currentValue << endl
     << index << endl;
}

template<typename ITEM_TYPE>
void 
BasicScanItem<ITEM_TYPE>::set (istream& is)
{
  is >> currentValue >> index;
  set ();
}


template<typename ITEM_TYPE>
bool 
BasicScanItem<ITEM_TYPE>::check ()
{
  IndexableScanItem::check ();

  if (minValue == maxValue)
    cerr << "Minimal and maximal values of a scan item can not be equal. "
	 << "Current setting of both values " 
	 << minValue
	 << " not accepted."
	 << endl << Error::Exit;

  return true;
}

template<typename ITEM_TYPE>
BasicScanItem<ITEM_TYPE>::
~BasicScanItem ()
{}

/* ***************************************************** */
RealScanItem::RealScanItem (Configuration& itemDescription)
{
  initialize (itemDescription);
}

void 
RealScanItem::initialize (Configuration& itemDescription)
{
  minValue = itemDescription.getReal ("MIN_KEY");
  maxValue = itemDescription.getReal ("MAX_KEY");
  numPoints = itemDescription.getLong  ("POINTS_KEY");

  BasicScanItem<real_t>::check ();
}

// virtual 
RealScanItem::~RealScanItem () 
{
}

/* ***************************************************** */
IntegerScanItem::IntegerScanItem (Configuration& itemDescription)
{
  initialize (itemDescription);
}

void 
IntegerScanItem::initialize (Configuration& itemDescription)
{
#if 0
  /*: commented out, because 'GlobalKeys.cfg' says that these values
    are @real... */
  minValue = itemDescription.getLong ("MIN_KEY");
  maxValue = itemDescription.getLong ("MAX_KEY");
#endif
  minValue = static_cast<long>
    ( itemDescription.getReal ("MIN_KEY") );
  maxValue = static_cast<long>
    ( itemDescription.getReal ("MAX_KEY") );
  numPoints = itemDescription.getLong  ("POINTS_KEY");

  BasicScanItem<long>::check ();
}

// virtual 
IntegerScanItem::~IntegerScanItem () 
{}

/* ***************************************************** */
RealLinearScanItem::
RealLinearScanItem (Configuration& itemDescription) :
  RealScanItem (itemDescription)
{ 
  const string objKey = itemDescription.getString  ("OBJECT_KEY");

  objPtr = scannableObjects.get<typeof(*objPtr)> (objKey);  // (?)

  step = (maxValue - minValue) / (numPoints - 1);

  BasicScanItem<real_t>::reset (); /* do not forget! */
}

// virtual 
void
RealLinearScanItem::calc ()
{
  currentValue = minValue + index * step; 
}

/* ***************************************************** */
RealLogarithmicScanItem::
RealLogarithmicScanItem (Configuration& itemDescription) : 
  RealScanItem (itemDescription)
{ 
  if (minValue <= 0)
    cerr << "RealLogarithmicScanItem error: the minimal value "
	 << minValue
	 << " can not be accepted: only positive values are admissible."
	 << endl << Error::Exit;

  if (maxValue <= 0)
    cerr << "RealLogarithmicScanItem error: the maximal value "
	 << maxValue
	 << " can not be accepted: only positive values are admissible."
	 << endl << Error::Exit;

  const string objKey = itemDescription.getString  ("OBJECT_KEY");

  objPtr = scannableObjects.get<typeof(*objPtr)> (objKey);  // (?)

  step = (log(maxValue) - log(minValue)) / (numPoints - 1);

  BasicScanItem<real_t>::reset (); /* do not forget! */
}

// virtual 
void
RealLogarithmicScanItem::calc ()
{
  currentValue = exp (log(minValue) + index * step); 
}

/* ***************************************************** */
IntegerLinearScanItem::
IntegerLinearScanItem (Configuration& itemDescription) :
  IntegerScanItem (itemDescription)
{ 
  const string objKey = itemDescription.getString ("OBJECT_KEY");
  
  objPtr = scannableObjects.get<typeof(*objPtr)> (objKey);  // (?)

  step = (long) 
    floor (((real_t)(maxValue - minValue)) / ((real_t)(numPoints - 1)));

  if (step == 0)
  {
    if (minValue < maxValue)
      step = 1;
    else 
      step = -1;
  }

  numPoints = (maxValue - minValue) / step;

  BasicScanItem<long>::reset (); /* do not forget! */
}

// virtual 
void
IntegerLinearScanItem::calc ()
{
  currentValue = minValue + index * step; 
}

/* ***************************************************** */
IntegerLogarithmicScanItem::
IntegerLogarithmicScanItem (Configuration& itemDescription ) :
  IntegerScanItem (itemDescription)
{ 
  if (minValue <= 0)
    cerr << "IntegerLogarithmicScanItem error: the minimal value "
	 << minValue
	 << " can not be accepted: only positive values are admissible."
	 << endl << Error::Exit;

  if (maxValue <= 0)
    cerr << "IntegerLogarithmicScanItem error: the maximal value "
	 << maxValue
	 << " can not be accepted: only positive values are admissible."
	 << endl << Error::Exit;

  const string objKey = itemDescription.getString  ("OBJECT_KEY");

  objPtr = scannableObjects.get<typeof(*objPtr)> (objKey);  // (?)

  // real value!
  step = (log((real_t)maxValue) - log((real_t)minValue)) / (numPoints - 1);

  BasicScanItem<long>::reset (); /* do not forget! */
}

// virtual 
void
IntegerLogarithmicScanItem::calc ()
{
  currentValue = 
    (long) ceil (exp (log ((real_t)minValue) + (real_t)(index * step)));
}

// virtual 
template<typename ITEM_TYPE>
void 
TwoDimensionalScanItem<ITEM_TYPE>::operator>> (ostream& os) const
{
  os << *objPtr1 
     << " "
     << *objPtr2;
}

//virtual 
template<typename ITEM_TYPE>
void 
TwoDimensionalScanItem<ITEM_TYPE>::set ()
{
  *objPtr1 = currentValue1;
  *objPtr2 = currentValue2;
}

//virtual 
template<typename ITEM_TYPE>
void 
TwoDimensionalScanItem<ITEM_TYPE>::reset (void)
{
  index = 0;
  currentValue1 = minValue1;
  currentValue2 = minValue2;
}

template<typename ITEM_TYPE>
bool 
TwoDimensionalScanItem<ITEM_TYPE>::check ()
{
  IndexableScanItem::check ();

  if (minValue1 == maxValue1)
    cerr << "Minimal and maximal values of a scan item can not be equal. "
	 << "Current setting of both values " 
	 << minValue1
	 << " not accepted."
	 << endl << Error::Exit;

  if (minValue2 == maxValue2)
    cerr << "Minimal and maximal values of a scan item can not be equal. "
	 << "Current setting of both values " 
	 << minValue1
	 << " not accepted."
	 << endl << Error::Exit;

  return true;

}

//virtual 
template<typename ITEM_TYPE>
void 
TwoDimensionalScanItem<ITEM_TYPE>::get (ostream& os)
{
  os << currentValue1 << endl
     << currentValue2 << endl
     << index << endl;

}

//virtual 
template<typename ITEM_TYPE>
void 
TwoDimensionalScanItem<ITEM_TYPE>::set (istream& is)
{
  is >> currentValue1 >> currentValue2 >> index;

  set ();
}


TwoDimensionalRealLinearScanItem::
TwoDimensionalRealLinearScanItem (Configuration& itemDescription)
{
  initialize (itemDescription);

  const string objKey1 = itemDescription.getString  ("OBJECT1_KEY");
  objPtr1 = scannableObjects.get<typeof(*objPtr1)> (objKey1);

  const string objKey2 = itemDescription.getString  ("OBJECT2_KEY");
  objPtr2 = scannableObjects.get<typeof(*objPtr2)> (objKey2);

  step1 = (maxValue1 - minValue1) / (numPoints - 1);
  step2 = (maxValue2 - minValue2) / (numPoints - 1);

  reset (); /* do not forget! */
}


void
TwoDimensionalRealLinearScanItem::calc ()
{
  currentValue1 = minValue1 + index * step1; 
  currentValue2 = minValue2 + index * step2; 
}

//virtual 
void 
TwoDimensionalRealLinearScanItem::
initialize (Configuration& itemDescription)
{
  minValue1 = itemDescription.getReal ("MIN1_KEY");
  maxValue1 = itemDescription.getReal ("MAX1_KEY");
  minValue2 = itemDescription.getReal ("MIN2_KEY");
  maxValue2 = itemDescription.getReal ("MAX2_KEY");

  numPoints = itemDescription.getLong  ("POINTS_KEY");

  this->check ();
}

//virtual 
TwoDimensionalRealLinearScanItem::
~TwoDimensionalRealLinearScanItem ()
{}

/* ***************************************************** */


TwoDimensionalRealEllipticScanItem::
TwoDimensionalRealEllipticScanItem (Configuration& itemDescription)
{
  initialize (itemDescription);

  const string objKey1 = itemDescription.getString  ("OBJECT1_KEY");
  objPtr1 = scannableObjects.get<typeof(*objPtr1)> (objKey1);

  const string objKey2 = itemDescription.getString  ("OBJECT2_KEY");
  objPtr2 = scannableObjects.get<typeof(*objPtr2)> (objKey2);

  phiStep = (phiMax - phiMin) / (numPoints - 1);

  minValue1 = center1 + r1 * cos (phiMin * Two_Pi / 360.0);
  maxValue1 = center1 + r1 * cos (phiMax * Two_Pi / 360.0);

  minValue2 = center2 + r2 * sin (phiMin * Two_Pi / 360.0);
  maxValue2 = center2 + r2 * sin (phiMax * Two_Pi / 360.0);

  reset (); /* do not forget! */
}

void
TwoDimensionalRealEllipticScanItem::
reset ()
{
  TwoDimensionalScanItem<real_t>::reset ();
  currentPhi = phiMin;
}


// virtual 
void 
TwoDimensionalRealEllipticScanItem::operator>> (ostream& os) const
{
  os << currentValue1 
     << " "
     << currentValue2 
     << " "
     << currentPhi;
}


// virtual 
void
TwoDimensionalRealEllipticScanItem::calc ()
{
  currentPhi    = phiMin + index * phiStep;
  currentValue1 = center1 + r1 * cos (currentPhi * Two_Pi / 360.0); 
  currentValue2 = center2 + r2 * sin (currentPhi * Two_Pi / 360.0); 
}


// virtual 
bool
TwoDimensionalRealEllipticScanItem::check ()
{
  IndexableScanItem::check ();

  if (phiMin == phiMax)
    cerr << "Minimal and maximal values of a scan item can not be equal. "
	 << "Current setting of both values " 
	 << phiMin
	 << " not accepted."
	 << endl << Error::Exit;

  if (r1 <= 0)
    cerr << "The first axis of the ellipse " 
	 << "in the scan item has to be positive. "
	 << "Current setting  " 
	 << r1
	 << " not accepted."
	 << endl << Error::Exit;

  if (r2 <= 0)
    cerr << "The second axis of the ellipse " 
	 << "in the scan item has to be positive. "
	 << "Current setting  " 
	 << r2
	 << " not accepted."
	 << endl << Error::Exit;

  return true;
}


//virtual 
void 
TwoDimensionalRealEllipticScanItem::
initialize (Configuration& itemDescription)
{
  phiMin = itemDescription.getReal ("MIN_ANGLE_KEY");
  phiMax = itemDescription.getReal ("MAX_ANGLE_KEY");

  numPoints = itemDescription.getLong  ("POINTS_KEY");

  r1 = itemDescription.getReal ("AXIS1_KEY");
  r2 = itemDescription.getReal ("AXIS2_KEY");

  center1 = itemDescription.getReal ("CENTER1_KEY");
  center2 = itemDescription.getReal ("CENTER2_KEY");

  this->check ();
}

//virtual 
TwoDimensionalRealEllipticScanItem::
~TwoDimensionalRealEllipticScanItem ()
{}


/* ***************************************************** */

// virtual 
void 
FromFileScanItem::
operator>> (ostream& os) const
{

  if (objects.getTotalSize () == 0)
  {
    return;
  }

  os << *(objects[0]);

  for (int i = 1; i < objects.getTotalSize (); ++i)
  {
    os << " "
       << *(objects[i]);
  }

}


// virtual 
void  
FromFileScanItem::
set ()
{
  for (int i = 0; i < objects.getTotalSize (); ++i)
  {
    *(objects[i]) = currentValues[i];
  }


}

// virtual 
void  
FromFileScanItem::
get (ostream& os)
{
  for (int i = 0; i < objects.getTotalSize (); ++i)
  {
    os << currentValues[i] << endl;
  }

  os << index << endl;
}

// virtual 
void  
FromFileScanItem::
set (istream& is)
{
  for (int i = 0; i < objects.getTotalSize (); ++i)
  {
    is >> currentValues[i];
  }

  is >> index;
}

// virtual 
void  
FromFileScanItem::
reset (void)
{
  index = 0;
  
  if (externalDataReader != NULL)
  {
    delete externalDataReader;
  }

  // reopen the file
  externalDataReader = new ExternalDataReader ();
  initExternalDataReader (fileName, numberOfColumns);

  for (int i = 0; i < objects.getTotalSize (); ++i)
  {
    externalDataReader->addParameter (dataColumns[i], 
				      ExternalDataTypes::ptFloat);
  }

  // read the first row:

  // IT SEEMS TO BE A BUG of the externalDataReader, but without this
  // call of 'existsData ()' it does not work correctly. The getFloatParam
  // returns then 0 instead of the values from the file. Very strange!
  if (! externalDataReader->existsData ())
  {
    return;
  }

  for (int i = 0; i < objects.getTotalSize (); ++i)
  {
    currentValues[i] 
      = externalDataReader->getFloatParam (i+1); 

#if DEBUG__FROM_FILE_SCAN_ITEM
    cout << "currentValues["
	 << i
	 << "] = "
	 <<  currentValues[i]
	 << endl;
#endif
  }

  externalDataReader->next(); // go to the next row
}

// virtual 
bool  
FromFileScanItem::
check ()
{
  return IndexableScanItem::check ();
}

void  
FromFileScanItem::
initExternalDataReader (string fileName, int numberOfColumns)
{
  assert (numberOfColumns > 0);
  externalDataReader->init (fileName.c_str (), numberOfColumns);
  externalDataReader->errorOnEmptyRow = false;
  externalDataReader->throwExceptionOnBadRow = true; //termination on error
}


FromFileScanItem::
FromFileScanItem (Configuration& itemDescription)
{
#if DEBUG__FROM_FILE_SCAN_ITEM
  cout << "call initialize... "
       << endl;
#endif
  initialize (itemDescription);
#if DEBUG__FROM_FILE_SCAN_ITEM
  cout << "ok."
       << endl
       << "call reset... "
       << endl;
#endif
  reset ();
#if DEBUG__FROM_FILE_SCAN_ITEM
  cout << "ok."
       << endl;
#endif
}

// virtual 
void  
FromFileScanItem::
initialize (Configuration& itemDescription)
{
  // data source file
  fileName = itemDescription.getString ("EXTERNAL_DATA_FILENAME_KEY");

  numberOfColumns = itemDescription.getInteger ("NUMBER_OF_COLUMNS_KEY");

  if (numberOfColumns <= 0)
  {
    cerr << "A positive value expected on the field '"
	 << itemDescription.getOriginalKey ("NUMBER_OF_COLUMNS_KEY")
	 << "' of the 'from file' scan item. Found value "
	 << numberOfColumns 
	 << "can not be accepted."
	 << endl << Error::Exit;
  }

  // check the data file and count the number of lines
  externalDataReader = new ExternalDataReader ();
  initExternalDataReader (fileName, numberOfColumns);

  // dummy
  externalDataReader->addParameter (1, ExternalDataTypes::ptFloat); 

#if DEBUG__FROM_FILE_SCAN_ITEM
  cout << "count the lines... "
       << endl;
#endif

  real_t dummy;
  long numberOfLines = 0;
  while (externalDataReader->existsData ())
  {
    dummy = externalDataReader->getFloatParam (1);
    ++numberOfLines;
    externalDataReader->next ();
  }

  delete externalDataReader;
  externalDataReader = NULL;

  numPoints = numberOfLines;
  //: number of lines counted;
#if DEBUG__FROM_FILE_SCAN_ITEM
  cout << "number of lines: "
       << numPoints
       << endl;
#endif

  // parse the given configuration:
  int numberOfSubitems = 0;
  while (1)
  {
    string tmpStr =  string("SUBITEM") 
      + "[" + toString (numberOfSubitems) + "]";

    if (! itemDescription.checkForKey (tmpStr))
    {
      break;
    }
    
    ++ numberOfSubitems;
  }

  if (numberOfSubitems <= 0)
  {
    cerr << "No subitem found! The 'from file' scan item "
	 << "can not be constructed."
	 << endl << Error::Exit;
  }

#if DEBUG__FROM_FILE_SCAN_ITEM
  cout << "number of subitems: " 
       << numberOfSubitems 
       << endl;
#endif

  // alloc the local arrays
  objects.alloc (numberOfSubitems);
  dataColumns.alloc (numberOfSubitems);
  currentValues.alloc (numberOfSubitems);

  for (int j = 0; j < numberOfSubitems; ++j)
  {
    string tmpStr =  string("SUBITEM") + "[" + toString (j) + "]";

    Configuration subitemDescription 
    = itemDescription.getSubConfiguration (tmpStr);

    string objKey = subitemDescription.getString  ("OBJECT_KEY");

    objects[j] = scannableObjects.get<real_t> (objKey);

    dataColumns[j] = subitemDescription.getInteger  ("COLUMN_KEY");
  }

#if DEBUG__FROM_FILE_SCAN_ITEM
    cout << "dataColumns = "
	 << dataColumns
	 << endl;
#endif
}

// virtual 
void
FromFileScanItem::calc ()
{
  if (! externalDataReader->existsData ())
  {
    return;
  }

  // read the next row:
  for (int i = 0; i < objects.getTotalSize (); ++i)
  {
    currentValues[i] = 
      externalDataReader->getFloatParam (i+1); 
  }

  externalDataReader->next (); // go to the next row
}

