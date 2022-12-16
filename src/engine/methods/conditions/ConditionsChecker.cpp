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
 * $Id: ConditionsChecker.cpp,v 1.8 2005/11/21 16:43:10 hamannho Exp $
 *
 */


#include "ConditionsChecker.hpp"
#include "data/DynSysData.hpp"
#include "methods/output/IOStreamFactory.hpp"
#include "../utils/strconv/StringConverter.hpp"

// #include "../utils/debug/Oops.hpp"

#include <cfloat>

using std::cerr;
using std::ostringstream;

// see 'CheckForUserDefinedCondition'
map<string,ConditionsChecker::CheckConditionFunction*>
ConditionsChecker::checkConditionFunctionsMap;


const char * ConditionsChecker::key = "CONDITIONS_CHECKER_KEY";

/* *********************************************************************** */
ConditionsChecker::
Init::Init (ConditionsChecker & aOwner):
  IterTransition ("ConditionsChecker::Init"),
  owner (aOwner)
{}

void
ConditionsChecker::
Init::execute (IterData& iterData)
{
  owner.time = 0;
  owner.way = 0;


  for (list<Check*>::const_iterator i = owner.checkers.begin ();
       i != owner.checkers.end ();
       ++i)
  {
    (*i)->conditionFulfilled = false;
  } 

  for (list<Write*>::const_iterator i = owner.writers.begin ();
       i != owner.writers.end ();
       ++i)
  {
    (*i)->isValid = false;
  } 
}
      
/* *********************************************************************** */
ConditionsChecker::
Calculate::Calculate (ConditionsChecker & aOwner) :
  IterTransition ("ConditionsChecker::Calculate"),
  owner (aOwner)
{}


void
ConditionsChecker::
Calculate::execute (IterData& iterData)
{
  DynSysData& dynSysData = iterData.dynSysData;

  // time: 
  ++(owner.time);

  // way:
  real_t s = 0;
  for (int i=0; i<dynSysData.getStateSpaceDim (); ++i)
    s += sq(dynSysData.orbit[0][i] - dynSysData.orbit[-1][i]); 

  owner.way += sqrt(s);
}
/* *********************************************************************** */
ConditionsChecker::
Check::
Check (ConditionsChecker & aOwner, 
       const string& type,
       ConditionsChecker::WriteTime* aWriteTime,
       ConditionsChecker::WriteWay* aWriteWay,
       ConditionsChecker::WriteSetting* aWriteSetting):
  IterTransition (type),
  owner (aOwner),
  writeTime (aWriteTime),
  writeWay (aWriteWay),
  writeSetting (aWriteSetting)
{}


void
ConditionsChecker::
Check::execute (IterData& iterData)
{
  if (conditionFulfilled) // already fulfilled -> nothing to do.
    return;

  // NaN check:
  //
  // If the trajectory diverges, the checks is assumed as failed.
  // For this reason, the 'conditionFulfilled' is set to true,
  // and in nexs steps nothing will be done. But the 'setOutputValues'
  // is not called, i.e. nothing will be saved.
  //
  
  DynSysData& dynSysData = iterData.dynSysData;

  for (int i = 0; i<dynSysData.getStateSpaceDim (); ++i)
  {
    if (fabs(dynSysData.orbit[0][i])>= DBL_MAX)
    {
      conditionFulfilled = true;
      return;
    }
  }

  if (check (dynSysData)) // condition fulfilled 
  {
    conditionFulfilled = true;
    setOutputValues ();
  }
}

void
ConditionsChecker::
Check::setOutputValues ()
{
  if (writeTime != NULL)
  {
    writeTime->isValid = true;
    writeTime->time = owner.time;
  }

  if (writeWay != NULL)
  {
    writeWay->isValid = true;
    writeWay->way = owner.way;
  }

  if (writeSetting != NULL)
  {
    writeSetting->isValid = true;
  }
}


/* *********************************************************************** */
ConditionsChecker::
CheckForGivenPoint::
CheckForGivenPoint (ConditionsChecker & aOwner,
		    Array<real_t>& aPoint,
		    ConditionsChecker::WriteTime* aWriteTime,
		    ConditionsChecker::WriteWay* aWriteWay,
		    ConditionsChecker::WriteSetting* aWriteSetting) :
  Check (aOwner, 
	 "ConditionsChecker::CheckForGivenPoint",
	 aWriteTime, aWriteWay, aWriteSetting),
  point (aPoint.getTotalSize ())
{
  point = aPoint; // array copy
}

bool
ConditionsChecker::
CheckForGivenPoint::check (DynSysData& data)
{
  for (int i = 0; i<data.getStateSpaceDim (); ++i)
  {
    if (fabs (data.orbit[0][i] - point[i]) > owner.cmpPrecision )
      return false; // use L_max norm
  }

  return true;
}

/* *********************************************************************** */
ConditionsChecker::
CheckForAnyFixedPoint::
CheckForAnyFixedPoint (ConditionsChecker & aOwner,
		       ConditionsChecker::WriteTime* aWriteTime,
		       ConditionsChecker::WriteWay* aWriteWay,
		       ConditionsChecker::WriteSetting* aWriteSetting):
  Check (aOwner, 
	 "ConditionsChecker::CheckForAnyFixedPoint",
	 aWriteTime, aWriteWay, aWriteSetting)
{}

bool
ConditionsChecker::
CheckForAnyFixedPoint::
check (DynSysData& data)
{

  for (int i = 0; i<data.getStateSpaceDim (); ++i)
  {

//        if (data.orbit[0][i] == 
//  	  std::numeric_limits<real_t>::quiet_NaN ())
//  	return false; // NaN reached;

    if (fabs (data.orbit[0][i] - data.orbit[-1][i]) > owner.cmpPrecision )
      return false; // use L_max norm
  }

  return true;
}

/* *********************************************************************** */
ConditionsChecker::
CheckForLeavingRectangleArea::
CheckForLeavingRectangleArea (ConditionsChecker & aOwner,
			      Array<Array<real_t> >& givenRanges,
			      ConditionsChecker::WriteTime* aWriteTime,
			      ConditionsChecker::WriteWay* aWriteWay,
			      ConditionsChecker::WriteSetting* aWriteSetting,
			      const string& name):
  Check (aOwner, 
	 name,
	 aWriteTime, aWriteWay, aWriteSetting)
{
  ranges.alloc (givenRanges.getTotalSize ());

  for (int i=0; i< ranges.getTotalSize (); ++i)
  {
    assert (givenRanges[i].getTotalSize () == 2);

    ranges[i].alloc (2);
    ranges[i] = givenRanges[i]; // array copy
  }
}

bool
ConditionsChecker::
CheckForLeavingRectangleArea::
check (DynSysData& data)
{
  for (int i = 0; i<data.getStateSpaceDim (); ++i)
  {
    if (data.orbit[0][i] < ranges[i][0])
    {
      return true; // less then min range
    }

    if (data.orbit[0][i] > ranges[i][1])
    {
      return true; // greater then max range
    }
  }
  return false;
}

/* *********************************************************************** */
ConditionsChecker::
CheckForStayInRectangleArea::
CheckForStayInRectangleArea (ConditionsChecker & aOwner,
			     Array<Array<real_t> >& givenRanges,
			     ConditionsChecker::WriteWay* aWriteWay,
			     ConditionsChecker::WriteSetting* aWriteSetting) :
  CheckForLeavingRectangleArea (aOwner, givenRanges,
				NULL, aWriteWay, aWriteSetting,
				"ConditionsChecker::CheckForStayInRectangleArea")
{
}

bool
ConditionsChecker::
CheckForStayInRectangleArea::
check (DynSysData& data)
{
  // dirty check for NaN
  for (int i = 0; i<data.getStateSpaceDim (); ++i)
  {
    if (!(data.orbit[0][i] < 0) &&
	!(data.orbit[0][i] >= 0) )
    {
      // cout << data.orbit[0] << ": NaN detected" << endl;
      return false;
    }
  }

  return ! CheckForLeavingRectangleArea::check (data);
}

/* *********************************************************************** */
ConditionsChecker::
CheckForLeavingCircleArea::
CheckForLeavingCircleArea (ConditionsChecker & aOwner,
			   real_t aRadius,
			   Array<real_t> & aCenter,
			   ConditionsChecker::WriteTime* aWriteTime,
			   ConditionsChecker::WriteWay* aWriteWay,
			   ConditionsChecker::WriteSetting* aWriteSetting):
  Check (aOwner, 
	 "ConditionsChecker::CheckForLeavingCircleArea",
	 aWriteTime, aWriteWay, aWriteSetting),
  radius (aRadius)
{
  center.alloc (aCenter.getTotalSize ());
  center = aCenter; // array copy
}

bool
ConditionsChecker::
CheckForLeavingCircleArea::
check (DynSysData& data)
{
  real_t sum = 0;
  for (int i = 0; i<data.getStateSpaceDim (); ++i)
  {
    sum += sq(data.orbit[0][i] - center[i]);
  }

  return (sum > sq(radius));
}


/* *********************************************************************** */
ConditionsChecker::
Write::Write (ConditionsChecker & aOwner,
	      const string& fileName,
	      const string& name,
	      ScanData& scanData) : 
  ScanTransition (name),
  owner(aOwner),
  isValid (false)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}
/* *********************************************************************** */
ConditionsChecker::
WriteTime::WriteTime (ConditionsChecker& aOwner,
		      DynSysData& dynSysData,
		      const string& fileName,
		      ScanData& scanData) : 
  Write (aOwner, fileName, "ConditionsChecker::WriteTime", scanData)
{  
  ContinuousDynSysData* cData = NULL;
  cData = dynamic_cast <ContinuousDynSysData*> ( &(dynSysData) );
  
  if (cData != NULL)
  {
    isContinuous = true;
    deltaT = cData->dt;
  }
  else
  {
    isContinuous = true;
    deltaT = 1.0;
  }

}

void
ConditionsChecker::
WriteTime::
execute (ScanData& scanData)
{
  if (! isValid) return;

  (*f) << scanData
       << " ";

  if (isContinuous)
    (*f) << time * deltaT;
  else 
    (*f) << time;

  (*f) << endl;
}

/* *********************************************************************** */
ConditionsChecker::
WriteWay::WriteWay (ConditionsChecker & aOwner,
		    const string& fileName,
		    ScanData& scanData) : 
  Write (aOwner, fileName, "ConditionsChecker::WriteWay", scanData)
{
}

void
ConditionsChecker::
WriteWay::
execute (ScanData& scanData)
{
  if (! isValid) return;

  (*f) << scanData
       << " " << way << endl;
}

/* *********************************************************************** */
ConditionsChecker::
WriteSetting::
WriteSetting (ConditionsChecker & aOwner,
	      const string& fileName,
	      ScanData& scanData) : 
  Write (aOwner, fileName, "ConditionsChecker::WriteSetting", scanData)
{
}

void
ConditionsChecker::
WriteSetting::
execute (ScanData& scanData)
{
  if (! isValid) return;

  (*f) << scanData << endl;
}


/* *********************************************************************** */
void
ConditionsChecker::
addConditionFunction (string name,
                      CheckConditionFunction* f)
{
  assert (f != NULL);

  checkConditionFunctionsMap[name] = f;
}


/* *********************************************************************** */
ConditionsChecker::
CheckForUserDefinedCondition::
CheckForUserDefinedCondition (ConditionsChecker & aOwner,
			      ConditionsChecker::CheckConditionFunction* aF,
			      ConditionsChecker::WriteTime* aWriteTime,
			      ConditionsChecker::WriteWay* aWriteWay,
			      ConditionsChecker::WriteSetting* aWriteSetting) :
  Check (aOwner, 
	 "ConditionsChecker::CheckForUserDefinedCondition",
	 aWriteTime, aWriteWay, aWriteSetting),
  f (aF)
{
}

bool 
ConditionsChecker::
CheckForUserDefinedCondition::
check (DynSysData& data)
{
  return f (data);
}



/* *********************************************************************** */
ConditionsChecker::
ConditionsChecker (ScanData& scanData,
		   Configuration& methodDescription,
		   MethodsData& methodsData):
  time (0),
  way (0.0),
  init (NULL),
  calculate (NULL)
{
  // parts which work in each case:
  init = new Init (*this);
  calculate = new Calculate (*this);

  if (methodDescription.checkForKey ("COMPARE_PRECISION_KEY"))
  {
    cmpPrecision = methodDescription.getReal ("COMPARE_PRECISION_KEY");
  }
  else
  { 
    cmpPrecision = 1.0e-8; // default value
  }


  // number of conditions to be checked within a single run:
  int conditionsNumber = 
    methodDescription.getInteger ("CONDITIONS_NUMBER_KEY");

  if (conditionsNumber < 1)
    std::cerr << "Invalid setting of the field '"
	      << methodDescription.getOriginalKey ("CONDITIONS_NUMBER_KEY")
	      << "' found! Value "
	      << conditionsNumber
	      << " can not be accepted, only positive values"
	      << " are admissible."
	      << endl << Error::Exit;


    // local variables. It is important, that these pointers are 
    // set to NULL, because it will be proved before its using
    // in specific 'Check' sub-classes.
  WriteTime* writeTime;
  WriteWay* writeWay;
  WriteSetting* writeSetting;

  // for all conditions:  
  for (int conditionIndex = 0; 
       conditionIndex< conditionsNumber; 
       ++conditionIndex)
  {
    // create Configuration containing a description for this condition: 
    string tmpStr = string ("CONDITION")
      + "[" + toString (conditionIndex) + "]";
    
    Configuration conditionDescription 
      = methodDescription.getSubConfiguration (tmpStr.c_str());

    writeTime = NULL;
    writeWay = NULL;
    writeSetting = NULL;

    // case 1:
    if (conditionDescription.checkForEnumValue 
	("CONDITION_TYPE_KEY", "ANY_FIXED_POINT_KEY") ) 
    {
      // create writers dependent on options setting.
      // Note: file names are for this specific type of checker
      if (conditionDescription.getBool ("SAVE_TIME_KEY"))
      {
	const string& fileName = conditionDescription.getString 
	  ("TIME_UNTIL_FIXED_POINT_FILE_KEY");

	writeTime = new WriteTime (*this,
				   scanData.iterData().dynSysData,
				   fileName, 
				   scanData);
	    
	writers.push_back (writeTime);
      }
	
      if (conditionDescription.getBool ("SAVE_WAY_KEY"))
      {
	const string& fileName = conditionDescription.getString 
	  ("WAY_UNTIL_FIXED_POINT_FILE_KEY");

	writeWay = new WriteWay (*this, fileName, scanData);
	    
	writers.push_back (writeWay);
      }

      if (conditionDescription.getBool ("SAVE_SETTING_KEY"))
      {
	const string& fileName = conditionDescription.getString 
	  ("SETTINGS_LEADING_TO_FIXED_POINT_FILE_KEY");
	writeSetting = new WriteSetting (*this, fileName, scanData);
	    
	writers.push_back (writeSetting);
      }

      if (writeTime    == NULL && 
	  writeWay     == NULL && 
	  writeSetting == NULL )
      {
	std::cerr << "Trying to create a checker for the "
		  << " confition with type '"
		  << conditionDescription.getOriginalKey 
	  ("ANY_FIXED_POINT_KEY")
		  << "' failure!" 
		  << endl
		  << "Neither '"
		  << conditionDescription.getOriginalKey
	  ("SAVE_TIME_KEY")
		  << "' nor '"
		  << conditionDescription.getOriginalKey 
	  ("SAVE_WAY_KEY")
		  << "' nor '"
		  << conditionDescription.getBool 
	  ("SAVE_SETTING_KEY")
		  << "' is active."
		  << endl << Error::Exit;
      }

      // create the checker.
      // Note: dependent on option setting some of writers can be NULL 
      CheckForAnyFixedPoint* checkForAnyFixedPoint
	= new CheckForAnyFixedPoint (*this,
				     writeTime,
				     writeWay,
				     writeSetting);
      checkers.push_back (checkForAnyFixedPoint);
    }
    // case 2:
    else if (conditionDescription.checkForEnumValue 
	     ("CONDITION_TYPE_KEY", "GIVEN_POINTS_KEY") ) 
    {	    
      // get points to be checked:
      Array<Array<real_t> > points;

      conditionDescription.getArray ("POINTS_KEY", points);

      int numberOfPoints = points.getTotalSize ();

      if (numberOfPoints < 1)
	std::cerr << "Trying to create a checker for the "
		  << " confition with type '"
		  << conditionDescription.getOriginalKey 
	  ("GIVEN_POINTS_KEY")
		  << "' failure!" 
		  << endl		
		  << "No points given!"
		  << endl << Error::Exit;

      int stateSpaceDim = 
	scanData.iterData().dynSysData.getStateSpaceDim ();

      if (points[0].getTotalSize () != stateSpaceDim)
	std::cerr << "Trying to create a checker for the "
		  << " confition with type '"
		  << conditionDescription.getOriginalKey 
	  ("GIVEN_POINTS_KEY")
		  << "' failure!" 
		  << endl		
		  << "The number of components ("
		  << points[0].getTotalSize () 
		  << ") is not equal to the state space dimension ("
		  << stateSpaceDim
		  << ")."
		  << endl << Error::Exit;

	    // create writers dependent on options setting.
	    // Note: file names are for this specific type of checker
	    // Note: checkers and writers will be created
	    // for each point separately
      bool timeOpt = 
	conditionDescription.getBool ("SAVE_TIME_KEY");
      bool wayOpt = 
	conditionDescription.getBool ("SAVE_WAY_KEY");
      bool settingOpt = 
	conditionDescription.getBool ("SAVE_SETTING_KEY");

      if ((!timeOpt) && (!wayOpt) && (!settingOpt) )
      {
	std::cerr << "Trying to create a checker for the "
		  << " confition with type '"
		  << conditionDescription.getOriginalKey 
	  ("GIVEN_POINTS_KEY")
		  << "' failure!" 
		  << endl
		  << "Neither '"
		  << conditionDescription.getOriginalKey
	  ("SAVE_TIME_KEY")
		  << "' nor '"
		  << conditionDescription.getOriginalKey 
	  ("SAVE_WAY_KEY")
		  << "' nor '"
		  << conditionDescription.getBool 
	  ("SAVE_SETTING_KEY")
		  << "' is active."
		  << endl << Error::Exit;
      }

      for (int indexOfThisPoint = 0; 
	   indexOfThisPoint < numberOfPoints; 
	   ++indexOfThisPoint)
      {
	// file names will be created:
	if (timeOpt)
	{
	  string s
	    = string (conditionDescription.getString 
		      ("TIME_UNTIL_GIVEN_POINT_FILE_KEY"))
	    + toString (conditionIndex)
	    + "_"
	    + toString (indexOfThisPoint)		    
	    + "." 
	    + conditionDescription.getString
	    ("FILE_EXT_KEY");
		    
	  writeTime = new WriteTime (*this, 
				     scanData.iterData().dynSysData,
				     s,
				     scanData);
		    
	  writers.push_back (writeTime);
	}
	
	if (wayOpt)
	{
	  string s
	    = string (conditionDescription.getString 
		      ("WAY_UNTIL_GIVEN_POINT_FILE_KEY"))
	    + toString (conditionIndex)
	    + "_"
	    + toString (indexOfThisPoint)
	    + "."
	    + conditionDescription.getString
	    ("FILE_EXT_KEY");
	  writeWay = new WriteWay (*this, s, scanData);
		    
	  writers.push_back (writeWay);
	}

	if (settingOpt)
	{
	  string s
	    = string (conditionDescription.getString
		      ("SETTINGS_LEADING_TO_GIVEN_POINT_FILE_KEY"))
	    + toString (conditionIndex)
	    + "_"
	    + toString (indexOfThisPoint)
	    + "."
	    + conditionDescription.getString
	    ("FILE_EXT_KEY");

	  writeSetting = 
	    new WriteSetting (*this, s, scanData);
		    
	  writers.push_back (writeSetting);
	}
		
	// create the checker.
	// Note: dependent on option setting some of writers 
	// can be NULL 
	CheckForGivenPoint* checkForGivenPoint
	  = new CheckForGivenPoint (*this,
				    points[indexOfThisPoint],
				    writeTime,
				    writeWay,
				    writeSetting);
	checkers.push_back (checkForGivenPoint);
      } // for (indexOfThisPoint=0,...)
    } // if ()
 
    // case 3:
    else if (conditionDescription.checkForEnumValue 
	     ("CONDITION_TYPE_KEY", "LEAVES_RECTANGLE_AREA_KEY") ) 
    {
      // create writers dependent on options setting.
      // Note: file names are for this specific type of checker

      if (conditionDescription.getBool ("SAVE_TIME_KEY"))
      {
	const string& fileName = conditionDescription.getString 
	  ("TIME_UNTIL_RECTANGLE_IS_LEAVED_FILE_KEY");

	writeTime = new WriteTime (*this,
				   scanData.iterData().dynSysData, 
				   fileName,
				   scanData);
	    
	writers.push_back (writeTime);
      }
	
      if (conditionDescription.getBool ("SAVE_WAY_KEY"))
      {
	const string& fileName = conditionDescription.getString 
	  ("WAY_UNTIL_RECTANGLE_IS_LEAVED_FILE_KEY");

	writeWay = new WriteWay (*this, fileName, scanData);
	    
	writers.push_back (writeWay);
      }

	
      if (conditionDescription.getBool ("SAVE_SETTING_KEY"))
      {
	const string& fileName = conditionDescription.getString 
	  ("SETTINGS_LEADING_FROM_RECTANGLE_FILE_KEY");

	writeSetting = new WriteSetting (*this, fileName, scanData);
	    
	writers.push_back (writeSetting);
      }

      if ( (writeTime    == NULL) && 
	   (writeWay     == NULL) && 
	   (writeSetting == NULL) )
      {
	std::cerr << "Trying to create a checker for the "
		  << " confition with type '"
		  << conditionDescription.getOriginalKey 
	  ("LEAVES_RECTANGLE_AREA_KEY")
		  << "' failure!" 
		  << endl
		  << "Neither '"
		  << conditionDescription.getOriginalKey
	  ("SAVE_TIME_KEY")
		  << "' nor '"
		  << conditionDescription.getOriginalKey 
	  ("SAVE_WAY_KEY")
		  << "' nor '"
		  << conditionDescription.getBool 
	  ("SAVE_SETTING_KEY")
		  << "' is active."
		  << endl << Error::Exit;
      }

      // ranges of the rectangle area:
      int stateSpaceDim = 
	scanData.iterData().dynSysData.getStateSpaceDim ();
	    
      Array<Array<real_t> > ranges (stateSpaceDim);
      for (int i=0; i < stateSpaceDim; ++i)
	ranges[i].alloc (2);
	    
      conditionDescription.getArray ("RANGES_KEY", ranges);

      // create the checker.
      // Note: dependent on option setting some of writers can be NULL 
 
      CheckForLeavingRectangleArea* checkForLeavingRectangleArea
	= new CheckForLeavingRectangleArea (*this,
					    ranges,
					    writeTime,
					    writeWay,
					    writeSetting);
	    
      checkers.push_back (checkForLeavingRectangleArea);

    } // if (...)
    // case 4:
    else if (conditionDescription.checkForEnumValue 
	     ("CONDITION_TYPE_KEY", "LEAVES_CIRCLE_AREA_KEY") ) 
    {
      // create writers dependent on options setting.
      // Note: file names are for this specific type of checker

      if (conditionDescription.getBool ("SAVE_TIME_KEY"))
      {
	const string& fileName = conditionDescription.getString 
	  ("TIME_UNTIL_CIRCLE_IS_LEAVED_FILE_KEY");

	writeTime = new WriteTime (*this,
				   scanData.iterData().dynSysData, 
				   fileName, 
				   scanData);
	    
	writers.push_back (writeTime);
      }
	
      if (conditionDescription.getBool ("SAVE_WAY_KEY"))
      {
	const string& fileName = conditionDescription.getString 
	  ("WAY_UNTIL_CIRCLE_IS_LEAVED_FILE_KEY");

	writeWay = new WriteWay (*this, fileName, scanData);
	    
	writers.push_back (writeWay);
      }

	
      if (conditionDescription.getBool ("SAVE_SETTING_KEY"))
      {
	const string& fileName = conditionDescription.getString 
	  ("SETTINGS_LEADING_FROM_CIRCLE_FILE_KEY");

	writeSetting = new WriteSetting (*this, fileName, scanData);
	    
	writers.push_back (writeSetting);
      }

      if ( (writeTime    == NULL) && 
	   (writeWay     == NULL) && 
	   (writeSetting == NULL) )
      {
	std::cerr << "Trying to create a checker for the "
		  << " confition with type '"
		  << conditionDescription.getOriginalKey 
	  ("LEAVES_CIRCLE_AREA_KEY")
		  << "' failure!" 
		  << endl
		  << "Neither '"
		  << conditionDescription.getOriginalKey
	  ("SAVE_TIME_KEY")
		  << "' nor '"
		  << conditionDescription.getOriginalKey 
	  ("SAVE_WAY_KEY")
		  << "' nor '"
		  << conditionDescription.getBool 
	  ("SAVE_SETTING_KEY")
		  << "' is active."
		  << endl << Error::Exit;
      }

      // ranges of the rectangle area:
      int stateSpaceDim = 
	scanData.iterData().dynSysData.getStateSpaceDim ();
	    
      Array<real_t> center (stateSpaceDim);
	    
      conditionDescription.getArray ("CENTER_KEY", center);

      real_t radius = conditionDescription.getReal ("RADIUS_KEY");

      // create the checker.
      // Note: dependent on option setting some of writers can be NULL 
 
      CheckForLeavingCircleArea* checkForLeavingCircleArea
	= new CheckForLeavingCircleArea (*this,
					 radius,
					 center,
					 writeTime,
					 writeWay,
					 writeSetting);
	    
      checkers.push_back (checkForLeavingCircleArea);

    } // if (...)

    // case 5:
    else if (conditionDescription.checkForEnumValue 
	     ("CONDITION_TYPE_KEY", "STAYS_IN_RECTANGLE_AREA_KEY") ) 
    {
      // create writers dependent on options setting.
      // Note: file names are for this specific type of checker

      if (conditionDescription.getBool ("SAVE_WAY_KEY"))
      {
	const string& fileName = conditionDescription.getString 
	  ("WAY_WITHIN_RECTANGLE_FILE_KEY");

	writeWay = new WriteWay (*this, fileName, scanData);
	    
	writers.push_back (writeWay);
      }

      if (conditionDescription.getBool ("SAVE_SETTING_KEY"))
      {
	const string& fileName = conditionDescription.getString 
	  ("SETTINGS_STAYING_IN_RECTANGLE_FILE_KEY");

	writeSetting = new WriteSetting (*this, fileName, scanData);
	    
	writers.push_back (writeSetting);
      }

      if ( (writeWay     == NULL) && 
	   (writeSetting == NULL) )
      {
	std::cerr << "Trying to create a checker for the "
		  << " confition with type '"
		  << conditionDescription.getOriginalKey 
	  ("STAYS_IN_RECTANGLE_AREA_KEY")
		  << "' failure!" 
		  << endl
		  << "Neither '"
		  << conditionDescription.getOriginalKey 
	  ("SAVE_WAY_KEY")
		  << "' nor '"
		  << conditionDescription.getBool 
	  ("SAVE_SETTING_KEY")
		  << "' is active."
		  << endl << Error::Exit;
      }

      // ranges of the rectangle area:
      int stateSpaceDim = 
	scanData.iterData().dynSysData.getStateSpaceDim ();
	    
      Array<Array<real_t> > ranges (stateSpaceDim);
      for (int i=0; i < stateSpaceDim; ++i)
	ranges[i].alloc (2);
	    
      conditionDescription.getArray ("RANGES_KEY", ranges);

      // create the checker.
      // Note: dependent on option setting some of writers can be NULL 
 
      CheckForStayInRectangleArea* checkForStayInRectangleArea
	= new CheckForStayInRectangleArea (*this,
					   ranges,
					   writeWay,
					   writeSetting);
	    
      checkers.push_back (checkForStayInRectangleArea);

    } // if (...)
    // case 6:
    else if (conditionDescription.checkForEnumValue 
	     ("CONDITION_TYPE_KEY", "USER_DEFINED_KEY") ) 
    {
      // create writers dependent on options setting.
      // Note: file names are for this specific type of checker

      if (conditionDescription.getBool ("SAVE_TIME_KEY"))
      {
	const string& fileName = conditionDescription.getString 
	  ("TIME_UNTIL_USER_DEFINED_CONDITION_FILE_KEY");

	writeTime = new WriteTime (*this,
				   scanData.iterData().dynSysData, 
				   fileName, 
				   scanData);
	    
	writers.push_back (writeTime);
      }

      if (conditionDescription.getBool ("SAVE_WAY_KEY"))
      {
	const string& fileName = conditionDescription.getString 
	  ("WAY_UNTIL_USER_DEFINED_CONDITION_FILE_KEY");

	writeWay = new WriteWay (*this, fileName, scanData);
	    
	writers.push_back (writeWay);
      }

      if (conditionDescription.getBool ("SAVE_SETTING_KEY"))
      {
	const string& fileName = conditionDescription.getString 
	  ("SETTINGS_FOR_USER_DEFINED_CONDITION_FILE_KEY");

	writeSetting = new WriteSetting (*this, fileName, scanData);
	    
	writers.push_back (writeSetting);
      }

      if ( (writeTime    == NULL) && 
	   (writeWay     == NULL) && 
	   (writeSetting == NULL) )
      {
	std::cerr << "Trying to create a checker for the "
		  << " confition with type '"
		  << conditionDescription.getOriginalKey 
	  ("USER_DEFINED_KEY")
		  << "' failure!" 
		  << endl
		  << "Neither '"
		  << conditionDescription.getOriginalKey 
	  ("SAVE_TIME_KEY")
		  << "' nor '"
		  << conditionDescription.getOriginalKey 
	  ("SAVE_WAY_KEY")
		  << "' nor '"
		  << conditionDescription.getBool 
	  ("SAVE_SETTING_KEY")
		  << "' is active."
		  << endl << Error::Exit;
      }
 
      if ( checkConditionFunctionsMap.empty() ) 
	{
	  cerr << "Error Message from the method '"
	       << (methodDescription.getParentConfiguration ())
	    .getOriginalKey (this->key)
	       << "': No user-defined condition functions registered. "
	       << "Please do it firstly (in your .cpp file, typically "
	       << "in the 'connect' routine), and after that "
	       << "you will be able to use user-defined cell counters."
	       << endl << Error::Exit;
	    }

	  // now we need a key, for which the function to be called
	  // is registered in the corresponding map:
	  string key = conditionDescription.getString ("NAME_KEY");

	  // is this key present in the map?
	  map<string,CheckConditionFunction*>::iterator k 
	    = checkConditionFunctionsMap.find (key);
	
	  // not present -> error message:
	  if (k == checkConditionFunctionsMap.end ()) 
	    {
	      cerr << "Error: User-defined condition function with the name'"
		   << key 
		   << "' not found."
		   << endl;

	      cerr << "the following functions are registered:" 
		   << endl;
    
	      for ( map<string,CheckConditionFunction*>::const_iterator i
		      = checkConditionFunctionsMap.begin();
		    i != checkConditionFunctionsMap.end();
		    ++i ) 
		{
		  cerr << "    " << i->first << endl;
		}	      

	      cerr << Error::Exit;
	    }	  


      CheckForUserDefinedCondition* checkForUserDefinedCondition
	= new CheckForUserDefinedCondition (*this,
					    checkConditionFunctionsMap[key],
					    writeTime,
					    writeWay,
					    writeSetting);
	    
      checkers.push_back (checkForUserDefinedCondition);
    } // if (...)
    // hier weitermachen

  } // for(int i=0,...)

}
/* *********************************************************************** */
void
ConditionsChecker::
connect (PrePostStateMachine& scanMachine, 
	 IterMachine& iterMachine, 
	 ScanData& scanData,
	 Configuration& methodDescription)
{
  iterMachine.pre.add (init); 
  iterMachine.addToIterLoop (calculate); 

  for (list<Check*>::const_iterator i = checkers.begin ();
       i != checkers.end ();
       ++i)
  {
    iterMachine.addToIterLoop (*i);
  } 

  for (list<Write*>::const_iterator i = writers.begin ();
       i != writers.end ();
       ++i)
  {
    scanMachine.transition.add (*i);
  }
}

/* *********************************************************************** */
ConditionsChecker::
~ConditionsChecker ()
{
  delete init;
  delete calculate;

  for (list<Check*>::iterator i = checkers.begin ();
       i != checkers.end ();
       ++i)
  {
    delete (*i);
    (*i)=NULL;
  }

  for (list<Write*>::iterator i = writers.begin ();
       i != writers.end ();
       ++i)
  {
    delete (*i);
    (*i)=NULL;
  }

}

bool
ConditionsChecker::
isPossible (ScanData& scanData)
{
  if (scanData.runMode == SERVER) 
    return false;
  return true;
}
