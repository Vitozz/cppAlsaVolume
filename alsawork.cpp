#include "alsawork.h"
#include <iostream>

AlsaWork::AlsaWork()
{

}

void AlsaWork::setVolume(int volume)
{
	int min, max;

}

void AlsaWork::onError(int index)
{
	if (index < 0) {
		const char *error = snd_strerror(index);
		std::cerr << error;
		exit(1);
	}
}
