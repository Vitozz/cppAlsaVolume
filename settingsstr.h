#ifndef SETTINGSSTR_H
#define SETTINGSSTR_H

#include <string>
#include <vector>
#include "mixerswitches.h"

class settingsStr
{
public:
	settingsStr();
	~settingsStr();
	unsigned int cardId();
	unsigned int mixerId();
	bool notebookOrientation();
	bool isAutorun();
	std::string &currIconPack();
	std::vector<std::string> &cardList();
	std::vector<std::string> &mixerList();
	std::vector<std::string> &iconPacks();
	MixerSwitches &switchList();
	void setCardId(unsigned int id);
	void setMixerId(unsigned int id);
	void setNotebookOrientation(bool orient);
	void setIsAutorun(bool autorun);
	void pushBack(ListType listType, const std::string &item);
	void addMixerSwitch(MixerSwitches &switchItem);
	void setList(ListType listType, std::vector<std::string> &list);
	void setCurrIconPack(const std::string &iconPack);
private:
	unsigned int cardId_;
	unsigned int mixerId_;
	bool notebookOrientation_;
	bool isAutorun_;
	std::string currIconPack_;
	std::vector<std::string> *cardList_;
	std::vector<std::string> *mixerList_;
	std::vector<std::string> *iconPacks_;
	MixerSwitches *switchList_;
};

#endif // SETTINGSSTR_H
