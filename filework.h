#ifndef FILEWORK_H
#define FILEWORK_H

#include <iostream>
#include <glibmm/ustring.h>

Glib::ustring getResPath(const char *resName);
bool checkFileExists(const std::string &fileName);

#endif // FILEWORK_H