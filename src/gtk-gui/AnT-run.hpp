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
 * $Id: AnT-run.hpp,v 1.5 2005/04/08 17:35:58 rtlammer Exp $
 *
 */

#ifndef ANT_RUN_HPP
#define ANT_RUN_HPP

#include "GuiController.hpp"

extern string runGuiKey;

/** member template specialization */
// static
template<>
void
GuiController<runGuiKey>::
setupCreationCondFuncMap (map<const string, creation_cond_func_t*>& funcMap);

/** member template specialization */
// static
template<>
void
GuiController<runGuiKey>::
setupSavingFuncMap (map<const string, saving_func_t*>& funcMap);

/** member template specialization */
// static
template<>
void
GuiController<runGuiKey>::
setupCustomConfiguratorCreatorFuncMap
( map<const string, custom_creation_func_t*>& funcMap );

/** member template specialization */
// static
template<>
void
GuiController<runGuiKey>::
setupUpdateFuncMap (map<const string, update_func_t*>& funcMap);

#endif
