/* 
 * Copyright (C) 1999-2004 the AnT project,
 * Department of Image Understanding,
 * University of Stuttgart, Germany.
 * 
 * Copyright (C) 2002-2004 Alexander Kramer. More info at http://www.it77.de/antvis2
 *
 * This file is part of AnTvis, an online visualization library, developed
 * primarily for AnT - a simulation and analysis tool for dynamical systems.
 * 
 * AnTvis is free software; you can redistribute it and/or modify
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
 * $Id: object.h,v 1.8 2005/04/15 06:25:50 rtlammer Exp $
 *
 */

#ifndef objectH
#define objectH
//---------------------------------------------------------------------------


#include <iostream>
#include <cassert>
#include <string>
using std::string;

#ifndef NULL
#define NULL 0
#endif

/* The following classes are from "glui/algebra3.h" */
/* (all members are initialized to 0.0 in constructor): */
class vec2;
class vec3;
class vec4;
class mat3;

typedef vec3 TColor;
typedef vec2 TVec2;
typedef vec4 TVec4;
typedef mat3 TMat3;
typedef vec3 TVec3;

class GLUI_Panel;
typedef GLUI_Panel TPanel;

class TGUI;

class TObject;
class TVisualObject;
class TDimensions;
class TRoundDimensions;
class TEvent;
class TAbstractDataSource;
class TAbstractVectorIterator;
class TAxis3D;
class TCamera2D;
class TCamera3D;
class TGUI;

class TListItem;
class TBaseList;
class TList;

class TSelectingList;
class TMesh3D;
class TProperty;
class TPropertyList;
class TScene;
class TSwitchingScene;

class TText;
class TWindow;
class TWindow3D;
class TWindow2D;

class TIterator2D;
class TDataSource3D;
class TDataSource2D;
class TBufferDimensions2D;

class TVisualization;
class TdmPoints3D;
class TdmLine3D;
class TColoringMethod3D;

class TLinearBuffer;
class TArrayBuffer;
class TBuffer;

class TAsciiFileIterator;
class TAnimator;
class TAbstractThread;
class TThread;
class TAbstractWaitCondition;
class TWaitCondition;

class TAbstractMutex;
class TMutex;


#define NO_NAME "<unnamed>"

/**
 * TObject is the base class of all objects in agl.
 * all instances are stored in a list and
 * deleted automaticaly upon exiting
 */
class TObject
{
private:
  TObject * _next;
  /**
   * Stores the object's name.
   */
  string name;

protected:
  friend class TObjectGarbageCollector;

public:
  /**
   * Creates a new object with a name.
   */
    TObject (const string& objname = NO_NAME);
    virtual ~TObject ();
  /**
   * Returns the name of the class.
   */
  string className () const;
  /**
   * Sets the name of the object.
   */
  void setName (const string& aName);
  /**
   * Returns the name of the object.
   */
  const string& getName () const;
  /**
   * status output.
   */
  void out (const string& s);

  /**
  *  debug output
  */
  void debug (const string& s);

  /**
   * Error output.
   */
  void err (const string& s);
  /**
   * Warning output.
   */
  void warn (const string& s);
};

void deleteAllObjects ();

#endif
