#ifndef ALSAWORK_H
#define ALSAWORK_H

#include "alsa/asoundlib.h"

void onError (int errorIndex);
void setAlsaVolume(const char *mixer, long volume);

#endif // ALSAWORK_H
