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
 * $Id: ExternalDataSimulator.hpp,v 1.4 2005/07/22 09:12:02 vravrout Exp $
 *
 */

#ifndef EXTERNAL_DATA_SIMULATOR_HPP
#define EXTERNAL_DATA_SIMULATOR_HPP

#include "AbstractSimulator.hpp"
#include "utils/datareader/ExternalDataReader.hpp"

/**
 * Simulator for external data.  Data will be read from a file, and
 * interpreted as data from a discrete map.  
 * @note The number of
 * iterations given in the ini file is the number of data sets (lines)
 * to be read. If the file contains less then this number of lines,
 * all lines will be read, hence you can set the number of itertions
 * to a very large number for reading the file until end of file.  
 * @note integer and real numbers are accepted in the data file.
 * but internal we work only with real numbers. Another possibility
 * could be to use hybrid maps data, but it is a problem, if we have 
 * only integer data in the file. In this case the continuous state 
 * space dimension is zero, and I don't think, that hybrid maps 
 * can work with such orbits.
 */
class ExternalDataSimulator: public AbstractSimulator
{
private:
  /**
   * ExternalDataReader, reding the data from file
   */
  ExternalDataReader edr;
  /**
   * Initialize parts of the simulator, specific for ...:
   * <UL>
   *   <LI> ExternalDataReader
   *   <LI> external data proxy,
   *   <LI> map data,
   *   <LI> standard map iterator.
   * </UL>
   * @param dynSysDescription description of the map to be simulated.
   */
  virtual void initDynamicalSystem (Configuration& dynSysDescription);

};

#endif
