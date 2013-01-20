#include <boost/filesystem.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/regex.hpp>

#include <unicode/unistr.h>
#include <unicode/ucsdet.h>

#include "File.h"
#include "Utf8Utils.h"

File::File() :
		mFileSize(static_cast<boost::uintmax_t>(-1)) {
}

File::File(const char *fileName) :
		mFileName(fileName),
		mFileSize(static_cast<boost::uintmax_t>(-1)) {
	namespace bfs = boost::filesystem;
	namespace bip = boost::interprocess;

	const bfs::path path(fileName);
	boost::system::error_code ec;
	const bfs::file_status fs = bfs::status(path, ec);
	if (fs.type() != bfs::status_error) {
		// TODO: symlink
		const bool isFile = bfs::is_regular_file(fs);
		if (isFile) {
			const boost::uintmax_t fileSize = bfs::file_size(path, ec);
			if (fileSize != static_cast<boost::uintmax_t>(-1) && !ec) {
				mFileSize = fileSize;

				bip::file_mapping map(fileName, bip::read_only);
				bip::mapped_region view(map, bip::read_only); // TODO: lazy read
				const char *ptr = (const char *) view.get_address();
				const size_t size = view.get_size();

				UErrorCode errorCode = U_ZERO_ERROR;
				UCharsetDetector *ucsd = ucsdet_open(&errorCode);
				if (U_FAILURE(errorCode)) {
					// TODO:
				}

				errorCode = U_ZERO_ERROR;
				ucsdet_setText(ucsd, ptr, size - 1, &errorCode);
				if (U_FAILURE(errorCode)) {
					// TODO:
				}
				errorCode = U_ZERO_ERROR;
				int matchesFound;
				const UCharsetMatch **ucsm = ucsdet_detectAll(ucsd, &matchesFound, &errorCode);
				if (U_FAILURE(errorCode)) {
					// TODO:
				}

				for (int i = 0; i < matchesFound; i++) {
					Encoding encoding;
					errorCode = U_ZERO_ERROR;
					encoding.name = ucsdet_getName(ucsm[i], &errorCode);
					if (U_FAILURE(errorCode)) {
						// TODO:
					}
					errorCode = U_ZERO_ERROR;
					encoding.confidence = ucsdet_getConfidence(ucsm[i], &errorCode);
					if (U_FAILURE(errorCode)) {
						// TODO:
					}
					mFileEncodingCandidate.push_back(encoding);
				}
				assert(mFileEncodingCandidate.size() > 0);

				char *lineBegin = const_cast<char *>(ptr);
				const char *fileEnd = ptr + size;
				const char *encodingName = mFileEncodingCandidate[0].name;
				char *nextLineBegin;
				char *lineEnd;
				Line::LineFeedCode lfcDefault = Line::LineFeedCodeDefault;
				while (lineBegin != NULL) {
					Line::LineFeedCode lfc =
						Line::searchLineFeedCode(lineBegin, fileEnd, encodingName, &lineEnd, &nextLineBegin);
					if (lfcDefault == Line::LineFeedCodeDefault) {
						lfcDefault = lfc;
					}
					std::string normalized = toUtf8(lineBegin, lineEnd, encodingName);
					Line *line;
					if (lfc == lfcDefault) {
						line = new Line(normalized);
					} else {
						line = new Line(normalized, lfc);
					}
					mLines.push_back(line);
					lineBegin = nextLineBegin;
				}
			}
		}
	}
}

