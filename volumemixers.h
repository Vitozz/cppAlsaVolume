#ifndef VOLUMEMIXERS_H
#define VOLUMEMIXERS_H

#include <string>
#include <vector>

class VolumeMixers
{
public:
	VolumeMixers();
	~VolumeMixers();
	std::vector<std::string> &playback();
	std::vector<std::string> &capture();
	void setPlayback(std::vector<std::string> &list);
	void setCapture(std::vector<std::string> &list);
private:
	std::vector<std::string> *playback_;
	std::vector<std::string> *capture_;
};

#endif // VOLUMEMIXERS_H
