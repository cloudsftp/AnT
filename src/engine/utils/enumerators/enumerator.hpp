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
 * $Id: enumerator.hpp,v 1.5 2004/12/10 09:12:21 fundindy Exp $
 *
 */

#ifndef ENUMERATORS_HPP
#define ENUMERATORS_HPP

#include <map>
#include <vector>

using std::vector;
using std::map;

/**
    Enumerator is a template to create a enumerator object for several list-handling
    classes like vector and map. They should allow to traverse over all elements of a list
    without any particular knowledge about which kind of list the elements belong to.
    This object was constructed according to the specification of the Enumerator-class in Java
    and can be used like it.

  * @author Danny Fundinger
  */

template <typename ELEM_T>
class Enumerator{  

public:

    /** Constructor */
    Enumerator(){};
   
    /** Destructor */
    virtual ~Enumerator(){};

    /** Returns true if enumerator is not at the end of the list */
    virtual bool hasMoreElements() = 0;

    /** Resets the enumerator to the first element of the list */
    virtual void reset() = 0;

    /**
      Returns the focused element of the list. Afterwards, the enumerator focuses
      the next element of the list.

      @return NULL if the enumerator is at the end of the list.
    */
    virtual ELEM_T getNextElement() = 0;

    /** Total number of elements in the list. */
    virtual int size() = 0;

    virtual void close() = 0;

    virtual void deleteList() = 0;
};

/**
    Enumerator for a vector list type and elements of a special type. Also the
    iterator type for this vector type must be specified. The vector list type must
    correspond with the element type ELEM_T.

    \sa Enumerator
    
  * @author Danny Fundinger
  */
  
template <typename ELEM_T, typename LIST_T, typename ITER_T>
class VectorEnumerator : public Enumerator<ELEM_T>{

public:

   typedef ELEM_T ElementType;
   typedef LIST_T   ListType;
   typedef ITER_T   IterType;

   VectorEnumerator(ListType* vectorList){
      m_pVectorList = vectorList;
      reset();
    };

   ~VectorEnumerator(){};

   void reset(){
      // reset the iterator object.
      m_it = m_pVectorList->begin();
      m_end = m_pVectorList->end();
    };


    bool hasMoreElements(){
      return m_it != m_end;
    };

    int size(){
      return m_pVectorList->size();
    }

    void close(){
      m_it = m_end;
    };

    ElementType getAndEraseNextElement(){
      if( m_it != m_end ){
         ElementType elem = (*m_it);
         m_it = m_pVectorList->erase(m_it);
         return elem;
      }
      else return NULL;
    };

    ElementType getNextElement(){
      // return the next element and increase the iterator of not
      // at the end of the list.
      if( m_it != m_end ){
         ElementType elem = (*m_it);
         m_it++;
         return elem;
      }
      else return NULL;
    };

    void deleteList(){
      if(m_pVectorList) delete m_pVectorList;
      m_pVectorList = NULL;
    };
  
private:

  ListType* m_pVectorList;
  IterType m_it;
  IterType m_end;

};

/**
    Enumerator for a map list type and elements of a special type. Also the
    iterator type for this map type must be specified. The map list type must
    correspond with the element type ELEM_T.

    \sa Enumerator

  * @author Danny Fundinger
  */

template <typename ELEM_T, typename LIST_T, typename ITER_T>
class MapEnumerator : public Enumerator<ELEM_T>{

public:

   typedef ELEM_T ElementType;
   typedef LIST_T   ListType;
   typedef ITER_T   IterType;

   MapEnumerator(ListType* mapList){
      m_pMapList = mapList;
      reset();
    };

   ~MapEnumerator(){};

   void reset(){
      m_it = m_pMapList->begin();
      m_end = m_pMapList->end();
    };


    bool hasMoreElements(){
      return m_it != m_end;
    };

    int size(){
      return m_pMapList->size();
    }

    ElementType getAndEraseNextElement(){
      cerr << "Function getAndEraseNextElement() not implemented for hash maps."
      << endl << Error::Exit;

      return NULL;
    };
    
    ElementType getNextElement(){
      if( m_it != m_end ){
         ElementType elem = m_it->second;
         m_it++;
         return elem;
      }
      else return NULL;
    };

    void close(){
      m_it = m_end;
    };

    void deleteList(){
      if(m_pMapList) delete m_pMapList;
      m_pMapList = NULL;
    };
  

private:

  ListType* m_pMapList;
  IterType m_it;
  IterType m_end;

};


#endif
