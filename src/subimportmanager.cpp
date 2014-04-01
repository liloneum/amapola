#include "subimportmanager.h"
#include "SubtitlesParsers/SubRip/subripparser.h"
#include "SubtitlesParsers/DcSubtitle/interop/dcsubinteropparser.h"
#include "SubtitlesParsers/DcSubtitle/smpte/dcsubsmpteparser.h"

SubImportManager::SubImportManager() {

}

// Read a sample a the file to find the good parser
SubtitleFileParser *SubImportManager::findSubFileParser(FileReader fileReader) {

    QString file_name = fileReader.getFileName();

    SubtitleFileParser* parser;

    // If file extension is ".srt", check if it's readable SubRip format
    if ( file_name.section(".", -1) == "srt" ) {
        parser = new SubRipParser();

        if ( parser->readSample(fileReader) == true ) {

            return parser;
        }
    }
    // If file extension is ".xml", check if it's readable DCSub format
    else if ( file_name.section(".", -1) == "xml" ) {

        // Check interop
        parser = new DcSubInteropParser;

        if ( parser->readSample(fileReader) == true ) {

            return parser;
        }

        // Check smpte
        parser = new DcSubSmpteParser;

        if ( parser->readSample(fileReader) == true ) {

            return parser;
        }
    }
    // If file has no extension, try with all supported format
    else {

        parser = new SubRipParser();

        if ( parser->readSample(fileReader) == true ) {

            return parser;
        }

        parser = new DcSubInteropParser;

        if ( parser->readSample(fileReader) == true ) {

            return parser;
        }

        parser = new DcSubSmpteParser;

        if ( parser->readSample(fileReader) == true ) {

            return parser;
        }
    }

    // If file format is not supported, return a NULL parser
    return NULL;
}
