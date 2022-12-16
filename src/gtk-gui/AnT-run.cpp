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
 * $Id: AnT-run.cpp,v 1.24 2006/06/01 13:45:14 wackengg Exp $
 *
 */

#include "AnT-run.hpp"
#define DEBUG__ANT_RUN 0

#include "../engine/AnT-init.hpp"
#include "MainWindow.hpp"
#include "CustomConfigurators.hpp"

extern
Node* getDynSysNode_deepestInside (AbstractConfigurator* aConfigurator);

// extern
string runGuiKey ("Starting AnT...");

/* ********************************************************************** */

bool
isServerCreationCond ( AbstractConfigurator* aParentConfigurator,
		       const Node* aSpecNode )
{
  return ( getEnumValueFromParent ("mode", aParentConfigurator)
	   == "server" );
}

bool
fromSelectionCreationCond ( AbstractConfigurator* aParentConfigurator,
			    const Node* aSpecNode )
{
  return ( getEnumValueFromParent ("host", aParentConfigurator)
	   == "from_selection" );
}

bool
byNameCreationCond ( AbstractConfigurator* aParentConfigurator,
		     const Node* aSpecNode )
{
  return ( getEnumValueFromParent ("host", aParentConfigurator)
	   == "by_name" );
}

bool
points__CreationCond ( AbstractConfigurator* aParentConfigurator,
		       const Node* aSpecNode )
{
  return ( getEnumValueFromParent ("scan_fetch", aParentConfigurator)
	   == "points" );
}

bool
time__CreationCond ( AbstractConfigurator* aParentConfigurator,
		     const Node* aSpecNode )
{
  return ( getEnumValueFromParent ("scan_fetch", aParentConfigurator)
	   == "time" );
}

/** do not create, if the type of the dynamical system is
    'external_data' */
bool
system__CreationCond ( AbstractConfigurator* aParentConfigurator,
		       const Node* aSpecNode )
{
  const Node* const dynSysNode
    = getDynSysNode_deepestInside
    ( (MainWindow::AnT_GUI ()).mainWindowConfigurator.get () );

  const Node* const typeNode
    = ( dynSysNode->findNode ("type") );
  assert (typeNode != NULL);

  string typeStr;
  leafNodeTo (typeNode, typeStr);

  return (typeStr != "external_data");
}

// VA: depending on this string the widget for shared object name
// is set / unset sensitive
string usePreCompiledSystemString ("usePreCompiledSystem");

// VA:
/** template specialization */
template<>
void
updateEvent<usePreCompiledSystemString>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "system" ) {
      GuiController<runGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      break;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<runGuiKey>::updateConfigurators (activationSiblings);
  }
}


// VA:
/** member template specialization */
// static
template<>
void
GuiController<runGuiKey>::
setupUpdateFuncMap (map<const string, update_func_t*>& funcMap)
{
  funcMap[usePreCompiledSystemString] 
    = &(updateEvent<usePreCompiledSystemString>);
}

/** member template specialization */
// static
template<>
void
GuiController<runGuiKey>::
setupCreationCondFuncMap (map<const string, creation_cond_func_t*>& funcMap)
{
  funcMap["system"]
    = &system__CreationCond;

  funcMap["port"]
    = &isServerCreationCond;

#if 1
  funcMap["client[]"]
    = &doNotCreate;
#else /* this is temporarily commented out: */
  funcMap["client[]"]
    = &isServerCreationCond;

  funcMap["client[]::select_host"]
    = &fromSelectionCreationCond;

  funcMap["client[]::hostname"]
    = &byNameCreationCond;

  funcMap["client[]::domain"]
    = &byNameCreationCond;

  funcMap["client[]::points"]
    = &points__CreationCond;

  funcMap["client[]::time"]
    = &time__CreationCond;
#endif
}

/* ********************************************************************** */

SavingStatus
runAnTOnSaving ( AbstractConfigurator* theRunConfigurator,
		 SavingErrListType& errList )
{
#if DEBUG__ANT_RUN
  cout << "AnT_gui: starting AnT..." << endl;
#endif
  assert (theRunConfigurator->isRootConfigurator ());
  assert ( ((MainWindow::AnT_GUI ()).runConfigurator).get ()
	   == theRunConfigurator );

  AnT::configFileName () = (MainWindow::AnT_GUI ()).iniFile;
  /// @see onSaving__printAnTHeader

  const Node* const runConfig
    = theRunConfigurator->getConfig ();

  AnT::runmode ()
    = getEnumValueFromParent ("mode", theRunConfigurator);

  const Node* const serverPortNode
    = runConfig->findNode ("port");
  if (serverPortNode != NULL) {
    assert (AnT::runmode () == "server");
    leafNodeTo (serverPortNode, AnT::serverPort);
    cout << "AnT server port: "
	 << AnT::serverPort
	 << endl;
  }

  // The key usePreCompiledSystem is used to check if a parsed function
  // or a shared object is used for simulation
  const Node* const parsedFunctionNode
    = runConfig->getNode ("usePreCompiledSystem");

  string parserString;
  leafNodeTo (parsedFunctionNode, parserString);
  bool parsedFunction 
    = ! PreSemanticChecker::getBoolFromIniString (parserString);

  // If a parsed function is used, the shared object is ignored
  if (!parsedFunction){

    const Node* const systemFileNode
      = runConfig->findNode ("system");
    
    if (parsedFunctionNode != NULL){
      if (systemFileNode != NULL) {
	/*: otherwise, we have 'external_data' as system type... */
	leafNodeTo (systemFileNode, AnT::systemFileName ());
	if ((AnT::systemFileName ()).empty ()) {
	  string errorMsg
	    = string ("The system file is not set by now.\n")
	    + "Please fix this before starting AnT!\n"
	    + "If you defined a system function in\n"
	    + "the AnT-gui you have to change \n"
	    + "<use parsed system function> to ON\n";
	  
	  errList.push_back (errorMsg);
	  
	  return WARNING;
	}
      }
    }
  }
  
  const Node* const logNode
    = runConfig->getNode ("log");
  string logString;
  leafNodeTo (logNode, logString);
  AnT::writeLogFileOption
    = PreSemanticChecker::getBoolFromIniString (logString);

  assert (AnT::iniRoot == NULL);
  AnT::iniRoot
    = ((MainWindow::AnT_GUI ()).mainWindowConfigurator)
    ->releaseRootConfig ();

#if 1 // 0 /* commented out */
  /* hide the opened windows: */
  AbstractContainerConfigurator::hide (NULL, theRunConfigurator);
  gtk_widget_hide ((MainWindow::AnT_GUI ()).window);
#if DEBUG__ANT_RUN
  cout << "AnT-gui: windows closed." << endl;
#endif
#endif

  gtk_main_quit ();
  return DONE;
}

/** member template specialization */
// static
template<>
void
GuiController<runGuiKey>::
setupSavingFuncMap (map<const string, saving_func_t*>& funcMap)
{
  funcMap[GUI_ROOT_KEY]
    = &runAnTOnSaving;
}


NEW_ALLOCATED(AbstractConfigurator*)
  newSystemLibConfigurator ( AbstractConfigurator* aParentConfigurator,
			     const Node* aSpecNode,
			     const char* givenType )
{
  FileInputConfigurator* result
    = dynamic_cast<FileInputConfigurator*>
    ( FileInputConfigurator::customCreator ( aParentConfigurator,
					     aSpecNode,
					     givenType ) );

  string sharedLibPattern
    = string ("*.") + ANT_SHARED_LIB_EXT;

  gtk_file_selection_complete
    ( GTK_FILE_SELECTION (result->getFileDialog (true)),
      /*: true -> force rhs widegt creation, so 'fileDialog' is not
	NULL... */
      sharedLibPattern.c_str () );

  return result;
}

/** member template specialization */
// static
template<>
void
GuiController<runGuiKey>::
setupCustomConfiguratorCreatorFuncMap
( map<const string, custom_creation_func_t*>& funcMap )
{
  funcMap [ "system" ]
    = &newSystemLibConfigurator;
}
