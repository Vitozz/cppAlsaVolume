/*
 * iconpacks.cpp
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

#include "iconpacks.h"

iconpacks::iconpacks(const std::string &fileName, const std::string outPath)
{
	fileName_ = fileName;
	outPath_ = outPath;
	tmpPath_ = Tools::getTmpDir();
	flags_ = ARCHIVE_EXTRACT_TIME;
	flags_ |= ARCHIVE_EXTRACT_PERM;
	flags_ |= ARCHIVE_EXTRACT_ACL;
	flags_ |= ARCHIVE_EXTRACT_FFLAGS;
}

iconpacks::~iconpacks()
{
}

void iconpacks::checkArchiveError(int err, int lineNumber, const std::string &text)
{
	if (err != ARCHIVE_OK) {
		std::cerr << "iconpacks.cpp::" << lineNumber << "::" << text << std::endl;
	}
	if (err < ARCHIVE_WARN) {
		std::cerr << "iconpacks.cpp::" << lineNumber << "::" << text << std::endl;
	}
}

void iconpacks::extract()
{
	Tools::clearTempDir(tmpPath_);
	struct archive_entry *entry;
	int err;
	struct archive *input_ = archive_read_new();
	struct archive *output_ = archive_write_disk_new();
	archive_read_support_format_all(input_);
	archive_read_support_compression_all(input_);
	archive_write_disk_set_options(output_, flags_);
	archive_write_disk_set_standard_lookup(output_);
	try {
		if (!fileName_.empty()) {
			err = archive_read_open_file(input_, fileName_.c_str(), 10240);
			checkArchiveError(err, 61, "Error reading archive");
			if (err == ARCHIVE_OK){
				std::string entryPath;
				while (1) {
					err = archive_read_next_header(input_, &entry);
					if (err == ARCHIVE_EOF) {
						break;
					}
					checkArchiveError(err, 66, "Reading header error");
					if (!outPath_.empty()) {
						entryPath = outPath_ + "/" + archive_entry_pathname(entry);
						archive_entry_set_pathname(entry, entryPath.c_str());
					}
					err = archive_write_header(output_, entry);
					checkArchiveError(err, 75, "Writing header error");
					if ((err == ARCHIVE_OK) && (archive_entry_size(entry) > 0)) {
						err = copyData(input_, output_);
						checkArchiveError(err, 78, "copyData function error");
					}
				}
				err = archive_write_finish_entry(output_);
				checkArchiveError(err, 82, "write_finish_entry function error");
			}
		}
	}
	catch (const std::exception &ex) {
		std::cout << ex.what() << std::endl;
	}
	archive_read_close(input_);
	archive_read_free(input_);
	archive_write_close(output_);
	archive_write_free(output_);
}

int iconpacks::copyData(archive *in, archive *out)
{
	int err;
	const void *buff;
	size_t size;
#if ARCHIVE_VERSION_NUMBER >= 3000000
	int64_t offset;
#else
	off_t offset;
#endif

	while (1) {
		err = archive_read_data_block(in, &buff, &size, &offset);
		if (err == ARCHIVE_EOF) {
			return ARCHIVE_OK;
		}
		if (err != ARCHIVE_OK) {
			return err;
		}
		err = archive_write_data_block(out, buff, size, offset);
		if (err != ARCHIVE_OK) {
			checkArchiveError(err, 115, "write data error");
			return err;
		}
	}
	return ARCHIVE_WARN;
}

std::vector<std::string> iconpacks::getPacks()
{
	std::string path = "";
	std::vector<std::string> checkList;
	std::vector<std::string> result;
	const std::string suffix = Tools::PathSuffix + "iconpacks/";
	checkList.push_back("/usr" + suffix);
	checkList.push_back("/usr/local" + suffix);
	checkList.push_back(Tools::getHomePath() + "iconpacks/");
	checkList.push_back(Tools::getCWD() + "/iconpacks/");
	std::vector<std::string>::iterator it = checkList.begin();
	std::vector<std::string> list;
	while (it != checkList.end()) {
		list = Tools::getFileList(*it);
		if (list.size() > 0) {
			std::vector<std::string>::iterator lit = list.begin();
			while (lit != list.end()) {
				std::string item = *lit;
				if (item.find("tar.gz") != std::string::npos) {
					path = *it;
					goto next;
				}
				lit++;
			}
		}
		it++;
	}
	next:
	result.push_back(Tools::defaultIconPack);
	if (!path.empty()) {
		std::vector<std::string> packs = Tools::getFileList(path);
		it = packs.begin();
		std::string item;
		while (it != packs.end()) {
			item = path + *it;
			result.push_back(item);
			it++;
		}
	}
	return result;
}
