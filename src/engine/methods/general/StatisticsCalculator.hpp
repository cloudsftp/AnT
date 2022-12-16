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
 * $Id: StatisticsCalculator.hpp,v 1.3 2004/03/12 12:51:38 vravrout Exp $
 *
 */

#ifndef STATISTICS_CALCULATOR_HPP
#define STATISTICS_CALCULATOR_HPP

#include "methods/MethodsData.hpp"
#include "utils/conditions/OutputConditions.hpp"
#include "utils/Averager.hpp"

/**
 * Simple statistics calculations. I.e.
 * <UL>
 * <LI> mean value of each variable \f$\overline{x} \f$
 * </LI>
 * <LI> standard deviation of each variable :
 * \f[ \sqrt{\frac{(\sum{x^2} - \frac{{(\sum x)}^2}{N})}{N}} \f]
 * </LI>
 * <LI> correlations between single state variables (for dynamical systems
 * with more then one state variable):
 * \f[\frac{N \sum{x y} - \sum x \sum y}
 *        {\sqrt{(N \sum{x^2}-{(\sum x)^2})(N \sum{y^2}-{(\sum y)^2}) }} \f]
 </LI>
 * </UL>
 */
class StatisticsCalculator : public InvestigationMethod
{
public:
  static const char * key;

  /**
   * will be calculated as
   * \f$\mbox{\tt sums}_i = \sum_{t=T_{begin}}^{t=T_{end}} x_i(t)\f$
   * where \f$T_{begin}\f$ means the transient time and
   * \f$T_{end}\f$ means the complete number of simulation step.
   */
  Array<Averager<real_t> > sums;

  /**
   * will be calculated as
   * \f$\mbox{\tt quadSums}_i = \sum_{t=T_{begin}}^{t=T_{end}} x^2_i(t)\f$
   * where \f$T_{begin}\f$ means the transient time and
   * \f$T_{end}\f$ means the complete number of simulation step.
   */
  Array<Averager<real_t> > quadSums;

  /**
   * will be calculated as
   * \f$\mbox{\tt products}_{i,j} = 
   * \sum_{t=T_{begin}}^{t=T_{end}} x_i(t) x_j(t)\f$
   * where \f$T_{begin}\f$ means the transient time and
   * \f$T_{end}\f$ means the complete number of simulation step.
   */
  Array<Array<Averager<real_t> > > products;

  /**
   * Class for initialization of StatisticsCalculator variables;
   * The arrays 'sums', 'quadSums' and 'products' will be set to zero.
   */
  class Init : public IterTransition
  {
  private: 
    StatisticsCalculator & owner;

  public:
    Init (StatisticsCalculator & aOwner);

    /**
     * Initialize the StatisticsCalculator
     * @param iterData iterData
     */
    virtual void execute (IterData& iterData);
  };

  /**
   * The values of arrays 'sums', 'quadSums' and 'products' 
   * will be calculated.
   * @see StatisticsCalculator::sums, StatisticsCalculator::quadSums,
   * StatisticsCalculator::products.
   */
  class Calculate : public IterTransition
  {
  private: 
    StatisticsCalculator & owner;

  public:
    Calculate (StatisticsCalculator & aOwner);

    /**
     *
     */
    virtual void execute (IterData& iterData);
  };

  /**
   * Output class for average values
   * \f$\overline{x} \f$
   */    
  class WriteAverage : public ScanTransition
  {
  private: 
    StatisticsCalculator & owner;
    ostream *file;
  public:
    WriteAverage (StatisticsCalculator & aOwner,
		  const string& fileName,
		  ScanData& scanData);

    /**
     *
     */
    virtual void execute (ScanData& scanData);
  };

  /**
   * Output class for the standard deviation
   * \f[ \sqrt{\frac{(\sum{x^2} - \frac{{(\sum x)}^2}{N})}{N}} \f]
   */    
  class WriteStandardDeviation : public ScanTransition
  {
  private: 
    StatisticsCalculator & owner;
    ostream *file;
  public:
    WriteStandardDeviation (StatisticsCalculator  & aOwner,
			    const string& fileName,
			    ScanData& scanData);

    /**
     *
     */
    virtual void execute (ScanData& scanData);
  };

  /**
   * Output class for the correlation coefficient
   * \f[\frac{N \sum{x y} - \sum x \sum y}
   *        {\sqrt{(N \sum{x^2}-{(\sum x)^2})(N \sum{y^2}-{(\sum y)^2}) }} \f]
   *
   * @warning it works only if state space dimenson is > 1
   * @todo a warning should be written. Is it checked by GUI?
   */    
  class WriteCorrelationCoefficient : public ScanTransition
  {
  private: 
    StatisticsCalculator & owner;
    ostream *file;
  public:
    WriteCorrelationCoefficient (StatisticsCalculator& aOwner,
				 const string& fileName,
				 ScanData& scanData);

    /**
     *
     */
    virtual void execute (ScanData& scanData);
  };

  Init* init;
  
  ConditionalTransition* calculateTransition;
  WriteAverage* writeAverage;
  WriteStandardDeviation* writeStandardDeviation;
  WriteCorrelationCoefficient* writeCorrelationCoefficient;
  
  /**
   *
   */
  StatisticsCalculator (ScanData & scanData,
		        Configuration & ini,
		        MethodsData& abstractMethodsData);

  /**
   *
   */
  void connect (PrePostStateMachine& scanMachine, 
		IterMachine& iterMachine, 
		ScanData& scanData,
		Configuration & methodsDescription);

  ~StatisticsCalculator ();

  /**
   * the method can not be applied for run mode 'SERVER'
   */
  static bool isPossible (ScanData& scanData);
};


#endif
