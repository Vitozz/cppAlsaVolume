#include "switchcap.h"

switchcap::switchcap(bool Enabled, const std::string &Name)
: isEnabled_(Enabled),
  name_(Name)
{
}

bool switchcap::isEnabled()
{
	return isEnabled_;
}

std::string &switchcap::name()
{
	return name_;
}

void switchcap::setEnabled(bool isEnabled)
{
	isEnabled_ = isEnabled;
}

void switchcap::setName(const std::string &Name)
{
	name_ = Name;
}
