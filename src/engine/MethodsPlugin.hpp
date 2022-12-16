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
 * $Id: MethodsPlugin.hpp,v 1.1 2004/08/18 14:13:12 rtlammer Exp $
 *
 */

#ifndef METHOD_PLUGIN_HPP
#define METHOD_PLUGIN_HPP

#include "data/ScanData.hpp" /*: for 'ScanTransition' */
#include "utils/machines/IterMachine.hpp" /*: for 'IterTransition' */

/* ********************************************************************** */

enum MachineSpecification {ITER, SCAN};

/* there exist only specifications hereof (see below): */
template <const MachineSpecification machineSpec>
class MachineType;

/* template specialization */
template<>
class MachineType<ITER>
{
public:
  typedef IterTransition Type;
};

/* template specialization */
template<>
class MachineType<SCAN>
{
public:
  typedef ScanTransition Type;
};

/* ********************************************************************** */

enum PrePostPlacement {PRE, MAIN, POST};

/**
   Use the following template functions if you want to add your own
   method plugins to AnT...
 */
/* only declaration, we define specializations hereof later... */
template <const MachineSpecification machineSpec,
          const PrePostPlacement placement>
bool addMethodPlugin ( typename MachineType<machineSpec>
		       ::Type* pluginTransition );


/* template specialization */
template<>
bool addMethodPlugin<ITER, PRE> (IterTransition* pluginTransition);

/* template specialization */
template<>
bool addMethodPlugin<ITER, MAIN> (IterTransition* pluginTransition);

/* template specialization */
template<>
bool addMethodPlugin<ITER, POST> (IterTransition* pluginTransition);


/* template specialization */
template<>
bool addMethodPlugin<SCAN, PRE> (ScanTransition* pluginTransition);

/* template specialization */
template<>
bool addMethodPlugin<SCAN, MAIN> (ScanTransition* pluginTransition);

/* template specialization */
template<>
bool addMethodPlugin<SCAN, POST> (ScanTransition* pluginTransition);

#endif
