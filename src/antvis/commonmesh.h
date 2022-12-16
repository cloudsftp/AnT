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
 * $Id: commonmesh.h,v 1.5 2005/04/15 06:25:48 rtlammer Exp $
 *
 */


//---------------------------------------------------------------------------

#ifndef commonmeshH
#define commonmeshH
//---------------------------------------------------------------------------
#include "visualobject.h"
#include "algebra3_wrapper.h"
#include "dimensions.h"

/* forward class declaration: */
class GLUI_Spinner;

/**
*  this is the common super class for mesh and axis.
*
*/
class TCommonMesh:public TVisualObject
{
private:

protected:
  // for common routins, can be 3 or 2, default 3
  int dims;
  // for common routines, default = true
  bool showFontGui;

  string captionX;
  string captionY;
  string captionZ;

  TVec3 meshSteps;

  GLUI_Spinner *spinnerStepsX;
  GLUI_Spinner *spinnerStepsY;
  GLUI_Spinner *spinnerStepsZ;

  void recalcSteps ();



public:
  TCommonMesh ();
  virtual ~TCommonMesh ();

  // min, max are set in recalcSteps to dimension->min, ..->max
  TVec3 min;
  TVec3 max;

  int steps[3];
  int liveSteps[3];

  TRoundDimensions *dimensions;
  TText *text;
  int autostep;
  TColor color;
  float relativeFontSize;

  const string& getCaptionX ();
  const string& getCaptionY ();
  const string& getCaptionZ ();
  void setCaptionX (const string& caption);
  void setCaptionY (const string& caption);
  void setCaptionZ (const string& caption);

  virtual void guiInit (TGUI * gui, TPanel * panel);
  virtual void guiCallback ();
};

#endif
