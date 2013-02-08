#ifndef _b4b6ebc6_c765_4e02_9266_4c813476f8c6_
#define _b4b6ebc6_c765_4e02_9266_4c813476f8c6_

#include <stdlib.h>

#include <boost/filesystem.hpp>

#include "TextFile.h"
#include "Utf8Utils.h"
#include "Logger.h"

class Settings;

extern Settings *gSettings;

class Settings {
public:
	static const char *settingKey_defaultEncoding;
	static const char *settingKey_defaultLineFeed;
	static bool initialized;

	enum RESULT {
		SUCCESS,
		ERROR_CREATE_DIR,
		ERROR_CREATE_FILE,
		ERROR_FILE_ALREADY_EXISTS,
		ERROR_DIR_ALREADY_EXISTS,
		ERROR_UNKNOWN,
	};

	Settings();

	~Settings();

	static RESULT init() {
		if (sSettingsHome == "") {
// TODO: Comment out for debug because
// in my environment cannot create ~/.me directory.
// Maybe my home directory is symbolic link.
//			sSettingsHome = std::string(getenv("HOME")) + "/.me";
			sSettingsHome = std::string("settings");
			sSettingsFilePath = std::string(sSettingsHome + "/settings.json");
			Logger::d("Home:%s", sSettingsHome.c_str());
			Logger::d("File:%s", sSettingsFilePath.c_str());
		}
		namespace bfs = boost::filesystem;

		boost::system::error_code ec;
		const bfs::path dir(sSettingsHome.c_str());
		const bool existsDir = bfs::exists(dir, ec);
		if (ec || !existsDir) {
			Logger::d("file not found");
			bfs::create_directories(dir, ec);
			if (ec) {
				Logger::e("ERROR_CREATE_DIR");
				return ERROR_CREATE_DIR;
			}
		} else {
			const bfs::file_status fsdir(bfs::status(dir, ec));
			Logger::d("existsDir:%s", existsDir ? "true" : "false");
			Logger::d("fsdir.type():%d", fsdir.type());
			Logger::d("file_not_found:%d regular_file:%d directory_file:%d", bfs::file_not_found, bfs::regular_file, bfs::directory_file);
			if (!fsdir.type() == bfs::regular_file) {
				return ERROR_FILE_ALREADY_EXISTS;
			}
		}
		const bfs::path file(sSettingsFilePath.c_str());
		const bool existsFile = bfs::exists(file, ec);
		if (ec || !existsFile) {
			std::string json;
			json += "{\n";
			json += "  \"";
			json += settingKey_defaultEncoding;
			json += "\": \"UTF-8\",\n";
			json += "  \"";
			json += settingKey_defaultLineFeed;
			json += "\": \"LF\"\n";
			json += "}\n";

			std::ofstream ofs(sSettingsFilePath.c_str());
			ofs.write(json.c_str(), json.size());
			ofs.close();
		} else {
			const bfs::file_status fsfile(bfs::status(file, ec));
			if (ec) {
				Logger::e("error 3");
				return ERROR_UNKNOWN;
			}
			if (fsfile.type() == bfs::directory_file) {
				return ERROR_DIR_ALREADY_EXISTS;
			}
		}
		gSettings = new Settings;
		return SUCCESS;
	}

	void reload();

	void save();

	TextLine::LineFeed getDefaultLineFeed();

	std::string getDefaultEncoding();

private:
	static std::string sSettingsHome;
	static std::string sSettingsFilePath;
	TextFile *mFile;

	std::string findJsonValue(const char *name) const;
};

#endif
