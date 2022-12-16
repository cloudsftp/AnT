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
 * $Id: Averager.hpp,v 1.2 2004/03/11 15:20:45 wackengg Exp $
 *
 */

#ifndef AVERAGER_HPP
#define AVERAGER_HPP

#include "utils/GlobalConstants.hpp" // real_t


/**
 * Simple averaging of input values. Each call of the add routine
 * increase the sum of the input values and increment the counter.
 * the get routine returns the mean (average) value of all inputs
 * since the last reset.
 *
 * @note the template parameter 'Type' is assumed to be a scalar
 * number type, such as 'int', 'double', etc. It has to support the
 * standard arithmetical operations '+', '/' and '=0'. Otherwise the
 * averaging may be difficult.
 * */
template <class Type>
class Averager 
{
private:
    Type sum;
    long counter;

public:
    Averager ()
	{
	    reset ();
	}

    void add (Type value)
	{
	    sum += value;
	    ++counter;
	}

    void reset ()
	{
	    // is works for int type as well as for double
	    sum = 0;
	    counter = 0;
	}

    real_t getAverage ()
	{
	    return ((real_t)sum) / ((real_t) counter); 
	}

    real_t getSum ()
	{
	    return (real_t) sum; 
	}

    long getNumber ()
	{
	    return counter; 
	}
};

#endif
