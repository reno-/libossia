/*!
 * \file CurveSegmentNone.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef CURVESEGMENTNONE_H_
#define CURVESEGMENTNONE_H_

#include "Editor/CurveSegment.h"

namespace OSSIA {

  class CurveSegmentNone : public CurveSegment<void> {

public:

  // Constructors, destructor, assignment
  CurveSegmentNone();
  CurveSegmentNone(const CurveSegmentNone&);
  virtual ~CurveSegmentNone();
  CurveSegmentNone & operator= (const CurveSegmentNone&);
    
  virtual CurveSegmentType getType() const override final
    {return NONE_TYPE;};

  // Computation
  virtual void valueAt(double) const override final
    {return;};

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}



#endif /* CURVESEGMENTNONE_H_ */