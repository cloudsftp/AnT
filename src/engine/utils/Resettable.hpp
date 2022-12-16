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
 * $Id: Resettable.hpp,v 1.2 2004/03/11 15:20:45 wackengg Exp $
 *
 */

#ifndef RESETTABLE_HPP
#define RESETTABLE_HPP

#include "utils/GlobalConstants.hpp" // real_t
#include "utils/machines/IterMachine.hpp" // real_t


/**
 * */
template <class ResettableObj_t>
class Resettable : public IterTransition 
{
private:
  ResettableObj_t& obj;

public:
    Resettable (ResettableObj_t& anObj,
		string name)
      : IterTransition (name),
	obj (anObj)
	{
	}

  virtual void execute (IterData&) 
  {
    obj.reset ();
  }

};

#endif
