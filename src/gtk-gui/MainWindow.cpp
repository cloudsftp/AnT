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
 * $Id: MainWindow.cpp,v 1.40 2009/01/22 15:59:39 schanzml Exp $
 *
 */

#define DEBUG__MAIN_WINDOW_CPP 0

#ifndef ANT_CONFIG_H
#include "config.h"
#endif

#include <fstream>
using std::ifstream;

#include "MainWindow.hpp"
#include "Settings.hpp"
#include "Callbacks.hpp"
#include "../utils/debug/Error.hpp"
#include "AnT-run.hpp" /* because of 'runGuiKey' */
#include "../engine/AnT-init.hpp" /* see MainWindow::AnT_GUI */

#ifdef IN_PLACE_CONFIGURATOR
#include <glib/gstdio.h>
#include "FrameManager.hpp"
#endif

// static
void
  MainWindow::
resetMainWindowConfigurator (MainWindow* aMainWindow)
{
  assert (aMainWindow != NULL);

  assert ( (aMainWindow->mainWindowConfigurator).get () != NULL );
  assert ( (aMainWindow->mainWindowConfigurator)->isRootConfigurator () );

  Node* rootConfig = (aMainWindow->mainWindowConfigurator)
    ->releaseRootConfig ();
  if (rootConfig != NULL) {
    delete rootConfig;
  }

  const Node* const rootSpec = ( (aMainWindow->mainWindowConfigurator)
      ->getRootConfigurator () )
    ->specNode;
  assert (rootSpec != NULL);

  (aMainWindow->mainWindowConfigurator)
    .reset ( RootConfigurator
        ::newRootConfigurator<AnTGuiKey> (rootSpec) );
}

// static
void
MainWindow::newCallback ( GtkButton* aNewButton,
    gpointer aMainWindow )
{
  MainWindow* thisInstance
    = static_cast<MainWindow*> (aMainWindow );

  resetMainWindowConfigurator (thisInstance);
  (thisInstance->mainWindowConfigurator)
    ->forceConfigSetting (false);

  /* After the configuration file is given,
     the 'edit' and 'run' buttons should be deactivated.
   */
  gtk_widget_set_sensitive (thisInstance->editButton, FALSE);
  gtk_widget_set_sensitive (thisInstance->runButton, FALSE);

#ifdef IN_PLACE_CONFIGURATOR
  gtk_widget_set_sensitive (thisInstance->newButton, FALSE);
  gtk_widget_set_sensitive (thisInstance->loadButton, FALSE);
  gtk_widget_hide (thisInstance->logoFrame);
  gtk_widget_show (thisInstance->confFrame);
#else
  /* Now, deactivate the entire 'buttonsBar': */
  gtk_widget_set_sensitive (thisInstance->buttonsBar, FALSE);
#endif

  AbstractContainerConfigurator
    ::openWindow ( NULL, /* or: 'aNewButton' */
        (thisInstance->mainWindowConfigurator).get () );
}

// static
void
MainWindow::edit ( GtkButton* anEditButton,
    gpointer aMainWindow )
{
  MainWindow* thisInstance
    = static_cast<MainWindow*> (aMainWindow );

#ifdef IN_PLACE_CONFIGURATOR
  gtk_widget_set_sensitive (thisInstance->editButton, FALSE);
  gtk_widget_set_sensitive (thisInstance->runButton, FALSE);
  gtk_widget_set_sensitive (thisInstance->newButton, FALSE);
  gtk_widget_set_sensitive (thisInstance->loadButton, FALSE);
  gtk_widget_hide (thisInstance->logoFrame);
  gtk_widget_show (thisInstance->confFrame);
#else
  /* Now, deactivate the entire 'buttonsBar': */
  gtk_widget_set_sensitive (thisInstance->buttonsBar, FALSE);
#endif

  AbstractContainerConfigurator
    ::openWindow ( NULL, /* or: 'anEditButton' */
        (thisInstance->mainWindowConfigurator).get () );
}

// static
void
MainWindow::quitCallback ( GtkButton* aQuitButton,
    gpointer aMainWindow )
{
  MainWindow* thisInstance
    = static_cast<MainWindow*> (aMainWindow);

  assert (thisInstance != NULL);

  AbstractContainerConfigurator::quit
    ( NULL, (thisInstance->mainWindowConfigurator).get () );
}

// static
gboolean
MainWindow::close ( GtkWidget* mainGtkWindow,
    GdkEvent* event,
    gpointer aMainWindow )
{
#if 1 /* quit immediately: */
  MainWindow* thisInstance
    = static_cast<MainWindow*> (aMainWindow);

  assert (thisInstance != NULL);

  AbstractContainerConfigurator::quitImmediately
    ( NULL, (thisInstance->mainWindowConfigurator).get () );
#else
  MainWindow::quitCallback (NULL, aMainWindow);
#endif

  return FALSE;
}

  const string&
MainWindow::setIniFile (const string& anIniFile)
{
  static string errorMsg;

  errorMsg = ""; /*: the empty string indicates succes... */
  ifstream f (anIniFile.c_str ());

  do { /* do ... while (false); */
    if (! f.is_open ()) {
      errorMsg
        = string("Initialization file '")
        + anIniFile
        + "' not found.\nPlease enter a correct file name.";
      break;
    }

    iniFile = anIniFile;
#if DEBUG__MAIN_WINDOW_CPP && 0
    cout << "Your configuration file: "
      << iniFile
      << endl;
#endif

    MainWindow::resetMainWindowConfigurator (this);

    try {
      mainWindowConfigurator
        ->setConfig (createParseTree (f));
    } catch (...) {
      errorMsg
        = string("An error occured while parsing the initialization file:\n'")
        + anIniFile
        + "'\nStarting with a new configuration...";
      break;
    }

    /* After the configuration file is given,
       the 'edit' and 'run' buttons can be activated.
     */
    gtk_widget_set_sensitive (editButton, TRUE);
    gtk_widget_set_sensitive (runButton, TRUE);
  } while (false);

  f.close ();

  return errorMsg;
}

void
  MainWindow::setIniFileCallback
( GtkButton* selectorOkButton, gpointer aMainWindow )
{
  MainWindow* w = static_cast<MainWindow*> (aMainWindow);

  const string iniFile
    = gtk_file_selection_get_filename
    ( GTK_FILE_SELECTION (w->iniFileSelector) );

  const string& iniFileErrorMsg
    = w->setIniFile (iniFile);

  if (! iniFileErrorMsg.empty ()) {
    GtkWidget* dialog
      = newMessageBox ( iniFileErrorMsg.c_str (),
          "Initialization file load error" );
    gtk_signal_connect
      ( GTK_OBJECT (dialog),
        "delete_event",
        GTK_SIGNAL_FUNC (gtk_widget_destroy),
        NULL );

    gtk_widget_show_all (dialog);

    /* do not hide the file selection window :*/
    gtk_signal_emit_stop_by_name ( GTK_OBJECT (selectorOkButton),
        "clicked" );
  }
}

GtkWidget* MainWindow::getIniFileSelector ()
{
  if (! iniFileSelector) {
    iniFileSelector
      = newFileDialog ("Load initialization file");

    gtk_signal_connect
      ( GTK_OBJECT (GTK_FILE_SELECTION (iniFileSelector)->ok_button),
        "clicked",
        GTK_SIGNAL_FUNC (MainWindow::setIniFileCallback),
        this );

    string configFilePattern
      = string ("*.") + ANT_INI_FILE_EXT;
    gtk_file_selection_complete ( GTK_FILE_SELECTION (iniFileSelector),
        configFilePattern.c_str () );
  }

  return iniFileSelector;
}

/** Callback */
void
  MainWindow::
help (GtkButton* aHelpButton, gpointer aMainWindow)
{
  static GtkWidget* dialog = NULL;

  if (dialog == NULL) {
    string helpText
      = getVersionString ()
      + ", (c) " + ANT_YEARS
      + ",\n"
      + "\nMichael Schanz,"
      + "\nViktor Avrutin,"
      + "\nRobert Lammert,"
      + "\nGeorg Wackenhut,\n"
      + "\nUniversity of Stuttgart, IPVS.\n\n"
      + "For further or more detailed help\nplease check the AnT URL:\n\n"
      + ANT_HOMEPAGE
      + "\n\n";
    dialog = newMessageBox (helpText.c_str (), "AnT help");
  }

  gtk_widget_show_all (dialog);
}

/** Callback */
// static
void
  MainWindow::
run (GtkButton* aRunButton, gpointer aMainWindow)
{
  MainWindow* thisInstance
    = static_cast<MainWindow*> (aMainWindow);
  assert (thisInstance == &(MainWindow::AnT_GUI ()));

  if ((thisInstance->runConfigurator).get () != NULL) {
    /*: this widget was opened already, we must delete the old widget
      and create a new one, since the old one may be not up-to-date
      anymore... */
    delete ((thisInstance->runConfigurator)->releaseRootConfig ());
    (thisInstance->runConfigurator).reset (NULL);
  }

  string runCfgFileName
    = AnT::get__ANT_TOPDIR () + "/share/AnT/gui/AnT-run.cfg";
  ifstream runCfgStream (runCfgFileName.c_str ());
  assert (runCfgStream);

  NEW_ALLOCATED(Node*) runSpecNode
    = createParseTree (runCfgStream);
  (thisInstance->runConfigurator)
    .reset ( RootConfigurator
        ::newRootConfigurator<runGuiKey> (runSpecNode) );
  (thisInstance->runConfigurator)->forceConfigSetting (false);
#if DEBUG__MAIN_WINDOW_CPP
  cout << "Opening run window..." << endl;
#endif

#ifdef IN_PLACE_CONFIGURATOR
  gtk_widget_set_sensitive (thisInstance->editButton, FALSE);
  gtk_widget_set_sensitive (thisInstance->runButton, FALSE);
  gtk_widget_set_sensitive (thisInstance->newButton, FALSE);
  gtk_widget_set_sensitive (thisInstance->loadButton, FALSE);
  gtk_widget_hide (thisInstance->logoFrame);
  gtk_widget_show (thisInstance->confFrame);
#endif
  AbstractContainerConfigurator
    ::openWindow (NULL, (thisInstance->runConfigurator).get ());
}

GtkWidget* MainWindow::getLogoFrame()
{
  int numLogos = 0;
  const gchar* fileName = NULL; // temporary file name string pointer
  string logoDirName = AnT::get__ANT_TOPDIR () + "/share/AnT/gui/logos";
  string logoName = "";
  string subTitle = "";
  
  GDir* logoDir = g_dir_open (logoDirName.c_str (), 0, NULL);

  // run through the directory once to count the logos..
  while ((fileName = g_dir_read_name (logoDir)) != NULL) {
    if (g_str_has_suffix (fileName, ".xpm")) {
      ++numLogos;
    }
  }
  
  // ..choose one of them..
  int chosenLogo = g_random_int_range (0, numLogos); // [0..numLogos) / [0..numLogos-1]

  // ..rewind..  
  g_dir_rewind (logoDir);

  // ..and run through a second time to get the name of the chosen logo
  while ((fileName = g_dir_read_name (logoDir)) != NULL) {
    if (g_str_has_suffix (fileName, ".xpm")) {
      if (chosenLogo == 0) {
        logoName = fileName;
        break;
      }
      
      --chosenLogo;
    }
  }

  g_dir_close (logoDir);

  GtkWidget* logoText = NULL;
  GtkWidget* logoFrame = gtk_vbox_new (FALSE, 0);

  GtkWidget* logoBox = gtk_vbox_new (FALSE, 5);
  GtkWidget* textBox = gtk_hbox_new (FALSE, 0);

  // construct the full path name of the logo to be displayed
  string logoFileName = logoDirName + "/" + logoName;
  
  // construct the full path name of the subtitle
  string titleFileName = logoDirName + "/" + logoName + ".txt";
  
  FILE *fp = g_fopen (titleFileName.c_str (), "r");
  
  if (fp != NULL) {
    char buffer[2048];
    
    while (fgets (buffer, sizeof (buffer), fp) != NULL) {
      subTitle += buffer;        
    }

    fclose (fp);

    logoText = gtk_label_new (subTitle.c_str ());    
  } else {
    logoText = gtk_label_new ((titleFileName + " not found").c_str ());      
  }

  gtk_label_set_justify (GTK_LABEL (logoText), GTK_JUSTIFY_RIGHT);

  GdkPixmap* gdkPixmap
    = gdk_pixmap_colormap_create_from_xpm ( NULL,
        gdk_colormap_get_system (),
        NULL,
        NULL,
        logoFileName.c_str () );

  GtkWidget* gtkPixmap = gtk_pixmap_new (gdkPixmap, NULL);

  gtk_box_pack_end (GTK_BOX (textBox), logoText, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (logoBox), gtkPixmap, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (logoBox), textBox, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (logoFrame), logoBox, TRUE, FALSE, 0);

  return logoFrame;
}

  MainWindow::MainWindow ()
  :  mainWindowConfigurator (NULL)
  , runConfigurator (NULL)
  , window (getDefaultToplevelWindow (MAIN_WINDOW_TITLE.c_str ()))
  , buttonsBar (gtk_vbutton_box_new ())
, iniFileSelector (NULL) // see #getIniFileSelector
  , newButton (gtk_button_new_with_label ("new")) // free?!
  , loadButton (gtk_button_new_with_label ("load"))
  , editButton (gtk_button_new_with_label ("edit"))
  , runButton  (gtk_button_new_with_label ("run "))
  , helpButton (gtk_button_new_with_label ("help"))
  , quitButton (gtk_button_new_with_label ("quit"))
{
  assert (AnT::specRoot != NULL);
  mainWindowConfigurator.reset
    ( RootConfigurator
      ::newRootConfigurator<AnTGuiKey> (AnT::specRoot) );

#if 0 /* commented out */
  gtk_signal_connect ( GTK_OBJECT (window),
      "delete_event",
      GTK_SIGNAL_FUNC (MainWindow::close),
      this );
#endif
  gtk_signal_connect ( GTK_OBJECT (window),
      "delete_event",
      GTK_SIGNAL_FUNC (stopDeleteEvent),
      this );

  gtk_signal_connect ( GTK_OBJECT (newButton),
      "clicked",
      GTK_SIGNAL_FUNC (MainWindow::newCallback),
      this );

  gtk_signal_connect ( GTK_OBJECT (editButton),
      "clicked",
      GTK_SIGNAL_FUNC (MainWindow::edit),
      this );

  gtk_signal_connect ( GTK_OBJECT (runButton),
      "clicked",
      GTK_SIGNAL_FUNC (MainWindow::run),
      this );

  gtk_signal_connect_object ( GTK_OBJECT (loadButton),
      "clicked",
      GTK_SIGNAL_FUNC (gtk_widget_show_all),
      GTK_OBJECT (getIniFileSelector ()) );

  gtk_signal_connect ( GTK_OBJECT (quitButton),
      "clicked",
      GTK_SIGNAL_FUNC (MainWindow::quitCallback),
      this );

  gtk_signal_connect ( GTK_OBJECT (helpButton),
      "clicked",
      GTK_SIGNAL_FUNC (help),
      this );


  GtkWidget* windowBox = gtk_hbox_new (TRUE, DEFAULT_BOX_SPACING);
  gtk_box_set_homogeneous ( GTK_BOX (windowBox), 
      FALSE );
  gtk_container_add (GTK_CONTAINER (window), windowBox);

#if 0 /* commented out */
  /* create the pixmap file name: */
  string logoFileName
    = AnT::get__ANT_TOPDIR () + "/share/AnT/gui/logo.xpm";

  /* logo pixmap: */
  GdkPixmap* gdkPixmap
    = gdk_pixmap_colormap_create_from_xpm ( NULL,
        gdk_colormap_get_system (),
        NULL,
        NULL,
        logoFileName.c_str () );

  GtkWidget* gtkPixmap = gtk_pixmap_new (gdkPixmap, NULL);

  GtkWidget* frame = gtk_frame_new (NULL);
  gtk_container_add (GTK_CONTAINER (frame), gtkPixmap);

  gtk_container_add ( GTK_CONTAINER (windowBox),
      frame );

  //      gtk_container_add ( GTK_CONTAINER (windowBox),
  //  			gtk_hseparator_new () );
#endif /* commented out */

  // buttons bar:
  gtk_button_box_set_child_size
    ( GTK_BUTTON_BOX (buttonsBar), 
      MAIN_WINDOW_BUTTON_WIDTH, 
      MAIN_WINDOW_BUTTON_HEIGHT );

  gtk_button_box_set_layout
    ( GTK_BUTTON_BOX (buttonsBar),
      GTK_BUTTONBOX_SPREAD );

#ifdef IN_PLACE_CONFIGURATOR
  gtk_box_pack_start (GTK_BOX (windowBox), buttonsBar, FALSE, FALSE, 0);
#else
  gtk_container_add ( GTK_CONTAINER (windowBox),
      buttonsBar );
#endif
  gtk_widget_set_usize ( buttonsBar,
      MAIN_WINDOW_BUTTON_WIDTH 
      + 2 * MAIN_WINDOW_BUTTON_SPACING,
      MAIN_WINDOW_HEIGHT );

  /* the insensitive made buttons will be reactivated after loading
     the corresponding configuration file */

  // The 'new' branch is deactivated until a logic for this 
  // branch is found!
  gtk_container_add ( GTK_CONTAINER (buttonsBar),
      newButton );

  gtk_container_add ( GTK_CONTAINER (buttonsBar),
      loadButton );
  gtk_container_add ( GTK_CONTAINER (buttonsBar),
      gtk_hseparator_new () );

  gtk_container_add ( GTK_CONTAINER (buttonsBar),
      editButton );
  gtk_widget_set_sensitive (editButton, FALSE);

  gtk_container_add ( GTK_CONTAINER (buttonsBar),
      runButton );
  gtk_widget_set_sensitive (runButton, FALSE);
  gtk_container_add ( GTK_CONTAINER (buttonsBar),
      gtk_hseparator_new () );

  gtk_container_add ( GTK_CONTAINER (buttonsBar),
      helpButton );

  gtk_container_add ( GTK_CONTAINER (buttonsBar),
      quitButton );


#ifdef IN_PLACE_CONFIGURATOR
  logoFrame = getLogoFrame();
  confFrame = gtk_hbox_new (FALSE, 0);

  if (logoFrame != NULL)  {
    gtk_box_pack_start (GTK_BOX (windowBox), logoFrame, TRUE, FALSE, 0);
  }

  gtk_box_pack_start (GTK_BOX (windowBox), confFrame, TRUE, TRUE, 0);
  
  gtk_window_set_default_size (GTK_WINDOW (window),
                               MAIN_WINDOW_INITIAL_WIDTH,
                               MAIN_WINDOW_INITIAL_HEIGHT);
  
  FrameManager::attach (GTK_CONTAINER (confFrame));
#else
  /* create the pixmap file name: */
  string logoFileName
    = AnT::get__ANT_TOPDIR () + "/share/AnT/gui/logo.xpm";

  /* logo pixmap: */
  GdkPixmap* gdkPixmap
    = gdk_pixmap_colormap_create_from_xpm ( NULL,
        gdk_colormap_get_system (),
        NULL,
        NULL,
        logoFileName.c_str () );

  GtkWidget* gtkPixmap = gtk_pixmap_new (gdkPixmap, NULL);

  GtkWidget* frame = gtk_frame_new (NULL);
  gtk_container_add (GTK_CONTAINER (frame), gtkPixmap);

  gtk_container_add ( GTK_CONTAINER (windowBox),
      frame );
#endif

  //      gtk_container_add ( GTK_CONTAINER (windowBox),
  //  			gtk_hseparator_new () );
}

MainWindow::~MainWindow ()
{
  assert (this == &(AnT_GUI ()));

  if (mainWindowConfigurator->hasConfig ()) {
#if DEBUG__MAIN_WINDOW_CPP
    cout << "'quit' clicked in the run section of AnT-gui..."
      << endl;
#endif
    assert (AnT::iniRoot == NULL);
    delete (mainWindowConfigurator->releaseRootConfig ());
  }

  if (runConfigurator.get () != NULL) {
    delete (runConfigurator->releaseRootConfig ());
    delete (runConfigurator->specNode);
  }
}

void MainWindow::show ()
{
  gtk_widget_show_all (window);
  gtk_widget_hide (confFrame);
}

// static
  MainWindow&
MainWindow::AnT_GUI ()
{
  static MainWindow result;

  return result;
}

