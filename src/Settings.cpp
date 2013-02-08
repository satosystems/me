#include "Settings.h"

const char *Settings::settingKey_defaultEncoding = "defaultEncoding";
const char *Settings::settingKey_defaultLineFeed = "defaultLineFeed";
bool Settings::initialized = false;

std::string Settings::sSettingsHome;
std::string Settings::sSettingsFilePath;

Settings *gSettings;

Settings::Settings() : mFile(new TextFile(sSettingsFilePath)) {
	Logger::d("sSettingsFilePath:%s", sSettingsFilePath.c_str());
	initialized = true;
}

Settings::~Settings() {
	delete mFile;
}

void Settings::reload() {
	mFile->load();
	for (int i = 0; i < mFile->getLineCount(); i++) {
		std::string s;
		s.append(mFile->mLines[i]->data(), mFile->mLines[i]->size());
		Logger::d("%s", s.c_str());
	}
}

void Settings::save() {
	mFile->save();
}

std::string Settings::findJsonValue(const char *name) const {
	std::string key(std::string("\"").append(name).append("\""));
	std::string value;
	int lineCount = mFile->mLines.size();
	for (int i = 0; i < lineCount; i++) {
		TextLine *line = dynamic_cast<TextLine *>(mFile->mLines[i]);
		int index = line->find(key.data(), 0, (int) key.size());
		if (index != -1) {
			index += key.size();
			do {
				if (index < line->size()) {
					if ((*line)[index++] == ':') {
						break;
					}
				} else {
					if (++i < lineCount) {
						index = 0;
						line = dynamic_cast<TextLine *>(mFile->mLines[i]);
					} else {
						Logger::d("JSON file was broken, no found ':'");
						return value;
					}
				}
			} while (true);
			do {
				if (index < line->size()) {
					if ((*line)[index++] == '"') {
						break;
					}
				} else {
					if (++i < lineCount) {
						index = 0;
						line = dynamic_cast<TextLine *>(mFile->mLines[i]);
					} else {
						Logger::d("JSON file was broken, no found begin of value '\"'");
						return value;
					}
				}
			} while (true);
			int start = index;
			bool escape = false;
			do {
				if (index < line->size()) {
					if ((*line)[index++] == '"') {
						if (!escape) {
							break;
						}
					}
					escape = (*line)[index - 1] == '\\';
				} else {
					Logger::d("JSON file was broken, no found end of value '\"'");
					return value;
				}
			} while (true);
			int end = index - 1;
			for (int i = start; i < end; i++) {
				value += (*line)[i];
			}
			break;
		}
	}
	if (value == "") {
		Logger::d("%s was not found", name);
	}
	return value;
}

TextLine::LineFeed Settings::getDefaultLineFeed() {
	TextLine::LineFeed lf = TextLine::LineFeedLF;
	if (initialized) {
		reload();
		std::string value = findJsonValue(settingKey_defaultLineFeed);
		if (value == "CRLF") {
			lf = TextLine::LineFeedCRLF;
		} else if (value == "CR") {
			lf = TextLine::LineFeedCR;
		}
	}
	return lf;
}

std::string Settings::getDefaultEncoding() {
	std::string value;
	if (initialized) {
		reload();
		value = findJsonValue(settingKey_defaultEncoding);
	}
	if (value == "") {
		value = "UTF-8";
	}
	return value;
}

