#include <TimeBox.h>
#include <TimeValue.h>
//#include <TimeNode.h>
#include <vector>
using namespace OSSIA;

class TimeBox::Impl
{/*
	public:
		Impl():
			_startTimeNode{new TimeNode},
			_endTimeNode{new TimeNode}
		{

		}
		std::vector<TimeProcess*> _processes;
		TimeNode* _startTimeNode{nullptr};
		TimeNode* _endTimeNode{nullptr};*/
};
class TimeBox::const_iterator
{

};

TimeBox::TimeBox()//:
//	pimpl{new Impl}
{

}

TimeBox::~TimeBox()
{

}

TimeBox::const_iterator TimeBox::begin() const
{
//	return pimpl->_processes.cbegin();
}

TimeBox::const_iterator TimeBox::end() const
{
//	return pimpl->_processes.cend();
}

void TimeBox::play(bool log, std::string name) const
{
//	for(TimeProcess* p : *this)
	{

	}
}
/*
TimeNode&TimeBox::getStartNode() const
{
//	return *pimpl->_startTimeNode;
}

TimeNode&TimeBox::getEndNode() const
{
//	return *pimpl->_endTimeNode;
}

void TimeBox::setStartNode(TimeNode&& ev)
{
//	pimpl->_startTimeNode = new TimeNode(ev);
}

void TimeBox::setEndNode(TimeNode&& ev)
{
//	pimpl->_endTimeNode = new TimeNode(ev);
}

void TimeBox::setStartNode(const TimeNode& ev)
{
//	pimpl->_startTimeNode = &const_cast<TimeNode&>(ev);
}

void TimeBox::setEndNode(const TimeNode&ev)
{
//	pimpl->_endTimeNode = &const_cast<TimeNode&>(ev);
}
*/
TimeValue TimeBox::getLength() const
{
//	return pimpl->_endTimeNode->getDate() - pimpl->_startTimeNode->getDate();
}

bool TimeBox::isRigid()
{
//	return true;
}
