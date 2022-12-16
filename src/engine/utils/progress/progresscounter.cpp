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
 * $Id: progresscounter.cpp,v 1.3 2004/12/10 09:12:21 fundindy Exp $
 *
 */
 
#include "progresscounter.hpp"

ProgressCounter::ProgressCounter(int size){

  m_iTotalSize = size;
  if(m_iTotalSize == 0) m_iTotalSize = 1;
  m_iProgressCount = 0;
  m_iStepCount = 0;
  m_iStepSize = 0;
  m_iDiv = 0;

}
ProgressCounter::~ProgressCounter(){
}

/** No descriptions */
void ProgressCounter::init(){

  m_iProgressCount = 0;
  m_iStepSize = 10;
  m_iStepCount = 0;
  m_iDiv = m_iTotalSize / m_iStepSize;

  if( m_iDiv < 1000) m_iStepSize = 2;
  else if( m_iDiv > 10000) m_iStepSize = 100;

  m_iDiv = m_iTotalSize / m_iStepSize;
  calculateProgress();
}

bool ProgressCounter::increase(){

  if(m_iStepCount < m_iTotalSize){

    m_iStepCount++;
    calculateProgress();
    return true;
  }
  else return false;
}

bool ProgressCounter::increaseBy(int inc){

  if((m_iStepCount + inc) < m_iTotalSize){

    m_iStepCount += inc;
    calculateProgress();
    return true;
  }
  else if(m_iStepCount < m_iTotalSize){

    m_iStepCount = m_iTotalSize - 1;
    calculateProgress();
    return true;
  }
  else return false;
}

int ProgressCounter::getCount(){

  return m_iStepCount;
}

void ProgressCounter::reset(){

  init();
}


void ProgressCounter::calculateProgress(){

  if( m_iStepCount == 0) cout << "0% (0)" << endl;
  else if( m_iDiv == 0 ){}
  else if( (m_iStepCount % m_iDiv) == 0){
    m_iProgressCount += (100 / m_iStepSize);
    cout << m_iProgressCount << "% (" << m_iStepCount << ")" << endl;
  }
}

