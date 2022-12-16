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
 * $Id: ProgressWriter.hpp,v 1.2 2004/03/11 15:20:47 wackengg Exp $
 *
 */

#ifndef PROGRESS_WRITER_H
#define PROGRESS_WRITER_H

#include "data/ScanData.hpp"

/**
 * already proceeded part of a scan (in percents) will be
 * writen to stdout.
 */
class ProgressWriter : public ScanTransition
{
private: 
  static const real_t percentStep;

  /**
   * number of points in the complete scan item sequence
   * (product of numPoints-values of all scan items in the
   * sequence). It will be assumed, that all these items are
   * of the type 'IndexableScanItem'.
   */
  long numPoints;
  /**
   * number of points which are to be proceeded before
   * the progress output is to be writen at the next time.
   */
  real_t nextOutput;
 
  ScanItemSequence* scanItemSequence;

public:
  ProgressWriter (ScanData& scanData);

  virtual void execute (ScanData& scanData);

  ~ProgressWriter ();

  /**
   * check, whether the progress output is possible.
   * currently three conditions will be proved: 'scanMode' must be
   * al least 1, scanData must be of type 'scanItemSequence' and
   * all scan items in it must be of type 'indexableScanItem'. 
   */
  static bool isPossible (ScanData& scanData);
};

#endif
