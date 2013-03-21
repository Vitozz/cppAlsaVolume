#ifndef SWITCHCAP_H
#define SWITCHCAP_H

#include <string>

class switchcap
{
public:
	switchcap();
	bool enabled();
	std::string name();
	void setEnabled(bool isEnabled);
	void setName(const std::string &Name);
private:
	bool enabled_;
	std::string *name_;
};

#endif // SWITCHCAP_H
