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
 * $Id: PDE_1d_Simulator.cpp,v 1.5 2005/05/04 13:50:54 grosser Exp $
 *
 */

#include "PDE_1d_Simulator.hpp"
#include "iterators/ODE_Integrator.hpp"
#include "SpatialDiffOperators.hpp"
#include "../utils/strconv/StringConverter.hpp"
#include "proxies/PDE_1d_Proxy.hpp"

void PDE_1d_Simulator::
initDynamicalSystem (Configuration& dynSysDescription)
{
  Array<Array<real_t> > boundary (1);
  boundary[0].alloc (2);

  dynSysDescription.getArray ("DOMAIN_BOUNDARY_KEY", boundary);

  int numberOfCells = 
    dynSysDescription.getInteger ("NUMBER_OF_GRID_POINTS_KEY");

  if (boundary[0][0] >= boundary[0][1])
    {
      cerr << "Upper domain boundary (given value: "
	   << boundary[0][0]
	   << ") should be larger than the lower one (given value: "
	   << boundary[0][1]
	   << ")." 
	   << endl << Error::Exit;
    }

  if (numberOfCells <= 1)
    {
      cerr << "number of grid points (given value: "
	   << numberOfCells
	   << ") should be larger than one." 
	   << endl << Error::Exit;
    }


  int cellDim = dynSysDescription.getInteger ("STATE_SPACE_DIM_KEY");

  initialValuesResetter = new 
    InitialStates::SpaceDependent::CellularResetter (dynSysDescription, 
						     numberOfCells, 
						     cellDim);

  InitialStates::SpaceDependent::Initializer 
    initializer (*initialValuesResetter, 1, numberOfCells, cellDim);

  Configuration methodDescription = 
    dynSysDescription.getSubConfiguration ("INTEGRATION_METHOD_KEY");

  string name = dynSysDescription.getString ("SYSTEM_NAME_KEY");
  
  PDE_Data* newPDE_Data = new PDE_Data
    ( name,
      initializer, 
      dynSysDescription.getLong ("NUMBER_OF_ITERATIONS_KEY"),
      methodDescription.getReal ("STEP_KEY"),
      numberOfCells,
      boundary,
      cellDim );

  dynSysData = newPDE_Data;

  if (dynSysDescription.getBool ("DIRICHLET_BOUNDARY_CONDITIONS_KEY"))
    {
      proxy = new PDE_1d_Dirichlet_Proxy 
	(numberOfCells, cellDim, &(newPDE_Data->boundary));
    }
  else
    {
      proxy = new PDE_1d_Proxy 
	(numberOfCells, cellDim, &(newPDE_Data->boundary));
    }

  dynSysIterator = ODE_Integrator::get
    ( methodDescription, 
      *(static_cast<AbstractODE_Proxy*>(proxy)),
      *newPDE_Data );

  initDifferentialOperators (dynSysDescription);
}



void 
PDE_1d_Simulator::
initDifferentialOperators (Configuration& dynSysDescription)
{
  Configuration boundariesDescription = 
    dynSysDescription.getSubConfiguration ("BOUNDARY_CONDITIONS_KEY");

  if (boundariesDescription.checkForKey ("DEFAULT_POLICY_KEY"))
    {
      if (boundariesDescription.checkForEnumValue ("DEFAULT_POLICY_KEY",
						   "CYCLIC_KEY"))
      {
	defaultBoundaryPolicy = CYCLIC;
      }
      else if (boundariesDescription.checkForEnumValue ("DEFAULT_POLICY_KEY",
							"FLUXLESS_KEY"))
	{
	  defaultBoundaryPolicy = FLUXLESS;
	}
      else if (boundariesDescription.checkForEnumValue ("DEFAULT_POLICY_KEY",
							"INTERPOLATED_KEY"))
	{
	  defaultBoundaryPolicy = INTERPOLATED;
	}
      else
	cerr << "Wrong setting '"
	     << boundariesDescription.getEnum ("DEFAULT_POLICY_KEY")
	     << "' of the field '"
	     << boundariesDescription.getOriginalKey ("DEFAULT_POLICY_KEY")
	     << "'."
	     << endl << Error::Exit;
    }
  else
    {
      defaultBoundaryPolicy = INVALID;
    }

  // specific operators:
  if ( boundariesDescription.
       checkForKey ("NUMBER_OF_DIFFERENTIAL_OPERATORS_KEY"))
    {
      int numberOfDifferentialOperators = boundariesDescription.getInteger 
	("NUMBER_OF_DIFFERENTIAL_OPERATORS_KEY");

      for (int operatorNumber = 0; 
	   operatorNumber < numberOfDifferentialOperators;
	   ++operatorNumber)
	{
	  string tmpStr = "OPERATOR";
	  tmpStr = tmpStr + "[" + toString (operatorNumber) + "]";
      
	  Configuration operatorDescription = 
	    boundariesDescription.getSubConfiguration (tmpStr);      

	  string type (operatorDescription.getString ("NAME_KEY"));
      
	  if ( operatorDescription
	       .checkForEnumValue ("POLICY_KEY", "CYCLIC_KEY") )
	    {
	      boundaryPolicyMap() [type] = CYCLIC;
	    }
	  else if ( operatorDescription
		    .checkForEnumValue ("POLICY_KEY", "FLUXLESS_KEY") )
	    {
	      boundaryPolicyMap() [type] = FLUXLESS;
	    }
	  else if ( operatorDescription
		    .checkForEnumValue ("POLICY_KEY", "INTERPOLATED_KEY") )
	    {
	      boundaryPolicyMap() [type] = INTERPOLATED;
	    }
	  else if ( operatorDescription
		    .checkForEnumValue ("POLICY_KEY", "CONSTANT_KEY") )
	    {
	      boundaryPolicyMap() [type] = CONSTANT;

	      constantBoundaryPolicyMin() [type] = 
		operatorDescription.getReal ("LOWER_BOUNDARY_VALUE_KEY");

	      constantBoundaryPolicyMax() [type] = 
		operatorDescription.getReal ("UPPER_BOUNDARY_VALUE_KEY");
	    }
	  else
	    cerr << "Wrong setting '"
		 << operatorDescription.getEnum ("POLICY_KEY")
		 << "' of the field '"
		 << operatorDescription.getOriginalKey ("POLICY_KEY")
		 << "'."
		 << endl << Error::Exit;
      
	}// for (int operatorNumer = 0; ...
    }// if (...check ("NUMBER_OF ...
}
