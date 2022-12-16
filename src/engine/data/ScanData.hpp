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
 * $Id: ScanData.hpp,v 1.7 2010/11/11 14:53:15 schanzml Exp $
 *
 */

#ifndef SCAN_DATA_HPP
#define SCAN_DATA_HPP

#include <list>

#include "../utils/config/Configuration.hpp"
#include "utils/machines/StateMachine.hpp"
#include "utils/machines/IterMachine.hpp"
#include "utils/datareader/ExternalDataReader.hpp"

using std::istream;
using std::ostream;

class AbstractScanItem;

/**
 * Common interface for all kinds of scan. An abstract class.  
 */
class ScanData: public AbstractState
{
protected:
  /**
   * contains the dimension of the scan or 0 for poincare maps
   */
  int scanMode;

public:
  /**
   * is false as long as a scan is not completed and true after that.
   * will be set to false in constructor of ScanData and to true in
   * 'ScanItemSequence::standaloneScanNext', 
   * 'ScanItemSequence::netClientScanNext',
   * 'ScanItemSequence::netServerScanNext'.
   */
  bool finalFlag;

  /**
   * reference to iter data, used in several investigation methods
   * where data (orbit) from a ScanTransition will be used.
   */
  IterData* iterDataPtr;

  /**
   * Run mode, will be used for instance in several 'isPossible'
   * routines, if a specific investigation method is not avaliable
   * on server, etc.
   */
  runMode_t runMode;
  
  /**
   * The sole constructor of this class.
   * @param data& iterData 
   */
  ScanData (IterData* data);

  /**
   * Will be needed for using of this class in the scan machine.
   */
  virtual bool isFinal (AbstractTransition*);

  /**
   * calculate the next scan setting for which the 
   * iterations run must be performed.
   * Standalone Variant.
   */
  virtual void standaloneScanNext () = 0;

  /**
   * calculate the next scan setting for which the 
   * iterations run must be performed. 
   * Net Client Variant.
   */
  virtual void netClientScanNext () = 0;

  /**
   * calculate the next scan setting for which the 
   * iterations run must be performed. 
   * Net Server Variant.
   */
  virtual void netServerScanNext () = 0;

  /**
   * initialize the scan setting.
   */
  virtual void initialize (Configuration& ini) = 0;

  /**
   * reset the scan setting.
   */
  virtual void reset () = 0;

  virtual void operator>> (ostream&) const = 0;

  /**
   * output the current scan setting. Will be used in several
   * investigation methods.
   */
  friend ostream& operator<< (ostream& os, const ScanData& scanData);

  /**
   * the factory for 'ScanData'-objects!
   */
  static ScanData* create (Configuration&, IterData*, runMode_t);

  virtual int getScanMode () = 0;

private:
  void checkIterData () const
  {
    if (iterDataPtr == NULL) {
      cerr << "'ScanData::checkIterData': 'iterDataPtr' is NULL!"
	   << endl
	   << Error::Exit;
      throw this; /*: for safety reasons... */
    }
  }

public:
  IterData& iterData ()
  {
    checkIterData ();

    return (*iterDataPtr);
  }

public:
  const IterData& iterData () const
  {
    checkIterData ();

    return (*iterDataPtr);
  }
}; /* class ScanData */


/**
 * scans of this kind will be completed by varying of several values,
 * each orthogonal to each other. A sequence of 'AbstractScanItem'
 * will be proceeded via proceeding of all combinations of values from
 * all items in the sequence.  
*/
class ScanItemSequence: public ScanData
{
private:
  /**
   * add pointer to a scan item into sequence.
   * Will be called in 'ScanItemSequence::initialize'
   * @param item pointer to an AbstractScanItem to be added.
   */
  void add (AbstractScanItem* item);

protected:
  bool firstCall;

  bool inc ();
  void set ();

public:
  void reset ();

  typedef list<AbstractScanItem*> seq_t;
  seq_t sequence;

  ScanItemSequence (IterData* iterData);

  virtual void standaloneScanNext ();
  virtual void netClientScanNext ();
  virtual void netServerScanNext ();

  virtual void initialize (Configuration& ini);
  virtual void operator>> (ostream&) const;

  virtual int getScanMode ();

  void set (string& scanpoint);
  string* get ();

  friend class ANPServer;

  virtual ~ScanItemSequence ();
}; /* class ScanItemSequence */


/**
 * transition, which can be proceeded in the ScanMachine.
 */
class ScanTransition : public AbstractTransition
{
private:
  virtual void execute (AbstractState& s);

public:
  virtual void execute (ScanData&) = 0;
  ScanTransition (string aName);
}; /* class ScanTransition */


/** 
 * Common interface for all kinds of scan items. An abstract class.  
 */
class AbstractScanItem
{
public:
  virtual void operator>> (ostream&) const = 0;

  /**
   * Write the current state of the scan item to output stream.
   * Seems to be needed in the network variant, probably by server. 
   * Will be called in 'string* ScanItemSequence::get ()'
   */
  virtual void get (ostream& os) = 0;

  /**
   * Set the current state of the scan item from input stream.
   * 'void ScanItemSequence::set (string& scanpoint)'
   *
   * Seems to be needed in the network variant, probably by client. 
   */
  virtual void set (istream& is) = 0;

  virtual void initialize (Configuration& itemDescription) = 0;
  /**
   * set the values of the external objects, which will be managed
   * in the scan item to current values. Typical implementation
   * (from 'BasicScanItem'):
   *
   *   *objPtr = currentValue;
   *
   */
  virtual void set () = 0;

  /**
   * calculate the next setting of the scan item.
   */
  virtual bool inc () = 0;

  /**
   * calculate the previous setting of the scan item.
   */
  virtual bool dec () = 0;

  /**
   * calculate the current setting of the scan item, used by inc/dec ().
   */
  virtual void calc () = 0;

  /**
   * returns number of scan points to be proceeded within this scan item.
   * It is easy for sub-classes of IndexableScanItem, but can be
   * more sophisticated, if one implements something else.
   */
  virtual long getNumPoints () = 0;

  /**
   * reset the item to starting values. Will be called by using
   * of scan items in a sequence as soon as end of calculation
   * for any item is reached. Typical implementation
   * (from 'BasicScanItem'):
   *
   * index = 0;
   * currentValue = minValue;
   *
   */
  virtual void reset (void) = 0;

  /**
   * a virtual destructor will be really needed here!
   */
  virtual ~AbstractScanItem () {}

  /**
   * Factory for scan items.
   * @param scanDescription DataBase, containing descriptions of all 
   * scan items,
   * @param index index of the scan item to be returned, between zero
   * and (scanMode-1).
   */
  static AbstractScanItem* getItem (Configuration& scanDescription,
				    int index);
};


/**
 * Base class for all types of scan items, which possess
 * a fixed number \f$ N \f$ of scan points.
 */
class IndexableScanItem : public AbstractScanItem
{
protected:
  /**
   * number of points \f$ N \f$ to be done within the scan item
   */
  long numPoints;
  /**
   * index of the current point. Values between 0 and (\f$ N - 1 \f$)
   */
  long index;
  
public:
  virtual long getNumPoints ();
  virtual long getCurrentIndex ();
  virtual void setCurrentIndex (long newIndex);
  virtual bool inc ();
  virtual bool dec ();
  virtual bool check ();
  virtual ~IndexableScanItem () {}
};

/**
 * Base class for all types of scan items, which possess
 * a minimal and a maximal value of the scan parameter 
 * (\f$\sigma_{min}\f$ and \f$\sigma_{max}\f$), and 
 * produce a number N of scan values (equidistant or not), 
 * where the current value of the scan parameter is a function of
 * \f$\sigma_{min}\f$, \f$\sigma_{max}\f$ and N.
 */
template<typename ITEM_TYPE>
class BasicScanItem: public IndexableScanItem
{
protected:
  ITEM_TYPE currentValue; // this is calculated when calc() is called
  ITEM_TYPE  *objPtr; // this is set when set() is called
 
  ITEM_TYPE minValue;
  ITEM_TYPE maxValue;

public:
  virtual void operator>> (ostream& os) const;
  virtual void set ();

  virtual void get (ostream& os);
  virtual void set (istream& is);

  virtual void reset (void);
  virtual bool check ();
  virtual ~BasicScanItem ();
}; /* BasicScanItem */

class RealScanItem: public BasicScanItem<real_t>
{
protected:
  RealScanItem (Configuration& itemDescription);
    
public:
  virtual void initialize (Configuration& itemDescription);
  virtual ~RealScanItem ();
}; /* class RealScanItem */


class IntegerScanItem: public BasicScanItem<integer_t>
{
protected:
  IntegerScanItem (Configuration& itemDescription);

public:
  virtual void initialize (Configuration& itemDescription);
  virtual ~IntegerScanItem ();
}; /* class IntegerScanItem */


/**
 * Performs a scan with equidistant steps of a real variable. 
 * The value of the scan parameter \f$\sigma\f$ by the i-th step
 * is given by
 * \f[\sigma_i = \sigma_{min}+\frac{i}{N-1}
 * \left(\sigma_{max} - \sigma_{min}\right)\f]
 */
class RealLinearScanItem: public RealScanItem
{
private:
  real_t step;
  void calc ();

public:
  RealLinearScanItem (Configuration& itemDescription);
}; /* class RealLinearScanItem */


/**
 * Performs a scan with logarithmic steps of a real variable.
 * The value of the scan parameter \f$\sigma\f$ by the i-th step
 * is given by
 * \f[\sigma_i = \exp\left(\ln(\sigma_{min})+\frac{i}{N-1}
 * \left( \ln(\sigma_{max}) - \ln(\sigma_{min})\right) \right)\f]
 * Note: For minimal and maximal values of the scan parameters
 * only positive values are admissible.
 * All logarithms are natural.
 */
class RealLogarithmicScanItem: public RealScanItem
{
private:
  real_t step;
  void calc ();

public:
  RealLogarithmicScanItem (Configuration& itemDescription);
}; /* class RealLogarithmicScanItem */


/**
 * Performs a scan with equidistant steps of a integer variable. 
 * The value of the scan parameter \f$\sigma\f$ by the i-th step
 * is given by
 * \f[\sigma_i = \sigma_{min}+\frac{i}{N-1}
 * \left(\sigma_{max} - \sigma_{min}\right)\f]
 */
class IntegerLinearScanItem: public IntegerScanItem
{
private:
  long step;
  void calc ();

public:
  IntegerLinearScanItem (Configuration& itemDescription);
}; /* class IntegerLinearScanItem */


/**
 * Performs a scan with logarithmic steps of a integer variable.
 * The value of the scan parameter \f$\sigma\f$ by the i-th step
 * is given by
 * \f[\sigma_i = \left\lceil\exp\left(\ln(\sigma_{min})+\frac{i}{N-1}
 * \left( \ln(\sigma_{max}) - \ln(\sigma_{min})\right) \right)\right\rceil\f]
 * Note: For minimal and maximal values of the scan parameters
 * only positive values are admissible.
 * All logarithms are natural.
 */
class IntegerLogarithmicScanItem: public IntegerScanItem
{
private:
  /**
   * the step value is a real value. Only the resulting value
   * of the scan object will be rounded to an integer value.
   */
  real_t step;
  void calc ();

public:
  IntegerLogarithmicScanItem (Configuration& itemDescription);
}; /* class IntegerLogarithmicScanItem */

/**
 * perforn a scan in two-dimensional spaces.
 */
template<typename ITEM_TYPE>
class TwoDimensionalScanItem: public IndexableScanItem
{
protected:

  /**
   * Current value of the first scan object.
   * This will be calculated when inc() in a sub-class is called.
   */
  ITEM_TYPE currentValue1; 

  /**
   * Pointer to the first external object, which will be
   * managed here.
   * Its value will be set when set() is called
   */
  ITEM_TYPE  *objPtr1;

  /**
   * Current value of the second scan object.
   * This will be calculated when inc() in a sub-class is called.
   */
  ITEM_TYPE currentValue2;

  /**
   * Pointer to the second external object, which will be
   * managed here.
   * Its value will be set when set() is called
   */
  ITEM_TYPE  *objPtr2;
 
  /**
   * min value, which becames the first external object
   */ 
  ITEM_TYPE minValue1;
  /**
   * min value, which becames the second external object
   */ 
  ITEM_TYPE minValue2;
  /**
   * max value, which becames the first external object
   */ 
  ITEM_TYPE maxValue1;
  /**
   * max value, which becames the second external object
   */ 
  ITEM_TYPE maxValue2;

public:
  /**
   * write the contents of the both external objects to the stream
   */
  virtual void operator>> (ostream& os) const;

  /**
   * set the values 'currentValue1' and 'currentValue2'
   * into the corresponding external objects
   */
  virtual void set ();

  /**
   * reset the item: the current values are set to min values
   * and the index is set to zero.
   */
  virtual void reset (void);

  /**
   * check, whether the input values make sense. It will be proved,
   * that the number of points is greater then one and that the min
   * and max values are not the same. 
   */
  bool check ();

  /**
   * get the both current values and the current index into the stream.
   * I think, it could be needed for network managament...
   */
  virtual void get (ostream& os);

  /**
   * get the both current values and the current index into the stream.
   * I think, it could be needed for network managament...
   */
  virtual void set (istream& is);
};

/**
 * perforn a scan in two-dimensional spaces, where both components are
 * real-valued, and it will be scanned along a straight line. 
 */
class TwoDimensionalRealLinearScanItem: 
  public TwoDimensionalScanItem<real_t>
{
private:
  /**
   * step in the first direction
   */
  real_t step1;

  /**
   * step in the second direction
   */
  real_t step2;

  /**
   * calculate both current values
   * @warning be very careful by write of such routines!
   */
  void calc ();

public:

  TwoDimensionalRealLinearScanItem (Configuration& itemDescription);

  /**
   * initialize the contents of the scan item from the DataBase.
   * Get the following user defined values:
   * <UL>
   * <LI> minValue1 </LI>
   * <LI> maxValue1 </LI>
   * <LI> minValue2 </LI>
   * <LI> maxValue1 </LI>
   * <LI> numPoints </LI>
   * </UL>
   */
  virtual void initialize (Configuration& itemDescription);

  virtual ~TwoDimensionalRealLinearScanItem ();
}; /* BasicScanItem */



/**
 * perforn a scan in two-dimensional spaces, where both components are
 * real-valued, and it will be scanned along an elliptic curve.
 * The elliptic curve is defined by two co-ordinates of the center point,
 * two axis, and the min and max values of the angle. 
 * It means, the following values have to be defined by user:
 * <UL>
 * <LI> \f$x_0\f$, \f$y_0\f$ - co-ordinates of the center point</LI>
 * <LI> \f$r_x\f$, \f$r_y\f$ </LI> both axis of the ellipse
 * <LI> \f$\phi_{\min}\f$, \f$\phi_{\max}\f$ </LI> start and end angles
 * <LI> \f$N\f$ the number of angle samples</LI>
 * </UL>
 * Then the following calculation take place:
 * \f[\Delta \phi = \frac{\phi_{\max} - \phi_{\min}}{N-1}\f]
 * \f[x_i = x_0 + r_x 
 \sin \left(\frac{2\pi}{360.0}(\phi_{\min} + i \Delta\phi)\right)\f]
 * \f[y_i = y_0 + r_y 
 \cos \left(\frac{2\pi}{360.0}(\phi_{\min} + i \Delta\phi)\right)\f]
 * Note, that all angles are given in degrees (for reason of more readable 
 * input and output), but the calculation take place in radians.
 */
class TwoDimensionalRealEllipticScanItem: 
  public TwoDimensionalScanItem<real_t>
{
private:
    /**
     * start angle
     */
    real_t phiMin;

    /**
     * end angle
     */
    real_t phiMax;
    
    /**
     * angle step
     */
    real_t phiStep;

    /**
     * current value of the angle
     */
    real_t currentPhi;

    /**
     * first axis of the ellipse
     */
    real_t r1;

    /**
     * second axis of the ellipse
     */
    real_t r2;

    /**
     * center of the ellipse: first co-ordinate
     */
    real_t center1;

    /**
     * center of the ellipse: second co-ordinate
     */
    real_t center2;
    
    /**
     * calculate
     * @warning be very careful by write of such routines!
     */
    void calc ();

public:
  /**
   * write the contents of the both external objects and the current
   * angle to the stream. The corresponding routine of the base class
   * should be overloaded and called here.
   */
  virtual void operator>> (ostream& os) const;

  TwoDimensionalRealEllipticScanItem (Configuration& itemDescription);

  /**
   * reset the item. call the corresponding routine from the base
   * class and additionally reset the current value of the angle phi. 
   */
  virtual void reset ();

  /**
   * check, whether the input values make sense. 
   * @note the corresponding routine of the base class can not
   * be called here, because the min and max values of the external
   * objects, managed here, can be the same!
   */
  virtual bool check ();

  /**
   * initialize the contents of the scan item from the DataBase.
   * Get the following user defined values:
   * <UL>
   * <LI> phiMin </LI>
   * <LI> phiMax </LI>
   * <LI> numPoints </LI>
   * <LI> r1 </LI>
   * <LI> r2 </LI>
   * <LI> center1 </LI>
   * <LI> center2 </LI>
   * </UL>
   */
  virtual void initialize (Configuration& itemDescription);

  virtual ~TwoDimensionalRealEllipticScanItem ();
}; /* TwoDimensionalRealEllipticScanItem */



/**
 * The scan values will be read from an external file and hence can
 * form any arbitrary curve in the scan space. An arbitrary number of
 * objects can be varied. The user has to specify the number of
 * columns in the file, the indeces of columns which must be used as
 * scan objects and the names of objects to be varied.
 * 
 * @warning at the current stage, all objects are assumed to be real.
 *
 * @todo it should be redesigned in order to allow the integer-valued
 * objects also.
 * */
class FromFileScanItem 
  : public IndexableScanItem
{
private:
  /**
   * file name, set in constructor, needed by 'reset'
   * */
  string fileName;

  /**
   * number of columns inn the file, set in constructor, needed by 'reset'
   * */
  int numberOfColumns;

  /**
   * reads the external file.
   * */
  ExternalDataReader* externalDataReader;

  /**
   * pointers to objects to be varied
   * */
  Array<real_t*> objects;

  /**
   * current values of all scan objects
   * */
  Array<real_t> currentValues;

  /** 
   * indeces of columns in the data file, which contain the
   * settings for the i-th component of the scan vector 
   * */
  Array<int> dataColumns;

  void initExternalDataReader (string fileName, int numberOfColumns);

public:
  FromFileScanItem (Configuration& itemDescription);

  virtual void operator>> (ostream& os) const;
  virtual void set ();

  virtual void get (ostream& os);
  virtual void set (istream& is);

  virtual void reset (void);
  virtual bool check ();

  virtual void initialize (Configuration& itemDescription);
  virtual void calc ();
};

#endif
