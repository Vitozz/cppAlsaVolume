/*
 * iconpacks.h
 * Copyright (C) 2012 Vitaly Tonkacheyev
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef ICONPACKS_H
#define ICONPACKS_H

#include "archive.h"
#include "archive_entry.h"
#include "unistd.h"
#include "tools.h"
#include <fstream>
#include <cstdlib>
#include <vector>

class iconpacks
{
public:
	iconpacks(const std::string &fileName, const std::string outPath);
	~iconpacks();
	void extract();
	std::vector<std::string> getPacks();
private:
	void checkArchiveError(int err, int lineNumber, const std::string &text);
	int copyData(archive *in, archive *out);
private:
	std::string fileName_;
	std::string outPath_;
	std::string tmpPath_;
	int flags_;
};

#endif // ICONPACKS_H
