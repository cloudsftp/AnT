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
 * $Id: MethodsPlugin.cpp,v 1.1 2004/08/18 14:13:12 rtlammer Exp $
 *
 */

#include "MethodsPlugin.hpp"
#include "AnT-init.hpp" /*: AnT::simulator needed... */


/* template specialization */
template<>
bool addMethodPlugin<ITER, PRE> (IterTransition* pluginTransition)
{
  assert (AnT::simulator != NULL);

  ((AnT::simulator)->iterPre).add (pluginTransition);

  return true;
}

/* template specialization */
template<>
bool addMethodPlugin<ITER, MAIN> (IterTransition* pluginTransition)
{
  assert (AnT::simulator != NULL);

  (((AnT::simulator)->iterLoop).methodPlugIns)
    .add (pluginTransition);

  return true;
}

/* template specialization */
template<>
bool addMethodPlugin<ITER, POST> (IterTransition* pluginTransition)
{
  assert (AnT::simulator != NULL);

  ((AnT::simulator)->iterPost).add (pluginTransition);

  return true;
}


/* template specialization */
template<>
bool addMethodPlugin<SCAN, PRE> (ScanTransition* pluginTransition)
{
  assert (AnT::simulator != NULL);

  ((AnT::simulator)->scanPre).add (pluginTransition);

  return true;
}

/* template specialization */
template<>
bool addMethodPlugin<SCAN, MAIN> (ScanTransition* pluginTransition)
{
  assert (AnT::simulator != NULL);

  ((AnT::simulator)->scanSeq).add (pluginTransition);

  return true;
}

/* template specialization */
template<>
bool addMethodPlugin<SCAN, POST> (ScanTransition* pluginTransition)
{
  assert (AnT::simulator != NULL);

  ((AnT::simulator)->scanPost).add (pluginTransition);

  return true;
}
