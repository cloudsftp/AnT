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
 * $Id: MainWindow.hpp,v 1.18 2009/01/22 15:59:39 schanzml Exp $
 *
 */

#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include "CommonDefs.hpp"
#include "GeneralWidgets.hpp"
#include "SemanticCheck.hpp"

/* ********************************************************************** */

class MainWindow
{
public:
  std::auto_ptr<RootConfigurator> mainWindowConfigurator;
  std::auto_ptr<RootConfigurator> runConfigurator;

  GtkWidget* const window;

public:
  GtkWidget* const buttonsBar;
#ifdef IN_PLACE_CONFIGURATOR
  GtkWidget* logoFrame;
  GtkWidget* confFrame;
#endif

private:
  GtkWidget* iniFileSelector;

#ifdef IN_PLACE_CONFIGURATOR
public:
#else
private:
#endif
  GtkWidget* const newButton;
  GtkWidget* const loadButton;

public: /* needed in SemanticCheck.cpp: */
  GtkWidget* const editButton;
  GtkWidget* const runButton;

private:
  GtkWidget* const helpButton;
  GtkWidget* const quitButton;

public:
  string iniFile;

private:
  GtkWidget* getIniFileSelector ();

public:
  /* @return empty if succesfull, otherwise an appropriate error
     message */
  const string& setIniFile (const string& iniFileName);

private:
  /** called from 'newCallback', 'setIniFileCallback'. */
  static void resetMainWindowConfigurator (MainWindow* aMainWindow);

private:
  /** Callback */
  static void newCallback ( GtkButton* aNewButton,
			    gpointer aMainWindow );

public:
  /** Callback */
  static void setIniFileCallback ( GtkButton* selectorOkButton,
				   gpointer aMainWindow );

private:
  /** Callback */
  static void edit ( GtkButton* anEditButton,
		     gpointer aMainWindow );

private:
  /** Callback */
  static void run ( GtkButton* aRunButton,
		    gpointer aMainWindow );

private:
  /** Callback */
  static void help ( GtkButton* aHelpButton,
		     gpointer aMainWindow );

private:
  /** Callback (calls 'AbstractContainerConfigurator::quit') */
  static void quitCallback ( GtkButton* aQuitButton,
			     gpointer aMainWindow );

private:
  /** Callback (calls 'quitCallback') */
  static gint close ( GtkWidget* mainGtkWindow,
		      GdkEvent* event,
		      gpointer aMainWindow );

private:
  /** Helper function */
  static GtkWidget* getLogoFrame ();

private:
  /** @see MainWindow::AnT_GUI */
  MainWindow ();

protected:
  /** no implemntation: copy constructor disabled... */
  MainWindow (const MainWindow&);

  ~MainWindow ();

public:
  /** singleton template: we create a single static instance on first
      call and the result is always a reference to this instance... */
  static MainWindow& AnT_GUI ();

public:
  void show ();
};

#endif
