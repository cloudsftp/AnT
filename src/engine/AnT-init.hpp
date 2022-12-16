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
 * $Id: AnT-init.hpp,v 1.18 2006/10/02 12:30:37 rtlammer Exp $
 *
 */

#ifndef ANT_INIT_HPP
#define ANT_INIT_HPP

#include <string>
using std::string;

#ifndef ANT_CONFIG_H
#include "config.h"
#endif
#ifndef ANT_SHARED_LIB_EXT
#define ANT_SHARED_LIB_EXT "so"
#endif
#ifndef ANT_INI_FILE_EXT
#define ANT_INI_FILE_EXT "ant"
#endif

#include "simulators/AbstractSimulator.hpp"
#include "../utils/config/Configuration.hpp"
#include "../utils/matheval/MathEvalParser.hpp"
#include <map>
using std::map;
#include <vector>
using std::vector;

#define TRANSITIONS_LOG_FILE_NAME "transitions.log"

class IOStreamFactory; /* forward declaration */
extern IOStreamFactory *ioStreamFactory;

class ANPClient; /* forward declaration */
extern ANPClient *anpClient;

typedef void ConnectSystemFuncType ();
/* this function pointer gets initialized when loading the system */
extern ConnectSystemFuncType* connectSystemPtr;


class AnT {
public:
  static map<string, unsigned int>& stateVariableNames ();

  static vector<MathEvalParser*>& parsedEquationsOfMotion ();

  static string get__ANT_TOPDIR ();

  static string getGlobalKeysCfgFullPathName ();

  static string& systemFileName ();

  static string& systemName ();

  static string& configFileName ();

  static bool writeLogFileOption;

  static const Node* specRoot;
  static const Node* iniRoot;
  static const LinkNode* preSemanticRoot;

  static string& runmode ();


  static AbstractSimulator* simulator;

public:
  static string& serverHostName ();

  static   long serverPort;

  static   long numScanPoints;
  static   long nominalTime;

public:
  static void setDefaults ();

  static void run (bool call_createParseTrees);

private:
  static systemFunctionTreatment_t systemFunctionTreatment;

public:
  static bool setSystemFunctionTreatment (systemFunctionTreatment_t type);

public:
  static systemFunctionTreatment_t getSystemFunctionTreatment ();
};

class NormalExit
{};

class ANPClientExit
{};

void loadSystem ();

void printHeader (ostream& s);

void printUsage (const char* arg0);

void printUsageAndExit (const char* arg0);

string getVersionString ();

/** Create the parse trees - either from the configuration file or
     from the network. Thereafter, 'AnT::specRoot' and 'AnT::iniRoot'
     point to the corresponding trees, which are hopefully
     correct... */
void createParseTrees ();

/** evaluate command-line options */
void parseCommandLine (int argc, const char** argv);

void initSystem ();

void initSimulator ();

#endif

