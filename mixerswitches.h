#ifndef MIXERSWITCHES_H
#define MIXERSWITCHES_H

#include "switchcap.h"
#include "glibmm/refptr.h"
#include "tools.h"
#include "boost/ptr_container/ptr_vector.hpp"

class MixerSwitches
{
public:
	MixerSwitches();
	void pushBack(SwitchType sType, switchcap *item);
	void setCaptureSwitchList(boost::ptr_vector<switchcap> list);
	void setPlaybackSwitchList(boost::ptr_vector<switchcap> list);
	void setEnumSwitchList(boost::ptr_vector<switchcap> list);
	void clear(SwitchType sType);
	boost::ptr_vector<switchcap> captureSwitchList();
	boost::ptr_vector<switchcap> playbackSwitchList();
	boost::ptr_vector<switchcap> enumSwitchList();

private:
	boost::ptr_vector<switchcap> captureSwitchList_;
	boost::ptr_vector<switchcap> playbackSwitchList_;
	boost::ptr_vector<switchcap> enumSwitchList_;
};

#endif // MIXERSWITCHES_H
