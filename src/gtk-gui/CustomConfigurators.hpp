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
 * $Id: CustomConfigurators.hpp,v 1.4 2004/02/27 21:24:28 wackengg Exp $
 *
 */

#ifndef CUSTOM_CONFIGURATORS_HPP
#define CUSTOM_CONFIGURATORS_HPP

#include "Configurators.hpp"
#include "GeneralWidgets.hpp"

class FileInputConfigurator
  : public AbstractConfigurator
{
private:
  GtkWidget* fileTextEntry;
  GtkWidget* fileDialog;

public:
  FileInputConfigurator ( RootConfigurator* aRootConfigurator,
			  const Node* aSpecNode );

public:
  /** if 'forceCreation' is 'true', then 'createRhsWidget' is called
      and the result is guaranteed to be valid (not 'NULL'). */
  GtkWidget* getFileDialog (bool forceCreation = false);

protected:
  /** redefine (abstract) */
  virtual
  Node* createConfig (const string& configKey);

private:
  static
  void fileDialogOkCallback (GtkButton* okButton, gpointer aConfigurator);

private:
  static
  void arrowCallback ( GtkButton* arrowButton,
		       gpointer aConfigurator );

protected:
  /** redefine (abstract) */
  virtual
  GtkWidget* createRhsWidget (const Node* realSpecNode);

protected:
  /** redefine (abstract) */
  virtual
  void saveConfigProtected ();

protected:
  /** redefine (abstract) */
  virtual
  void syncWidgetsProtected ( const AbstractConfigurator* originator,
			      SyncMode syncMode );

public:
  /** This satisfies the function type 'custom_creation_func_t' and
      can be used accordingly in
      'GuiController<>::setupCustomConfiguratorCreatorFuncMap': */
  static
  NEW_ALLOCATED(AbstractConfigurator*)
    customCreator ( AbstractConfigurator* aParentConfigurator,
		    const Node* aSpecNode,
		    const char* givenType );
}; /* class FileInputConfigurator */

/* ********************************************************************** */

class CustomSelectorConfigurator
  : public AbstractConfigurator
{
public:
  ComboWidget selectionCombo;

public:
  CustomSelectorConfigurator ( RootConfigurator* aRootConfigurator,
			       const Node* aSpecNode );

protected:
  /** redefine (abstract) */
  virtual
  GtkWidget* createRhsWidget (const Node* realSpecNode);

public:
  /** redefine (abstract) */
  virtual
  Node* createConfig (const string& configKey);

protected:
  /** redefine (abstract) */
  virtual
  void saveConfigProtected ();

protected:
  /** redefine (abstract) */
  virtual
  void syncWidgetsProtected ( const AbstractConfigurator* originator,
			      SyncMode syncMode );

public:
  /** This satisfies the function type 'custom_creation_func_t' and
      can be used accordingly in
      'GuiController<>::setupCustomConfiguratorCreatorFuncMap': */
  static
  NEW_ALLOCATED(AbstractConfigurator*)
    customCreator ( AbstractConfigurator* aParentConfigurator,
		    const Node* aSpecNode,
		    const char* givenType );
}; /* class CustomSelectorConfigurator */

#endif
