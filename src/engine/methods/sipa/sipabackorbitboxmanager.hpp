
#ifndef SIPABACKORBITBOXMANAGER_HPP
#define SIPABACKORBITBOXMANAGER_HPP

#include "../symimages/symbolicimagegridboxmanager.hpp"

/**
  *@author Danny Fundinger
  */
  
class SipaBackOrbitBoxManager : public SymbolicImageGridBoxManager  {

public: 

   /**
    * methods name
    */
  static const char * key;
  
  typedef vector<SymbolicImageAbstractBox*> SymbolicImageAbstractBoxVector;

  SipaBackOrbitBoxManager();

  ~SipaBackOrbitBoxManager();
  
  bool parse(Configuration & coverDescription, int stateSpaceDimensionSize);
  
  void getSolutionBox(SymbolicImageAbstractBoxVector & solBoxes);
  
  real_t getSolutionDistance(SymbolicImageAbstractBox * box, Array<real_t> & distPoint, SymbolicImageAbstractBoxVector & targetBoxes);

  void setNewSolutionPoint(Array<real_t> & point);

  void deleteMapPointer();

private:

  Array< real_t > m_positionValues;
  Array<real_t> m_rSolutionPoint;

};
#endif
