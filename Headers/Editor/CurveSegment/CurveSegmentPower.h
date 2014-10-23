/*!
 * \file CurveSegmentPower.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef CURVESEGMENTPOWER_H_
#define CURVESEGMENTPOWER_H_

#include "Editor/CurveSegment.h"

namespace OSSIA {
  
  template <typename T>
  class CurveSegmentPower : public CurveSegment<T> {
    
  public:
    
    // Constructors, destructor, assignment
    CurveSegmentPower(Curve<T>*);
    CurveSegmentPower(const CurveSegmentPower&);
    virtual ~CurveSegmentPower();
    CurveSegmentPower & operator= (const CurveSegmentPower&);
    
    // Computation
    virtual T valueAt(double) const override;
    
    // Curve segment type
    virtual typename CurveSegment<T>::CurveSegmentType getType() const override final
    {return CurveSegment<T>::POWER_TYPE;};
    
    // pimpl idiom
  private:
    class Impl;
    Impl * pimpl;
    
  };

}

#endif /* CURVESEGMENTPOWER_H_ */