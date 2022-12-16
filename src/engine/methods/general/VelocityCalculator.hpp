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
 * $Id: VelocityCalculator.hpp,v 1.3 2004/03/12 12:51:39 vravrout Exp $
 *
 */

#ifndef VELOCITY_CALCULATOR_HPP
#define VELOCITY_CALCULATOR_HPP

#include "methods/MethodsData.hpp"

/**
 * Using an object of this class the velocity 
 * \f$\vec v\f$ of an orbit of an dynamical system
 * can be calculated.
 * For dynamical systems continuous in time the velocity
 * is given by the right-hand side of the system
 * equation: 
 * \f[\vec v (t) = \frac{d}{dt}\vec x (t) \f]
 * For dynamical systems discrete in time the velocity
 * is given by the increasing or decreasing of the 
 * state vector in each step:
 * \f[\vec v_n = \vec x_{n}-x_{n-1} \f]
 * Phase portraits \f[\vec x \times \vec v\f] 
 * and the average velocity 
 * \f[\vec v_{avg} = \frac{1}{N}\sum_{i=1}^{N}\vec v_i\f]
 * where \f$N\f$ is the number of the simulations steps
 * (excepting transient)
 * can be saved as well.
 */
class VelocityCalculator : public InvestigationMethod
{
public:
    /**
     * methods name */
    static const char * key;

    bool saveCurrentVelocity;
    bool saveAverageVelocity;
    bool savePhasePortrait;

    /**
     * the array will be set to zero vector in the 'Init::execute'
     * and calculated in each step in 'CalculateVelocity::execute'.
     */
    Array<real_t> currentVelocity;    

    /**
     * For the calculation of the average velocity after the
     * complete simulation run (in each scan step) the absolute
     * values of the velocity for each state variable will be
     * summarized.
     */
    Array<Averager<real_t> > summarizedVelocity;

    ScanData& scanData;
    
    /**
     * initialization of internal variables before each simulation run.
     */
    class Init : public IterTransition
    {
    private: 
	VelocityCalculator & owner;
    public:
	Init (VelocityCalculator & aOwner);
	virtual void execute (IterData& iterData);
    };


/**
 * calculation of the current velocity for dynamical systems 
 * continuous in time by call of the system function.
 * The velocity is given by the right hand side of the system
 * equation:
 * \f$\vec v (t) = \frac{d}{dt}\vec x (t) \f$
 */
    class CalculateVelocityForContinuousSystems : 
	public IterTransition
    {
    private:
	VelocityCalculator & owner;
    public:
	CalculateVelocityForContinuousSystems 
	(VelocityCalculator & aOwner);

	virtual void execute (IterData& iterData);
    };

/**
 * calculation of the current velocity for dynamical systems 
 * discrete in time.
 * The velocity is given by the increasing or decreasing of the 
 * state vector in each step:
 * \f$\vec v_n = \vec x_{n}-x_{n-1} \f$
 */
    class CalculateVelocityForDiscreteSystems : 
	public IterTransition
    {
    private:
	VelocityCalculator & owner;

    public:
	CalculateVelocityForDiscreteSystems  
	(VelocityCalculator & aOwner);

	virtual void execute (IterData& iterData);
    };

    /**
     * saving of the current velocity 
     */
    class WriteCurrentVelocity : public IterTransition
    {
    private: 
	VelocityCalculator & owner;
	ostream* f;
    
    public:
	WriteCurrentVelocity (VelocityCalculator & aOwner, 
			      const string& fileName,
			      ScanData& scanData);
	virtual void execute (IterData& iterData);
    };


    /**
     * saving of the phase portrait \f$\vec x (t) \times \vec v (t)\f$ 
     */
    class WritePhasePortrait : public IterTransition
    {
    private:
	VelocityCalculator & owner;
	ostream* f;

    public:
	virtual void execute (IterData& iterData);
	WritePhasePortrait (VelocityCalculator & aOwner, 
			    const string& fileName,
			    ScanData& scanData);
    };

/**
 *
 */
    class SummarizeCurrentVelocity : public IterTransition
    {
    private: 
	VelocityCalculator & owner;
    public:
	SummarizeCurrentVelocity (VelocityCalculator & aOwner);
	virtual void execute (IterData& iterData);
    };


/**
 *
 */
    class WriteAverageVelocity : public ScanTransition
    {
    private: 
	VelocityCalculator & owner;
	ostream* f;
    public:
	
	WriteAverageVelocity (VelocityCalculator & aOwner, 
			      const string& fileName,
			      ScanData& scanData);
	virtual void execute (ScanData& scanData);
    };


    Init * init;

    ConditionalTransition* calculateVelocity;
    ConditionalTransition* writeCurrentVelocity;
    ConditionalTransition* writePhasePortrait;

    SummarizeCurrentVelocity * summarizeCurrentVelocity;
    WriteAverageVelocity * writeAverageVelocity;


    /**
     * Sole constructor of this class. Creates an empty (not initialized
     * object).
     * @param scanData scanData
     */
    VelocityCalculator ( ScanData & scanData);

    /**
     * Initializes the VelocityCalculator
     * @param ini current (usre-defined) description for the
     * 'VelocityCalculator'
     * @param methodsData map with data of all active objects performing
     * several investigation methods. 
     * */
    void create ( Configuration& ini,
		  MethodsData& methodsData);

    /**
     *
     */
    void connect (PrePostStateMachine& scanMachine, 
		  IterMachine& iterMachine,
		  ScanData& scanData, 
		  Configuration & ini);

    virtual ~VelocityCalculator ();
 
    /**
     *
     */
    static bool isPossible (ScanData& scanData);
};

#endif
