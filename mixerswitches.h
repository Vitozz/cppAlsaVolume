#ifndef MIXERSWITCHES_H
#define MIXERSWITCHES_H

//#include "switchcap.h"
#include "glibmm/refptr.h"
#include "tools.h"

class MixerSwitches
{
public:
	MixerSwitches();
	~MixerSwitches();
	void pushBack(SwitchType sType, switchcap &item);
	void setCaptureSwitchList(std::vector<switchcap> &list);
	void setPlaybackSwitchList(std::vector<switchcap> &list);
	void setEnumSwitchList(std::vector<switchcap> &list);
	void clear(SwitchType sType);
	std::vector<switchcap> &captureSwitchList();
	std::vector<switchcap> &playbackSwitchList();
	std::vector<switchcap> &enumSwitchList();

private:
	std::vector<switchcap> *captureSwitchList_;
	std::vector<switchcap> *playbackSwitchList_;
	std::vector<switchcap> *enumSwitchList_;
};

#endif // MIXERSWITCHES_H
