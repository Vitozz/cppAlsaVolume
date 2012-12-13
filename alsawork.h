#ifndef ALSAWORK_H
#define ALSAWORK_H

#include <alsa/asoundlib.h>

class AlsaWork
{
public:
	AlsaWork();
	void setVolume (int volume);
private:
	void onError(int index);
};

#endif // ALSAWORK_H
