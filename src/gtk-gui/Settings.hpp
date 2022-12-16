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
 * $Id: Settings.hpp,v 1.12 2009/01/22 15:59:39 schanzml Exp $
 *
 */

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

typedef long integer_t;

#include <climits> /* because of LONG_MAX, ... */
#define INTEGER_T_MAX LONG_MAX
#define INTEGER_T_MIN LONG_MIN
 
#include <cfloat>
#define REAL_T_MAX DBL_MAX
#define REAL_T_MIN DBL_MIN
// #define REAL_T_DIG DBL_DIG

/* ********************************************************************** */

#define DEFAULT_BOX_SPACING 10
#define DEFAULT_LABEL_PADDING 10

#define DEFAULT_TEXT_ENTRY_LENGTH 1024

/* ********************************************************************** */

#ifndef ANT_CONFIG_H
#include "config.h"
#endif
const string MAIN_WINDOW_TITLE
= string ("AnT ") + ANT_MAJOR + '.' + ANT_MINOR
#if 0 /* commented out */
+ ", Release " + ANT_SUB + ANT_REV
#endif
+ ", (c) " + ANT_YEARS
+ ", University of Stuttgart, IPVS";

// #define MAIN_WINDOW_WIDTH 640
#define MAIN_WINDOW_HEIGHT 400

#define IN_PLACE_CONFIGURATOR
#define MAIN_WINDOW_INITIAL_WIDTH 800
#define MAIN_WINDOW_INITIAL_HEIGHT 600

#define MAIN_WINDOW_BUTTON_WIDTH 80
#define MAIN_WINDOW_BUTTON_HEIGHT 40
#define MAIN_WINDOW_BUTTON_SPACING DEFAULT_BOX_SPACING

#define CONTAINER_CONFIGURATOR_ARROW_WIDTH 50

#endif
