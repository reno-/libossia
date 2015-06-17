/*!
 * \file JamomaTimeConstraint.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once
#include "Editor/TimeConstraint.h"
#include "Editor/TimeValue.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeConstraint : public TimeConstraint
{

private:
  
  // Implementation specific
  TimeValue               mDuration;
  TimeValue               mDurationMin;
  TimeValue               mDurationMax;
  
  shared_ptr<TimeEvent>   mStartEvent;
  shared_ptr<TimeEvent>   mEndEvent;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaTimeConstraint(TimeValue nominal, shared_ptr<TimeEvent> startEvent, shared_ptr<TimeEvent> endEvent, TimeValue min, TimeValue max);
  
  JamomaTimeConstraint(const JamomaTimeConstraint * other);
  
  virtual ~JamomaTimeConstraint();
  
  shared_ptr<TimeConstraint> clone() const override;

# pragma mark -
# pragma mark Execution
  
  void play(bool log = false, string name = "") const override;

# pragma mark -
# pragma mark Accessors
  
  const shared_ptr<TimeEvent> & getStartEvent() const override;
  
  const shared_ptr<TimeEvent> & getEndEvent() const override;
};