#include "switchcap.h"

switchcap::switchcap()
{
	name_="";
}

bool switchcap::enabled()
{
	return enabled_;
}

std::string switchcap::name()
{
	return *name_;
}

void switchcap::setEnabled(bool isEnabled)
{
	enabled_ = isEnabled;
}

void switchcap::setName(const std::string &Name)
{
	name_ = Name;
}
