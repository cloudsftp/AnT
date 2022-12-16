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
 * $Id: symbolicimagemorsespectrumlayer.hpp,v 1.3 2004/03/22 13:19:04 wackengg Exp $
 *
 */
 
#ifndef SYMBOLICIMAGEMORSESPECTRUMLAYER_HPP
#define SYMBOLICIMAGEMORSESPECTRUMLAYER_HPP

#include "symbolicimagedefaultlayer.hpp"

/**

  Extends the default layer to a morse spectrum layer. This means
  this layer calculates the morse spectrum. All the operations
  for the Symbolic Image can be performed here as well, although
  most of them are not really useful. The box manager for this class
  must be linear extended.

  The main difference to the default layer is the usage of extended other
  classes (see init-function) and the changes in the buildEdges-function.

  Furthermore a MorseSpectrumOutputWriter was implemented.

  \sa SymbolicImageLinearExtendedIndexBoxManager, MorseSpectrumRecurrentCellSet,
  GreedyNonstationaryExponentsFinder.

  *@author Danny Fundinger
  */
  
class SymbolicImageMorseSpectrumLayer : public SymbolicImageDefaultLayer  {

public:

   /**
     Output writer which writes morse spectrum related information. This
     can be either the min/max or interpolated nonstationary exponents of the
     recurrent cell sets.

     @author Danny Fundinger
  */

   class  MorseSpectrumOutputWriter: public SymbolicImageOutputWriter{

      public:

        /** Key for periodic cell writer */
        static const char * key;

        /** Constructor. It needs the corresponding layer object. */
        MorseSpectrumOutputWriter(SymbolicImageMorseSpectrumLayer* morseLayer);

        /** Destructor. */
        ~MorseSpectrumOutputWriter();

        /** Parsing function */
        bool parse(Configuration & morseSpectrumWriterDescription, int numberOfSubdivisions);

      protected:

          /** Writes output */
          void writeOutput();

      private:

          /** The layer object of the writer. */
          SymbolicImageMorseSpectrumLayer* m_pMorseLayer;

          /** Write min values ? */
          bool m_bWriteMin;
          /** Write max values ? */
          bool m_bWriteMax;
          /** Write interpolated values ? */
          bool m_bWriteInterpolated;

          bool m_bWriteInvariantSet;

          bool m_bWritePeriodSize;
  };


    /** Key of SymbolicImageDefaultLayer. */
  static const char * key; 

  /** Constructor */
  SymbolicImageMorseSpectrumLayer(int numberOfSubdivisions);

  /** Destructor */
  ~SymbolicImageMorseSpectrumLayer();

  /**
     Initializes the layer. Use a SymbolicImageAbstractLinearExtendedBoxManager
     object for the boxManager.
     
  */
  bool init(SymbolicImageAbstractBoxManager * boxManager);

  /** The parsing function */
  bool parse(Configuration & coverDescription, int numberOfSubdivisions);

  bool isLinearExtensionActive();

protected:

  /**
      Overwrites the dummy function to add the MorseSpectrum OutputWriter.
  */
  virtual SymbolicImageOutputWriter* parseOutputWriter(Configuration & coverDescription, int numberOfSubdivisions);

  /**
     Overwrites and extends the buildEdges function to get the linear extended box mapping
     and add a framing to the cells.
  */
  void buildEdges();

  /**
     Overwrites and extends the locatePeriodic function to calculate the nonstationary exponents
     by class  GreedyNonstationaryExponentsFinder.

     \sa GreedyNonstationaryExponentsFinder
  */
  void locatePeriodic();

private:

  /**
    Additional init-function.
  */
  bool initExtended(SymbolicImageAbstractLinearExtendedBoxManager* extendedBoxManager);

private:

  /** The linear extended box manager */
  SymbolicImageAbstractLinearExtendedBoxManager* m_pLinearExtendedBoxManager;

};

#endif
