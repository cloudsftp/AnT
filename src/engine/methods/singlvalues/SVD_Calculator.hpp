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
 * $Id: SVD_Calculator.hpp,v 1.6 2004/05/03 13:36:52 rtlammer Exp $
 *
 */

#ifndef SVD_CALCULATOR_HPP
#define SVD_CALCULATOR_HPP

#ifndef ANT_CONFIG_H
#include "config.h"
#endif

#include "methods/MethodsData.hpp"

using std::cerr;

#if ANT_HAS_LIBLAPACK
extern "C" 
{
int dgesvd_( char *jobu, char *jobvt, int *m, int *n,
	     double *a, int *lda, double *s, double *u, int *ldu,
	     double *vt, int *ldvt, double *work, int *lwork,
	     int *info);
}
#endif

/**
 * Constructor SVD_Calculator
 * 
 * @todo The logic of the usingPoints and numPoints variables 
 * is to be discussed.
 */
class SVD_Calculator : public InvestigationMethod
{
public:
  static const char * key;

  // wenn sie sonst wo nicht gebraucht werden -> 
  // lokale Vars vom Konstruktor  
  int numPoints;
  int usingPoints;
  
#if ANT_HAS_LIBLAPACK

  char jobu; 
  char jobvt; 
  int m; 
  int n; 
  double * a; 
  int lda; 
  double * s; 
  double * u; 
  int ldu;
  double * vt; 
  int ldvt; 
  double * work; 
  int lwork; 
  int info;
  
  
  class Calculate  : public IterTransition
  {
  private:
    SVD_Calculator & owner;
    
  public:
    Calculate (SVD_Calculator & aOwner, 
	       ScanData& scanData,
	       Configuration& methodsDescription);

    virtual void execute (IterData& iterData);
    // ~Calculate ();
  };

  class WriteEvalues : public ScanTransition
  {
  private:
    SVD_Calculator & owner;
    ostream* f;
      
  public:
    WriteEvalues ( SVD_Calculator & aOwner,
		   Configuration& methodDescription, 
		   ScanData& scanData);

    virtual void execute (ScanData& scanData);
  };


  class WriteEvectors : public ScanTransition
  {
  private:
    SVD_Calculator & owner;
    ostream* f;
      
  public:
    WriteEvectors ( SVD_Calculator & aOwner,
		    Configuration& methodDescription, 
		    ScanData& scanData );

    virtual void execute (ScanData& scanData);
  };


  class WriteCovMatrix : public ScanTransition
  {
  private:
    SVD_Calculator & owner;
    ostream* f;
      
  public:
    WriteCovMatrix ( SVD_Calculator & aOwner,
		     Configuration& methodDescription, 
		     ScanData& scanData );

    virtual void execute (ScanData& scanData);
  };


  Calculate*       calculate;
  WriteEvalues*    writeEvalues;
  WriteEvectors*   writeEvectors;
  WriteCovMatrix*  writeCovMatrix;

#endif

  SVD_Calculator ( ScanData& scanData,
		   Configuration& methodDescription,
		   MethodsData& methodsData);

  void connect (PrePostStateMachine& scanMachine, 
		IterMachine& iterMachine, 
		ScanData& scanData,
		Configuration & ini);

  virtual ~SVD_Calculator ();
  static bool isPossible (ScanData& scanData);
};

#endif
