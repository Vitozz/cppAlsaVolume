#ifndef MIXERSWITCHES_H
#define MIXERSWITCHES_H

#include "switchcap.h"
#include "glibmm/refptr.h"
#include <vector>

typedef Glib::RefPtr<switchcap> switchSmartPtr;

class MixerSwitches
{
public:
	MixerSwitches();
	~MixerSwitches();
	void push_back(switchSmartPtr item);
	void setCaptureSwitchList(std::vector<switchSmartPtr> &list);
	void setPlaybackSwitchList(std::vector<switchSmartPtr> &list);
	void setEnumSwitchList(std::vector<switchSmartPtr> &list);

private:
	std::vector<switchSmartPtr> captureSwitchList_;
	std::vector<switchSmartPtr> playbackSwitchList_;
	std::vector<switchSmartPtr> enumSwitchList_;
};

#endif // MIXERSWITCHES_H
