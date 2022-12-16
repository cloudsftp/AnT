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
 * $Id: SVD_Calculator.cpp,v 1.12 2004/09/08 16:16:26 rtlammer Exp $
 *
 */

#include "SVD_Calculator.hpp"
#include "data/DynSysData.hpp"
#include "data/ScanData.hpp"
#include "methods/output/IOStreamFactory.hpp"


//#include "utils/debug/Oops.hpp"

const char* SVD_Calculator::key = "SVD_ANALYSIS_KEY";

SVD_Calculator::
SVD_Calculator (ScanData& scanData,
		Configuration& methodDescription,
		MethodsData& methodsData)
#if ANT_HAS_LIBLAPACK
    : calculate (NULL),
      writeEvalues (NULL),
      writeEvectors (NULL),
      writeCovMatrix (NULL)
{
  debugMsg1("'SVD_Calculator::SVD_Calculator' will be constructed!"); 

  // numPoints  
  numPoints = methodDescription.getInteger ("USING_LAST_POINTS_KEY");

  // usingPoints
      usingPoints = methodDescription.getInteger ("POINTS_STEP_KEY");

  if (numPoints < usingPoints)
      cerr << "The number of points("
	   << numPoints
	   << ") is less as the points step ("
	   << usingPoints
	   << ") voted for the singular value decomposition analysis."
	   << endl << Error::Exit;

  DynSysData& data = scanData.iterData().dynSysData;

  long stopTime = data.timer.getStopTime ();
  
  if (stopTime < numPoints)
      cerr << "The complete number of iterations ("
	   << stopTime
	   << ") is less as the number of points ("
	   << numPoints
	   << ") needed by the singular value decomposition analysis."
	   << endl << Error::Exit;

  data.orbit.leastSize (numPoints);
  
  jobu = 'A';
  jobvt = 'A';
  
  m   = data.getStateSpaceDim ();
  lda = m;
  ldu = m;
  n     = ((long) numPoints)  / ((long) usingPoints);
  ldvt  = n;
  lwork = 2*std::max(3*std::min(m,n)+std::max(m,n),5*std::min(m,n)-4);

  a  = (double *) malloc( m * n * sizeof(double));
  if (a == NULL)
    cerr << "can not allocate memory for 'a' in 'SVD_Calculator'" 
	 << endl << Error::Exit;
  
  s  = (double *) malloc( m * sizeof(double));
  if (s == NULL)
    cerr << "can not allocate memory for 's' in 'SVD_Calculator'" 
	 << endl << Error::Exit;
  
  u  = (double *) malloc( m * m * sizeof(double));
  if (u == NULL)
    cerr << "can not allocate memory for 'u' in 'SVD_Calculator'" 
	 << endl << Error::Exit;
  
  /*
    vt = NULL; 
  */
  vt  = (double *) malloc( n * n * sizeof(double));
  if (vt == NULL)
    cerr << "can not allocate memory for 'vt' in 'SVD_Calculator'" 
	 << endl << Error::Exit;
  
  work = (double *) malloc( lwork * sizeof(double));
  if (work == NULL)
    cerr << "can not allocate memory for 'work' in 'SVD_Calculator'" 
	 << endl << Error::Exit;
  
  
  // create 'sub-objects':
  calculate = new Calculate (*this, scanData, methodDescription);
  
  if ( methodDescription.getBool ("EVAL_KEY") )
    {
      writeEvalues = new WriteEvalues 
	(*this, methodDescription, scanData);
    }
  
  if ( methodDescription.getBool ("EVEC_KEY"))
    {
      writeEvectors = new WriteEvectors 
	(*this, methodDescription, scanData);
    }
  
  if ( methodDescription.getBool ("COV_MATRIX_KEY"))
    {
      writeCovMatrix = new WriteCovMatrix 
	(*this, methodDescription, scanData);
    }
}

#else

{
  cerr << "It seems to be, that you have no clapack installed. "
       << "For this reason I am afraid, that I can not perform "
       << "the required singular value decomposition analysis."
       << endl
       << Error::Exit;
}
#endif


#if ANT_HAS_LIBLAPACK

SVD_Calculator::
Calculate::
Calculate (SVD_Calculator & aOwner, 
	   ScanData& scanData,
	   Configuration& methodDescription) :
  IterTransition ("SVD_Calculator::Calculate"),
  owner (aOwner)
{
  debugMsg1("'SVD_Calculator::Calculate' will be constructed!"); 
}

// virtual 
void 
SVD_Calculator::
Calculate::
execute (IterData& iterData)
{
  if (! iterData.dynSysData.orbit.isComplete () ) 
    {
      cout << "SVD_Calculator WARNING: orbit is not complete. "
	   << "Calculation can not be performed."
	   << endl;
      
      return;
    }
  
  int i;
  int j;
  long k;
  real_t norm = sqrt(owner.numPoints);

  for (j = 0, k = 0;
       k > -owner.numPoints;
       ++j, k -= owner.usingPoints)
    {
      for(i=0; i < owner.m; i++)
	{
	  owner.a[ i*owner.n + j ] = iterData.dynSysData.orbit[k][i]/norm;
	}
    }

  dgesvd_(&owner.jobu, &owner.jobvt, &owner.m, &owner.n, owner.a, 
	  &owner.lda, owner.s, owner.u, &owner.ldu, owner.vt, 
	  &owner.ldvt, owner.work, &owner.lwork, &owner.info);

  if (owner.info != 0)
    cerr << "'SVD_Calculator::Calculate': dgesvd_ performed not correctly"
	 << endl << Error::Exit; 
  
}

//#####################################

SVD_Calculator::
WriteEvalues::
WriteEvalues ( SVD_Calculator & aOwner,
	       Configuration& methodDescription,
	       ScanData& scanData) :
  ScanTransition ("SVD_Calculator::WriteEvalues"),
  owner (aOwner)
{
  debugMsg1("'SVD_Calculator::WriteEvalues' will be constructed!"); 
  f = ioStreamFactory->getOStream
    ( methodDescription.getString ("EVAL_FILE_KEY"),
      &scanData);
}

// virtual 
void 
SVD_Calculator::
WriteEvalues::
execute (ScanData& scanData)
{
    for(int i=0; i < owner.m; i++)
    {
	(*f) << scanData 
	     << " " 
	     << owner.s[i];
    } 
    (*f) << endl;
}

//#####################################

SVD_Calculator::
WriteEvectors::
WriteEvectors (SVD_Calculator & aOwner,
	       Configuration& methodDescription,
	       ScanData& scanData ) :
  ScanTransition ("SVD_Calculator::WriteEvectors"),
  owner (aOwner)
{
  debugMsg1("'SVD_Calculator::WriteEvectors' will be constructed!"); 

  f = ioStreamFactory->getOStream
    ( methodDescription.getString ("EVEC_FILE_KEY"),
      &scanData);
}

// virtual 
void 
SVD_Calculator::
WriteEvectors::
execute (ScanData& scanData)
{
  for(int i=0; i < owner.m; i++)
    {
      for(int j=0; j < owner.m; j++)
	{
	  (*f) << scanData << " " << owner.u[i*owner.m+j];
	} 
      (*f) << endl;
    }
}

//#####################################

SVD_Calculator::
WriteCovMatrix::
WriteCovMatrix ( SVD_Calculator & aOwner,
		 Configuration& methodDescription,
		 ScanData& scanData ) :
  ScanTransition ("SVD_Calculator::WriteCovMatrix"),
  owner (aOwner)
{
  debugMsg1("'SVD_Calculator::WriteCovMatrix' will be constructed!");   
  f = ioStreamFactory->getOStream
    ( methodDescription.getString ("COV_MATRIX_FILE_KEY"),
      &scanData);
}

// virtual 
void 
SVD_Calculator::
WriteCovMatrix::
execute (ScanData& scanData)
{
  for(int i=0; i < owner.n; i++)
    {
      for(int j=0; j < owner.n; j++)
	{
	  (*f) << scanData << " " << owner.vt[i*owner.n+j];
	} 
      (*f) << endl;
    }
}
#endif

//#####################################

SVD_Calculator::
~SVD_Calculator ()
{
#if ANT_HAS_LIBLAPACK

  if (calculate != NULL)
    delete calculate;
  if (writeEvalues != NULL)
    delete writeEvalues;
  if (writeEvectors != NULL)
    delete writeEvectors;
  if (writeCovMatrix != NULL)
    delete writeCovMatrix;
#endif
}


void SVD_Calculator::connect (PrePostStateMachine& scanMachine, 
			      IterMachine& iterMachine, 
			      ScanData& scanData,
			      Configuration & ini)
{    	       
#if ANT_HAS_LIBLAPACK
  iterMachine.post.add (calculate);
  if (writeEvalues != NULL)
    scanMachine.transition.add (writeEvalues);
  if (writeEvectors != NULL)
    scanMachine.transition.add (writeEvectors);
  if (writeCovMatrix != NULL)
    scanMachine.transition.add (writeCovMatrix);
#endif
}

bool SVD_Calculator::isPossible (ScanData& scanData)
{
#if ANT_HAS_LIBLAPACK
  if (scanData.runMode == SERVER) 
    return false;
  return true;
#else
  return false;
#endif
}
