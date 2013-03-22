/*
 * tools.cpp
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

#include "tools.h"
#include "glibmm/fileutils.h"
#include "glibmm/keyfile.h"
#include "glib.h"
#include "glib/gstdio.h"
#include "archive.h"
#include "archive_entry.h"
#include "unistd.h"
#include <fstream>
#include <cstdlib>
#include <vector>
#include <map>


void checkArchiveError(int err, int lineNumber, const std::string &text) {
	if (err != ARCHIVE_OK) {
		std::cerr << "tools.cpp::" << lineNumber << "::" << text << std::endl;
	}
	if (err < ARCHIVE_WARN) {
		std::cerr << "tools.cpp::" << lineNumber << "::" << text << std::endl;
	}
}

bool Tools::checkFileExists(const std::string &fileName)
{
	return g_file_test(fileName.c_str(), G_FILE_TEST_EXISTS);
}

bool Tools::checkDirExists(const std::string &fileName)
{
	return g_file_test(fileName.c_str(), G_FILE_TEST_IS_DIR);
}

Glib::ustring Tools::getCWD()
{
	const size_t cwdSize = 255;
	char cwdBuffer[cwdSize];
	const Glib::ustring cwd = Glib::ustring(getcwd(cwdBuffer, cwdSize));
	return cwd;
}

Glib::ustring Tools::getHomePath()
{
	const Glib::ustring homepath(Glib::ustring(getenv("HOME")) + "/.local/share/alsavolume/");
	return homepath;
}

Glib::ustring Tools::getResPath(const char *resName)
{
	const Glib::ustring pathSuffix("/share/alsavolume/");
	const Glib::ustring resName_(resName);
	std::vector<Glib::ustring> list;
	list.push_back(getHomePath() + resName_);
	list.push_back(getCWD() + "/" + resName_);
	list.push_back(getTmpDir() + "/" + resName_);
	list.push_back("/usr"+ pathSuffix + resName_);
	list.push_back("/usr/local" + pathSuffix + resName_);
	std::vector<Glib::ustring>::iterator it = list.begin();
	while (it != list.end()) {
		if (checkFileExists(*it)) {
			return Glib::ustring(*it);
		}
		it++;
	}
	return "";
}

void Tools::createDirectory(const std::string &dirName)
{
	if (!g_file_test(dirName.c_str(), G_FILE_TEST_IS_DIR)) {
		std::cout << "Directory " << dirName << " not found. Attempting to create it.." << std::endl;
		gint err = 0;
		err = g_mkdir_with_parents(dirName.c_str(), 0755);
		if (err < 0) {
			std::cerr << g_file_error_from_errno(err) << std::endl;
		}
	}
}

void Tools::saveFile(const std::string &fileName, const Glib::ustring &fileData)
{
	try {
		std::ofstream ofile(fileName.c_str());
		ofile << fileData << std::endl;
		ofile.close();
	}
	catch ( const std::exception & ex ) {
		std::cout << "settings.cpp::57::Parsing failed:: " << ex.what() << std::endl;
	}
}

std::pair<bool, int> Tools::itemExists(std::vector<std::string> &vector_, const Glib::ustring& item)
{
	int index = 0;
	int i = 0;
	bool exists = false;
	std::vector<std::string>::iterator it = vector_.begin();
	while (it != vector_.end()) {
		Glib::ustring answ(*it);
		if (answ == item) {
			index = i;
			exists = true;
			break;
		}
		it++;
		i++;
	}
	std::pair<bool, int> result(exists, index);
	return result;
}

std::vector<std::string> Tools::getFileList(const std::string &dir)
{
	if (checkDirExists(dir)) {
		std::string dirname = g_path_get_dirname(dir.c_str());
		Glib::Dir dir_ (dirname);
		std::vector<std::string> entries (dir_.begin(), dir_.end());
		return entries;
	}
	return std::vector<std::string>();
}

std::string Tools::getTmpDir()
{
	return (std::string(g_get_user_cache_dir()) + std::string("/alsavolume"));
}

void Tools::extractArchive(const std::string &archiveFileName, const std::string &outPath)
{
	const std::string tmpDirPath = Tools::getTmpDir();
	Tools::clearTempDir(tmpDirPath);
	struct archive_entry *entry;
	int flags = ARCHIVE_EXTRACT_TIME;
	flags |= ARCHIVE_EXTRACT_PERM;
	flags |= ARCHIVE_EXTRACT_ACL;
	flags |= ARCHIVE_EXTRACT_FFLAGS;
	int err;
	struct archive *ar = archive_read_new();
	struct archive *ext = archive_write_disk_new();
	archive_read_support_format_all(ar);
	archive_read_support_compression_all(ar);
	archive_write_disk_set_options(ext, flags);
	archive_write_disk_set_standard_lookup(ext);
	try {
		err = archive_read_open_file(ar, archiveFileName.c_str(), 10240);
		checkArchiveError(err, 153, "Error reading archive");
		std::string entryPath;
		while (1) {
			err = archive_read_next_header(ar, &entry);
			if (err == ARCHIVE_EOF) {
				break;
			}
			checkArchiveError(err, 156, "Reading header error");
			if (!outPath.empty()) {
				entryPath = outPath + "/" + archive_entry_pathname(entry);
				archive_entry_set_pathname(entry, entryPath.c_str());
			}
			err = archive_write_header(ext, entry);
			checkArchiveError(err, 161, "Writing header error");
			if ((err == ARCHIVE_OK) && (archive_entry_size(entry) > 0)) {
				err = copyData(ar, ext);
				checkArchiveError(err, 164, "copyData function error");
			}
		}
		err = archive_write_finish_entry(ext);
		checkArchiveError(err, 168, "write_finish_entry function error");
	}
	catch (const std::exception &ex) {
		std::cout << ex.what() << std::endl;
	}
	archive_read_close(ar);
	archive_read_free(ar);
	archive_write_close(ext);
	archive_write_free(ext);
}

int Tools::copyData(archive *in, archive *out)
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
			checkArchiveError(err, 199, "write data error");
			return err;
		}
	}
	return ARCHIVE_WARN;
}

void Tools::clearTempDir(const std::string &path)
{
	try {
		if (checkDirExists(path)) {
			int err;
			std::vector<std::string> filelist = getFileList(path);
			std::vector<std::string>::iterator it = filelist.begin();
			std::string filename;
			while (it != filelist.end()) {
				filename = path + std::string(*it);
				if (checkFileExists(filename)) {
					err = g_unlink(filename.c_str());
					if (err != 0)
						std::cout << "Temp file" + *it + " was not removed : " << err << std::endl;
				}
				it++;
			}
			if ((err = g_rmdir(path.c_str())))
				std::cout << "Temp dir was not removed : " << err << std::endl;
		}
	}
	catch (const std::exception &ex) {
		std::cerr << ex.what() << std::endl;
	}
}

std::string Tools::checkIconPacks()
{
	std::vector<std::string> checkList;
	const std::string preffix = "/share/alsavolume/iconpacks/";
	const std::string localPath = "/usr/local" + preffix;
	const std::string globalPath = "/usr" + preffix;
	const std::string homePath = getHomePath() + "iconpacks/";
	const std::string cwd = getCWD() + "/iconpacks/";
	checkList.push_back(globalPath);
	checkList.push_back(localPath);
	checkList.push_back(homePath);
	checkList.push_back(cwd);
	std::vector<std::string>::iterator it = checkList.begin();
	std::vector<std::string> list;
	std::string item;
	while (it != checkList.end()) {
		list = getFileList(*it);
		if (list.size() > 0) {
			std::vector<std::string>::iterator i = list.begin();
			while (i != list.end()) {
				item = std::string(*i);
				if (item.find("tar.gz") != std::string::npos) {
					return std::string(*it);
				}
				i++;
			}

		}
		it++;
	}
	return "";
}

std::vector<std::string> &Tools::getIconPacks()
{
	const std::string path = checkIconPacks();
	std::vector<std::string> packs = getFileList(path);
	std::vector<std::string> result;
	result.push_back(Tools::defaultIconPack);
	std::vector<std::string>::iterator it = packs.begin();
	std::string item;
	while (it != packs.end()) {
		item = path + *it;
		result.push_back(item);
		it++;
	}
	return result;
}

std::string Tools::pathToFileName(const std::string &path)
{
	const std::string fileName = g_path_get_basename(path.c_str());
	return fileName;
}
