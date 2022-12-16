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
 * $Id: GlobalConstants.hpp,v 1.13 2010/11/11 14:53:15 schanzml Exp $
 *
 */

#ifndef GLOBAL_CONSTANTS_HPP
#define GLOBAL_CONSTANTS_HPP

#include <iostream>
using std::cout;
using std::endl;
using std::cerr;
#include <string>
using std::string;

#include <cmath>
#include <cassert>

#include "../utils/CompilerDetector.hpp"

/* ******** OPTIONS MENU ********** */
//#define DEBUG_MODE_1
#define SECURE_DOWN_CAST
/* ******************************** */

/**
 * data type for all real-valued objects. 
 * @warning a problem occures, if you change it from the current setting,
 * bacause the routines, which read something using C-function fscanf use
 * the %f string for data type specifying 
 */
typedef double real_t;

typedef long integer_t;

/**
 * data type for discrete time
 */
typedef long DiscreteTimeType;

/**
 * data type for continuous time. It will be used for such objects,
 * as \f$\Delta t\f$ - integration step and \f$\tau\f$ - delay time
 * @see the warning for the 'real_t' type
 */
typedef real_t ContinuousTimeType;

 /**
 * mode in which the simulator runs.
 */
typedef enum 
  { 
    /** 
     * server, which perform destribution of scan 
     * points among clients, summarize the results
     * and if needed perform some calculations with it.
     */
    SERVER,
    /**
     * client, which perform the most parts of calculation
     * on remote machines
     */ 
    CLIENT,
    /**
     * start all calculations on the local machine
     */
    STANDALONE,
    /**
     * a nested simulator within another simulator. will be needed
     * by some meta-systems, such as Poincare maps, induced by any other
     * system type, averaged stochastical systems, etc.
     */
    INSIDE,
    /**
     * a simulator used for initial state scans. needed by the
     * coexisting attractor scan.
     */
    ISS
  } runMode_t;


/**  
 * There are (atm) two types of treatment of the system function.
 * Using a compiled or parsed system function. In order to identify which 
 * kind of treatment we use, there exists an global variable called 
 * systemFunctionTreatment in GeneralConstant from type
 * systemFunctionTreatment_t 
 */
typedef enum
  {
    COMPILED,
    PARSED,
    UNDEFINED
  } systemFunctionTreatment_t;


#define sq(x)    ((x)*(x))

#define EULER     2.71828182845904523536
#define Pi        3.14159265358979323846
#define Pi_2      1.57079632679489661923
#define Pi_4      0.78539816339744830962
#define Two_Pi    6.28318530717958647692
#define Sqrt_2    1.41421356237309504880
#define Sqrt_3    1.73205080756887729353


#ifdef DEBUG_MODE_1
#define debugMsg1(s) cout << s << " (" << WhereAmI << ")" << endl
#define debugCode1(s); s 
#else
#define debugMsg1(s)
#define debugCode1(s);
#endif

#ifdef SECURE_DOWN_CAST
#define DOWN_CAST dynamic_cast
#else
#define DOWN_CAST static_cast
#endif

#define DESTRUCTOR_MESSAGE(X) cout << "destructing '" << #X << "' ..." << endl

#define WhereAmI __FILE__<<", "<<__LINE__<<": "

/**
 * equivalence of two strings (for instance for usage in factories)
 */
#define streq(s1,s2) (strcmp(s1,s2)==0)

/**
 * max lenght of file names (it means, without the current path, the
 * name only) 
*/
#define MAX_FILE_NAME_LENGTH 2048

#define OUTPUT_STREAM_PRECISION 15

#define NEW_ALLOCATED(Type) Type

#define OUTPUT_COMMENT_CHAR '#'

#endif
