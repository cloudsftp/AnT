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
 * $Id: symbolicimagegridboxmanager.hpp,v 1.5 2005/02/01 08:51:32 fundindy Exp $
 *
 */

#ifndef SYMBOLICIMAGEGRIDBOXMANAGER_HPP
#define SYMBOLICIMAGEGRIDBOXMANAGER_HPP

#include "symbolicimagelinearextendedindexboxmanager.hpp"
#include "linearextendedgridboxindexvaluemapper.hpp"
#include "gridboxscanpointcalculator.hpp"
#include "parametermodificationparser.hpp"

/**
    Manager for a n-dimensional grid box covering. This class initialises the
    index box manager and connects it with the GridBoxIndexValueMapper
    and the GridBoxScanPointCalculator. Furthermore it implements appropriate
    output formatters.

    This box manager can handle linear extensions if desired. The parse-method
    decides if the linear extension should be used and calls the correct init-method
    of a super-class.

    \sa GridBoxScanPointCalculator, GridBoxIndexValueMapper and SymbolicImageAbstractLayer
    
  *@author Danny Fundinger
  */


class SymbolicImageGridBoxManager : public SymbolicImageLinearExtendedIndexBoxManager  {

public:

  /**
     Base class for all output format classes. These classes can be connected
     via Indices with SymbolicImageOutputWriters to output data
     in an appropriate format.

     \sa SymbolicImageOutputWriter, SymbolicImageDefaultLayer
  */
  class  ValueOutputFormat: public SymbolicImageOutputFormat{

  public:
  
      /** Constructor. It needs the corresponding grid box manager object.*/
      ValueOutputFormat();

      /** Destructor. */
      virtual ~ValueOutputFormat();

      /**
        Writes the formatted data of a index box to the output stream.
      */
      virtual void writeOutput(ostream * fout, SymbolicImageIndexBox* pBox) = 0;

      /**
         Writes the formatted data of a box range object to the output stream.
      */
      virtual void writeOutput(ostream * fout, SymbolicImageIndexBoxRange* pBoxRange) = 0;

  };
 

  /**
      Object to format the box output. This one prepares the output
      of the grid box values and the grid box indices.

      @author Danny Fundinger
  */
  class  BoxValueOutputFormat: public ValueOutputFormat{

  public:

      /** Formatter name */
      static const char * key;

      /** Constructor. It needs the corresponding grid box manager object.*/
      BoxValueOutputFormat(SymbolicImageGridBoxManager* gridBoxManager);

      /** Destructor. */
      ~BoxValueOutputFormat();

      /**
       Parses the output format part of the covering section of the
       configuration file.

        @param numberOfStateDimensions the state space dimension of the system
      */
      bool parse(Configuration & boxFormatDescription);

      /**
        Writes the formatted data of a index box to the output stream.
      */
      void writeOutput(ostream * fout, SymbolicImageIndexBox* pBox);

      /**
          Not implemented for this kind of format. Produces error.
      */
      void writeOutput(ostream * fout, SymbolicImageIndexBoxRange* pBoxRange);

  protected:

       /**  The interpolation values for the output box values */
       Array< real_t > m_positionValues;

       /** Corresponding grid box manager object */
      SymbolicImageGridBoxManager*  m_pGridBoxManager;

      
  private:

    /** Should the index position be written? */
    bool m_bWriteBoxIndices;

    /** Should an interpolated value of the box be written? */
    bool m_bWriteBoxValues;

  };

   /**
      Object to format the box range output. It prepares the output
      of grid box ranges.

      @author Danny Fundinger
  */
  class  RangeValueOutputFormat: public ValueOutputFormat{

  public:

      /** Formatter name */
      static const char * key;

      /** Constructor. It needs the corresponding grid box manager object.*/
      RangeValueOutputFormat(SymbolicImageGridBoxManager* gridBoxManager);

      /** Destructor. */
      ~RangeValueOutputFormat();

      /**
       Parses the output format part of the covering section of the
       configuration file.
      */
      bool parse(Configuration & boxFormatDescription);

      /**
           Not implemented for this kind of format. Produces error.
      */
      void writeOutput(ostream * fout, SymbolicImageIndexBox* pBox);

      /**
        Writes the formatted data of a box range to the output stream.

        @param pBoxRange a box range object of type SymbolicImageIndexBoxRange
      */
      void writeOutput(ostream * fout, SymbolicImageIndexBoxRange* pBoxRange);

  private:

    /** Corresponding grid box manager object */
    SymbolicImageGridBoxManager*  m_pGridBoxManager;

    /** Should the index position be written? */
    bool m_bWriteRangeIndices;

    /** Should an interpolated value of the box be written? */
    bool m_bWriteRangeValues;

    /** Initialized with 1.0, needed to for correct interpolation of the maximum position */
    Array< real_t > m_maxPositionValues;
  };

  /**
    * methods name
    */
  static const char * key;

  /**
     Key of equidistant linear extension type.
  */
  static const char * equidistantLinearExtensionKey;

  /** Constructor. */
  SymbolicImageGridBoxManager();

  /** Destructor. */
  ~SymbolicImageGridBoxManager();

  /**
        Parsing function for initialisation of the grid box manager and to set all
        the variables needed by the index box manager it is derived from.

        In this function it is also decided if the linear extension will be applied.
        
        \sa SymbolicImageIndexBoxManager

        @param numberOfStateDimensions the state space dimension of the system
  */
  bool parse(Configuration& coverDescription, int stateSpaceDimensionSize);

   /** Checks if this manager has a formatter with the corresponding ID */
  bool hasFormatID(int ID);

  /**
    Implementation of the virtual method to write box data using the formatter
    with the specified ID.  pBox must be a SymbolicImageIndexBox-type object!!

    @param fout the output stream the data is written to
    @param pBox the box the data is taken from
    @param formatID the ID of the format object

   */
  void writeBoxData(ostream * fout, SymbolicImageAbstractBox * pBox, int formatID);

  /**
    Implementation of the virtual method to write box range data using the formatter
    with the specified ID.  pBoxRange must be a SymbolicImageIndexBoxRange type object !!

    @param fout the output stream the data is written to
    @param pBoxRange the box range object the data is taken from
    @param formatID the ID of the format object

   */
  void writeRangeData(ostream * fout, SymbolicImageAbstractBoxRange * pBoxRange, int formatID);

protected:

  /** The output formatters. */
  Array< ValueOutputFormat* > m_pOutputFormat;
};

#endif
