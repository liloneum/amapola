#include "ebuparser.h"
#include "myattributesconverter.h"
#include <QApplication>
#include <QDate>
#include <QXmlStreamReader>
#include <QTextEdit>
#include <QTextCodec>

// Languages code table
const QString EbuParser::LANGUAGES_CODES[128] =
{"Unknown",
 "Albanian",
 "Breton",
 "Catalan",
 "Croatian",
 "Welsh",
 "Czech",
 "Danish",
 "German",
 "English",
 "Spanish",
 "Esperanto",
 "Estonian",
 "Basque",
 "Faroese",
 "French",
 "Frisian",
 "Irish",
 "Gaelic",
 "Galician",
 "Icelandic",
 "Italian",
 "Lappish",
 "Latin",
 "Latvian",
 "Luxembourgian",
 "Lithuanian",
 "Hungarian",
 "Maltese",
 "Dutch",
 "Norwegian",
 "Occitan",
 "Polish",
 "Portugese",
 "Romanian",
 "Romansh",
 "Serbian",
 "Slovak",
 "Slovenian",
 "Finnish",
 "Swedish",
 "Turkish",
 "Flemish",
 "Wallon",
 "",
 "",
 "",
 "Reserved",
 "Reserved",
 "Reserved",
 "Reserved",
 "Reserved",
 "Reserved",
 "Reserved",
 "Reserved",
 "Reserved",
 "Reserved",
 "Reserved",
 "Reserved",
 "Reserved",
 "Reserved",
 "Reserved",
 "Reserved",
 "Reserved",
 "",
 "",
 "",
 "",
 "",
 "Zulu",
 "Vietnamese",
 "Uzbek",
 "Urdu",
 "Ukrainian",
 "Thai",
 "Telugu",
 "Tatar",
 "Tamil",
 "Tadzhik",
 "Swahili",
 "Sranan Tongo",
 "Somali",
 "Sinhalese",
 "Shona",
 "Serbo-croat",
 "Ruthenian",
 "Russian",
 "Quechua",
 "Pushtu",
 "Punjabi",
 "Persian",
 "Papamiento",
 "Oriya",
 "Nepali",
 "Ndebele",
 "Marathi",
 "Moldavian",
 "Malaysian",
 "Malagasay",
 "Macedonian",
 "Laotian",
 "Korean",
 "Khmer",
 "Kazakh",
 "Kannada",
 "Japanese",
 "Indonesian",
 "Hindi",
 "Hebrew",
 "Hausa",
 "Gurani",
 "Gujurati",
 "Greek",
 "Georgian",
 "Fulani",
 "Dari",
 "Churash",
 "Chinese",
 "Burmese",
 "Bulgarian",
 "Bengali",
 "Bielorussian",
 "Bambora",
 "Azerbaijani",
 "Assamese",
 "Armenian",
 "Arabic",
 "Amharic"
};

// Constructor
EbuParser::EbuParser()
{
}

// Read the GSI block of the given "stl" file
void EbuParser::readGsiBlock(MyFileReader file, SubExportDialog *exportDialog) {

    QList<quint8>* datas = file.data();

    // Disk Format Code (DFC)
    QString tcr_str;
    tcr_str.append( QChar::fromLatin1( datas->at(DISK_FORMAT_CODE_ADD + DFC_TIME_CODE_RATE_OFFSET) ) );
    tcr_str.append( QChar::fromLatin1( datas->at(DISK_FORMAT_CODE_ADD + DFC_TIME_CODE_RATE_OFFSET + 1) ) );
    exportDialog->setDiskFormatCode(tcr_str);

    // Display Standard Code (DSC)
    QString dsc_str;
    dsc_str.append( QChar::fromLatin1( datas->at(DISPLAY_STANDARD_CODE_ADD) ) );
    exportDialog->setDisplayMode(dsc_str);


    // Character Code Table (CCT) number
    QString cct_str;
    cct_str.append( QChar::fromLatin1( datas->at(CHARACTER_CODE_TABLE_NUMBER_ADD) ) );
    cct_str.append( QChar::fromLatin1( datas->at(CHARACTER_CODE_TABLE_NUMBER_ADD + 1) ) );
    exportDialog->setCharCodeTable(cct_str);

    // Language Code (LC)
    QString lc_str;
    lc_str.append( QChar::fromLatin1( datas->at(LANGUAGE_CODE_ADD) ) );
    lc_str.append( QChar::fromLatin1( datas->at(LANGUAGE_CODE_ADD + 1) ) );
    exportDialog->setLanguageCode(lc_str);

    // Original Programme Title (OPT)
    QString opt_str;
    for ( quint16 opt_it = 0; opt_it < ORIGINAL_PROGRAMME_TITLE_LENGTH; opt_it++ ) {

        opt_str.append( QChar::fromLatin1( datas->at(ORIGINAL_PROGRAMME_TITLE_ADD + opt_it ) ) );
    }

    exportDialog->setProgTitleVO( opt_str.simplified() );

    // Translated Programme Title (TPT)
    QString tpt_str;
    for ( quint16 tpt_it = 0; tpt_it < TRANSLATED_PROGRAMME_TITLE_LENGTH; tpt_it++ ) {

        tpt_str.append( QChar::fromLatin1( datas->at(TRANSLATED_PROGRAMME_TITLE_ADD + tpt_it ) ) );
    }

    exportDialog->setProgTitleTR( tpt_str.simplified() );

    // Original Episode Title (OET)
    QString oet_str;
    for ( quint16 oet_it = 0; oet_it < ORIGINAL_EPISODE_TITLE_LENGTH; oet_it++ ) {

        oet_str.append( QChar::fromLatin1( datas->at(ORIGINAL_EPISODE_TITLE_ADD + oet_it ) ) );
    }

    exportDialog->setEpTitleVO( oet_str.simplified() );

    // Translated Episode Title (TET)
    QString tet_str;
    for ( quint16 tet_it = 0; tet_it < TRANSLATED_EPISODE_TITLE_LENGTH; tet_it++ ) {

        tet_str.append( QChar::fromLatin1( datas->at(TRANSLATED_EPISODE_TITLE_ADD + tet_it ) ) );
    }

    exportDialog->setEpTitleTR( tet_str.simplified() );

    // Translator's Name (TN)
    QString tn_str;
    for ( quint16 tn_it = 0; tn_it < TRANSLATORS_NAME_LENGTH; tn_it++ ) {

        tn_str.append( QChar::fromLatin1( datas->at(TRANSLATORS_NAME_ADD + tn_it ) ) );
    }

    exportDialog->setTranslatorName( tn_str.simplified() );

    // Translator's Contact Details (TCD)
    QString tcd_str;
    for ( quint16 tcd_it = 0; tcd_it < TRANSLATORS_CONTACT_DETAILS_LENGTH; tcd_it++ ) {

        tcd_str.append( QChar::fromLatin1( datas->at(TRANSLATORS_CONTACT_DETAILS_ADD + tcd_it ) ) );
    }

    exportDialog->setTranslatorContact( tcd_str.simplified() );

    // Subtitle List Reference Code (SLR)
    QString slr_str;
    for ( quint16 slr_it = 0; slr_it < SUBTITLE_LIST_REFERENCE_CODE_LENGTH; slr_it++ ) {

        slr_str.append( QChar::fromLatin1( datas->at(SUBTITLE_LIST_REFERENCE_CODE_ADD + slr_it ) ) );
    }

    exportDialog->setSubListRef( slr_str.simplified() );

    // Creation Date (CD)
    QString cd_str;
    for ( quint16 cd_it = 0; cd_it < CREATION_DATE_LENGTH; cd_it++ ) {

        cd_str.append( QChar::fromLatin1( datas->at(CREATION_DATE_ADD + cd_it ) ) );
    }

    exportDialog->setCreationDate( cd_str.simplified() );

    // Revision Number (RN)
    QString rn_str;
    for ( quint16 rn_it = 0; rn_it < REVISION_NUMBER_LENGTH; rn_it++ ) {

        rn_str.append( QChar::fromLatin1( datas->at(REVISION_NUMBER_ADD + rn_it ) ) );
    }

    exportDialog->setRevisionNbr( rn_str.simplified() );

    // Maximum Number of Displayable Rows (MNR)
    QString mnr_str;
    for ( quint16 mnr_it = 0; mnr_it < MAX_NUMBER_OF_DISPLAYABLE_ROW_LENGTH; mnr_it++ ) {

        mnr_str.append( QChar::fromLatin1( datas->at(MAX_NUMBER_OF_DISPLAYABLE_ROW_ADD + mnr_it ) ) );
    }

    exportDialog->setMaxRow( mnr_str.simplified() );

    // Time Code: Start-of-Programme (TCP)
    QString tcp_str;
    for ( quint16 tcp_it = 0; tcp_it < TIME_CODE_START_OF_PROG_LENGTH; tcp_it++ ) {

        tcp_str.append( QChar::fromLatin1( datas->at(TIME_CODE_START_OF_PROG_ADD + tcp_it ) ) );
    }

    exportDialog->setStartOfProg( tcp_str.simplified() );

    // Country of Origin (CO)
    QString co_str;
    for ( quint16 co_it = 0; co_it < COUNTRY_OF_ORIGINE_LENGTH; co_it++ ) {

        co_str.append( QChar::fromLatin1( datas->at(COUNTRY_OF_ORIGINE_ADD + co_it ) ) );
    }

    exportDialog->setCountryOrigin( co_str.simplified() );

    // Publisher (PUB)
    QString pub_str;
    for ( quint16 pub_it = 0; pub_it < PUBLISHER_LENGTH; pub_it++ ) {

        pub_str.append( QChar::fromLatin1( datas->at(PUBLISHER_ADD + pub_it ) ) );
    }

    exportDialog->setPublisher( pub_str.simplified() );

    // Editor's Name (EN)
    QString en_str;
    for ( quint16 en_it = 0; en_it < EDITORS_NAME_LENGTH; en_it++ ) {

        en_str.append( QChar::fromLatin1( datas->at(EDITORS_NAME_ADD + en_it ) ) );
    }

    exportDialog->setEditorsName( en_str.simplified() );

    // Editor's Contact Details (ECD)
    QString ecd_str;
    for ( quint16 ecd_it = 0; ecd_it < EDITORS_CONTACT_DETAILS_LENGTH; ecd_it++ ) {

        ecd_str.append( QChar::fromLatin1( datas->at(EDITORS_CONTACT_DETAILS_ADD + ecd_it ) ) );
    }

    exportDialog->setEditorsContact( ecd_str.simplified() );
}

// Open and read a "stl" file and return a subtitles list
QList<MySubtitles> EbuParser::open(MyFileReader file) {

    QList<quint8>* datas = file.data();

    QList<MySubtitles> sub_list;

    // Retrieive the time code rate
    qint16 time_code_rate;
    QString tcr_str;
    tcr_str.append(datas->at(DISK_FORMAT_CODE_ADD + DFC_TIME_CODE_RATE_OFFSET));
    tcr_str.append(datas->at(DISK_FORMAT_CODE_ADD + DFC_TIME_CODE_RATE_OFFSET + 1));
    time_code_rate = tcr_str.toInt();

    // Retrieive char code table number
    QString char_code_table_str;
    char_code_table_str.append( QChar::fromLatin1( datas->at(CHARACTER_CODE_TABLE_NUMBER_ADD) ) );
    char_code_table_str.append( QChar::fromLatin1( datas->at(CHARACTER_CODE_TABLE_NUMBER_ADD + 1) ) );

    QTextCodec* char_codec;
    if ( char_code_table_str == CCT_CYRILIC ) {
        char_codec = QTextCodec::codecForName("ISO 8859-5");
    }
    else if ( char_code_table_str == CCT_ARABIC ) {
        char_codec = QTextCodec::codecForName("ISO 8859-6");
    }
    else if ( char_code_table_str == CCT_GREEK ) {
        char_codec = QTextCodec::codecForName("ISO 8859-7");
    }
    else if ( char_code_table_str == CCT_HEBREW ) {
        char_codec = QTextCodec::codecForName("ISO 8859-8");
    }

    // Number max of row for "Open" sub
    QString row_nbr_max_str;
    qint16 row_nbr_max;

    if ( datas->at( MAX_NUMBER_OF_DISPLAYABLE_ROW_ADD ) != GSI_UNUSED_BYTE ) {
        row_nbr_max_str.append( QChar::fromLatin1( datas->at(MAX_NUMBER_OF_DISPLAYABLE_ROW_ADD) ) );

        if ( datas->at( MAX_NUMBER_OF_DISPLAYABLE_ROW_ADD + 1 ) != GSI_UNUSED_BYTE ) {
            row_nbr_max_str.append( QChar::fromLatin1( datas->at(MAX_NUMBER_OF_DISPLAYABLE_ROW_ADD + 1) ) );
        }
        row_nbr_max = row_nbr_max_str.toInt();
    }
    else {
        row_nbr_max = 23;
    }


    // Retrieive the max characters per row
    QString char_per_row_max_str;
    qint16 char_per_row_max;
    if ( datas->at(MAX_NUMBER_OF_DISPLAYABLE_CHAR_IN_ROW_ADD ) != GSI_UNUSED_BYTE ) {
        char_per_row_max_str.append( QChar::fromLatin1( datas->at(MAX_NUMBER_OF_DISPLAYABLE_CHAR_IN_ROW_ADD ) ) );

        if ( datas->at(MAX_NUMBER_OF_DISPLAYABLE_CHAR_IN_ROW_ADD + 1 ) != GSI_UNUSED_BYTE ) {
            char_per_row_max_str.append( QChar::fromLatin1( datas->at(MAX_NUMBER_OF_DISPLAYABLE_CHAR_IN_ROW_ADD + 1) ) );
        }
        char_per_row_max = char_per_row_max_str.toInt();
    }
    else {
        char_per_row_max = qApp->property("prop_MaxCharPerLine").toInt();
    }


    MySubtitles* current_sub = new MySubtitles;
    bool is_new_sub = true;

    qreal v_pos_step;
    qreal font_size_pt;
    QString current_line;
    quint8 justification_code;
    quint8 first_row_v_pos;
    TextLine new_text_line;
    TextFont new_text_font;
    bool is_double_height = false;
    bool has_color_tag_open = false;
    bool has_italic_tag_open = false;
    bool has_underlined_tag_open = false;

    // For each tti block
    for ( qint32 i = TTI_ADD; i < datas->count(); i = i + TTI_BLOCK_LENGTH ) {

        QList<quint8> sub_text_data;

        sub_text_data.append(datas->mid(i, TTI_BLOCK_LENGTH));

        // If it's new sub - not a extension block
        if ( is_new_sub == true ) {

            // Read start time code
            QString start_time_str;
            for ( qint16 tc_it = 0; tc_it < TIME_CODE_IN_LENGTH; tc_it++ ) {

                QString number = QString::number( sub_text_data.at(TIME_CODE_IN_ADD + tc_it) );

                if ( number.count() == 1 ) {
                    number = number.prepend("0");
                }

                if ( tc_it != 3 ) {
                    number = number.append(":");
                }

                start_time_str.append(number);
            }

            current_sub->setStartTime( MyAttributesConverter::framesToTimeHMSms(start_time_str, time_code_rate) );

            // Read end time code
            QString end_time_str;
            for ( qint16 tc_it = 0; tc_it < TIME_CODE_OUT_LENGTH; tc_it++ ) {

                QString number = QString::number( sub_text_data.at(TIME_CODE_OUT_ADD + tc_it) );

                if ( number.count() == 1 ) {
                    number = number.prepend("0");
                }

                if ( tc_it != 3 ) {
                    number = number.append(":");
                }

                end_time_str.append(number);
            }

            current_sub->setEndTime( MyAttributesConverter::framesToTimeHMSms(end_time_str, time_code_rate) );

            // Duation auto ON
            current_sub->setDurationAuto(true);

            // Compute font size
            if ( datas->at(DISPLAY_STANDARD_CODE_ADD) == DSC_OPEN_SUB ) {
                font_size_pt = (11.0 / (qreal)row_nbr_max) * 72.0;
            }
            else {
                font_size_pt = (11.0 / 23.0) * 72.0;
            }

            new_text_font.setFontSize( QString::number( qRound(font_size_pt) ) );

            // Set font default color to white
            new_text_font.setFontColor("FFFFFFFF");

            // Compute the vertical position step
            first_row_v_pos = sub_text_data.at(VERTICAL_POSITION_ADD);

            if ( datas->at(DISPLAY_STANDARD_CODE_ADD) == DSC_OPEN_SUB ) {

                v_pos_step = 100.0 / (qreal)row_nbr_max;
            }
            else {

                v_pos_step = 100.0 / 23.0;
            }

            // Retrieive the horizontal alignment
            justification_code = sub_text_data.at(JUSTIFICATION_CODE_ADD);

            if ( ( justification_code == 0x00 ) || ( justification_code == 0x01 ) ){
                new_text_line.setTextHAlign("left");
                new_text_line.setTextHPosition("0.0");
            }
            else if ( justification_code == 0x02 ) {
                new_text_line.setTextHAlign("center");
                new_text_line.setTextHPosition("0.0");
            }
            else if ( justification_code == 0x03 ) {
                new_text_line.setTextHAlign("right");
                new_text_line.setTextHPosition("0.0");
            }

            // Add a new line to the current sub
            current_sub->setText(new_text_line, new_text_font);

            current_line = "";
        }

        quint16 crlf_count = 0;

        // Read each characters in the text field
        for ( qint32 j = TEXT_FIELD_ADD; j < (TEXT_FIELD_ADD + TEXT_FIELD_LENGTH); j++ ) {

            quint8 current_char = sub_text_data.at(j);

            // Reset the successive line break counter
            if ( ( current_char != 0x8A ) && ( crlf_count > 0 ) ) {
                crlf_count = 0;
            }

            // Is Teletext code
            if ( (current_char >= 0x00 ) && ( current_char <= 0x1F ) ) {

                // Double height code
                if ( current_char == 0x0D ) {

                    if ( is_double_height == false ) {

                        is_double_height = true;
                    }
                }
                // Normal height code
                else if ( current_char == 0x0C ) {

                    if ( is_double_height == true ) {

                        is_double_height = false;
                    }
                }
                // Color code
                else if ( ( current_char >= 0x00 ) && ( current_char <= 0x07 ) ) {

                    QString color_str;

                    if( current_char == ALPHA_BLACK ) {

                        color_str == "FF000000";
                    }
                    else if ( current_char == ALPHA_RED ) {

                        color_str = "FFFF0000";
                    }
                    else if ( current_char == ALPHA_GREEN ) {

                        color_str = "FF00FF00";
                    }
                    else if ( current_char == ALPHA_YELLOW ) {

                        color_str = "FFFFFF00";
                    }
                    else if ( current_char == ALPHA_BLUE ) {

                        color_str = "FF0000FF";
                    }
                    else if ( current_char == ALPHA_MAGENTA ) {

                        color_str = "FFFF00FF";
                    }
                    else if ( current_char == ALPHA_CYAN ) {

                        color_str = "FF00FFFF";
                    }
                    else if ( current_char == ALPHA_WHITE ) {

                        color_str = "FFFFFFFF";
                    }

                    if ( current_line == "" ) {
                        current_sub->text().last().Font().setFontColor(color_str);
                    }
                    else {

                        if ( has_color_tag_open ) {
                            current_line.append("</font>");
                        }

                        color_str = color_str.remove(0, 2);
                        current_line.append("<font color = #" +color_str +">");
                        has_color_tag_open = true;
                    }
                }
            }
            // Is unicode character
            else if ( (current_char >= 0x20 ) && ( current_char <= 0x7E ) ) {

                // If "space" char
                if ( current_char == 0x20 ) {

                    if ( current_line == "" ) {

                        if ( justification_code == 0x00 ) {
                            qreal current_h_pos = current_sub->text().last().textHPosition().toDouble();
                            current_h_pos = current_h_pos + (100.0 / (qreal)char_per_row_max);
                            current_sub->text().last().setTextHPosition( QString::number(char_per_row_max, 'f', 1));
                        }
                    }
                    else {
                        current_line.append(current_char);
                    }
                }
                // currency sign replace dollar sign for Latin charset
                else if ( current_char == 0x24 ) {

                    if ( char_code_table_str == CCT_LATIN ) {
                        current_line.append(0x00A4);
                    }
                    else {
                        current_line.append(current_char);
                    }
                }
                else {
                    current_line.append(current_char);
                }
            }
            // Basic character control parameters
            else if ( ( current_char >= 0x80 ) && ( current_char <= 0x85 ) ) {

                if ( current_char == 0x80 ) {
                    if ( has_italic_tag_open != true ) {
                        current_line.append("<i>");
                        has_italic_tag_open = true;
                    }
                }
                else if ( current_char == 0x81 ) {
                    if ( has_italic_tag_open == true ) {
                        current_line.append("</i>");
                        has_italic_tag_open = false;
                    }
                }
                else if ( current_char == 0x82 ) {
                    if ( has_underlined_tag_open != true ) {
                        current_line.append("<u>");
                        has_underlined_tag_open = true;
                    }
                }
                else if ( current_char == 0x83 ) {
                    if ( has_underlined_tag_open == true ) {
                        current_line.append("</u>");
                        has_underlined_tag_open = false;
                    }
                }
            }
            // Is linbreak
            else if ( current_char == 0x8A ) {

                // Avoid double linebreak with "double height" option
                if ( crlf_count > 0 ) {
                    // Nothing to do
                }
                else {

                    if ( has_color_tag_open ) {
                        current_line.append("</font>");
                        has_color_tag_open = false;
                    }
                    if ( has_underlined_tag_open ) {
                        current_line.append("</u>");
                        has_underlined_tag_open = false;
                    }
                    if ( has_italic_tag_open ) {
                        current_line.append("</i>");
                        has_italic_tag_open = false;
                    }

                    current_sub->text().last().setLine( MyAttributesConverter::plainTextToHtml(current_line) );
                    current_line = "";

                    current_sub->setText(new_text_line, new_text_font);

                    crlf_count = 0;

                    if ( is_double_height == true ) {

                        is_double_height = false;
                        crlf_count++;
                    }
                }
            }
            // Is specific encoded charater
            else if ( ( current_char >= 0xA0 ) && ( current_char <= 0xFF ) ) {

                // Latin characters
                if ( char_code_table_str == CCT_LATIN ) {

                    // Combined char
                    if ( current_char >= 0xC0 && current_char <= 0xCF ) {

                        if ( (j + 1) < sub_text_data.count() ) {

                            quint16 c = current_char;
                            c = (c * 0x100) + sub_text_data.at(j + 1);

                            quint16 combined_char = latinCombinigChar(c);
                            if ( combined_char != 0 ) {

                                current_line.append(combined_char);
                                j++;
                            }
                        }
                    }
                    else {
                        current_line.append( latinToUnicodeChar(current_char) );
                    }
                }
                // Cyrillic, Arabic, Greek, Hebrew characters
                else {
                    QByteArray current_char_byte;
                    current_char_byte.append(current_char);
                    QString unicode_char = char_codec->toUnicode(current_char_byte);
                    current_line.append(unicode_char);
                }
            }
            // Is end of block
            else if ( current_char == 0x8F ) {

                // Set vertical positions
                quint16 line_count = current_sub->text().count();

                // The first row is bottom aligned and compute position of the rows above it
                if ( first_row_v_pos >= (row_nbr_max / 2) ) {

                    // Retrieve last row position and compute
                    quint16 last_row_v_pos;

                    if ( is_double_height ) {

                        last_row_v_pos = first_row_v_pos + ( 2 * line_count ) - 1;
                    }
                    else {

                        last_row_v_pos = first_row_v_pos + line_count - 1;
                    }

                    if ( last_row_v_pos > row_nbr_max ) {
                        last_row_v_pos = row_nbr_max;
                    }

                    for ( quint16 line_nbr = 0; line_nbr < line_count; line_nbr++ ) {

                        quint16 row_v_pos;

                        row_v_pos = row_nbr_max - last_row_v_pos + line_nbr;
                        current_sub->text()[line_count - line_nbr - 1].setTextVAlign("bottom");
                        current_sub->text()[line_count - line_nbr - 1].setTextVPosition(QString::number(( (qreal)row_v_pos * v_pos_step), 'f', 1));
                    }
                }
                // The first row is top aligned
                else {

                    // Compute the rows position with first row position as reference
                    for ( quint16 line_nbr = 0; line_nbr < line_count; line_nbr++ ) {

                        quint16 row_v_pos;

                        row_v_pos = first_row_v_pos + line_nbr - 1;
                        current_sub->text()[line_nbr].setTextVAlign("top");
                        current_sub->text()[line_nbr].setTextVPosition(QString::number(( (qreal)row_v_pos * v_pos_step), 'f', 1));
                    }
                }

                // Close the calor tag, if anyone is open
                if ( has_color_tag_open ) {
                    current_line.append("</font>");
                    has_color_tag_open = false;
                }
                if ( has_italic_tag_open ) {
                    current_line.append("</i>");
                    has_italic_tag_open = false;
                }
                if ( has_underlined_tag_open ) {
                    current_line.append("</u>");
                    has_underlined_tag_open = false;
                }

                // Set the new line to current sub (html formated)
                current_sub->text().last().setLine( MyAttributesConverter::plainTextToHtml(current_line) );
                break;
            }
        }

        // Last bloack for the current subtitle
        if ( sub_text_data.at(EXTENSION_BLOCK_NUMBER_ADD) == 0xFF ) {

            // Save the current subtitle and create a new one
            sub_list.append(*current_sub);
            current_sub = new MySubtitles;
            is_new_sub = true;
        }
        else {
            is_new_sub = false;
        }
    }

    return sub_list;

}

// Save the given subtitles list in a "stl" file
void EbuParser::save(MyFileWriter &file, QList<MySubtitles> subtitlesList, SubExportDialog *exportDialog) {

    // GSI block
    QList<quint8> gsi_block_data;

    for ( quint16 i = 0; i < GSI_BLOCK_LENGTH; i++ ) {

        gsi_block_data.append(GSI_UNUSED_BYTE);
    }

    // Code Page Number (CPN)
    QString cpn_str = CPN_MULTILINGUAL;
    QByteArray cpn_bytes = cpn_str.toLatin1();

    for ( quint16 cpn_it = 0; cpn_it < CODE_PAGE_NUMBER_LENGTH; cpn_it++ ) {

        if ( cpn_it < cpn_bytes.size() ) {
            gsi_block_data.replace((CODE_PAGE_NUMBER_ADD + cpn_it), cpn_bytes[cpn_it]);
        }
    }

    // Disk Format Code (DFC)
    QString dfc_str = exportDialog->diskFormatCode();
    if ( dfc_str == "" ) {
        dfc_str = "25";
    }
    quint16 time_code_rate = dfc_str.toInt();
    dfc_str.prepend("STL");
    dfc_str.append(".01");
    QByteArray dfc_bytes= dfc_str.toLatin1();

    for ( quint16 dfc_it = 0; dfc_it < DISK_FORMAT_CODE_LENGTH; dfc_it++ ) {

        if ( dfc_it < dfc_bytes.size() ) {
            gsi_block_data.replace((DISK_FORMAT_CODE_ADD + dfc_it), dfc_bytes[dfc_it]);
        }
    }

    // Display Standard Code (DSC)
    bool teletext = false;
    QString dsc_str = exportDialog->displayMode();

    if ( dsc_str == "Open subtitling" ) {
        teletext = false;
        gsi_block_data.replace(DISPLAY_STANDARD_CODE_ADD, DSC_OPEN_SUB);
    }
    else if ( dsc_str ==  "Level-1 teletext" ) {
        teletext = true;
        gsi_block_data.replace(DISPLAY_STANDARD_CODE_ADD, DSC_LEVEL1_TELETEXT);
    }
    else if ( dsc_str ==  "Level-2 teletext"  ) {
        teletext = true;
        gsi_block_data.replace(DISPLAY_STANDARD_CODE_ADD, DSC_LEVEL2_TELETEXT);
    }

    bool is_chars_double_height = false;

    // In Teletext mode, the characters can be draw on two row height
    if ( teletext = true ) {
        is_chars_double_height = exportDialog->isDoubleHeight();
    }

    // Character Code Table (CCT) number
    QString cct_str;
    QString cct_name = exportDialog->charCodeTable();
    QTextCodec *char_codec;
    if ( cct_name == "Cyrillic" ) {
        cct_str = CCT_CYRILIC;
        char_codec = QTextCodec::codecForName("ISO 8859-5");
    }
    else if ( cct_name == "Arabic" ) {
        cct_str = CCT_ARABIC;
        char_codec = QTextCodec::codecForName("ISO 8859-6");
    }
    else if ( cct_name == "Greek" ) {
        cct_str = CCT_GREEK;
        char_codec = QTextCodec::codecForName("ISO 8859-7");
    }
    else if ( cct_name == "Hebrew" ) {
        cct_str = CCT_HEBREW;
        char_codec = QTextCodec::codecForName("ISO 8859-8");
    }
    else {
        cct_str = CCT_LATIN;
    }

    QByteArray cct_bytes = cct_str.toLatin1();

    for ( quint16 cct_it = 0; cct_it < CHARACTER_CODE_TABLE_NUMBER_LENGTH; cct_it++ ) {

        if ( cct_it < cct_bytes.size() ) {
            gsi_block_data.replace((CHARACTER_CODE_TABLE_NUMBER_ADD + cct_it), cct_bytes[cct_it]);
        }
    }

    // Language Code (LC)
    quint8 language_code = exportDialog->languageCode();
    //    QString lc_str = QString::number(language_code, 16);
    QString lc_str;
    lc_str.sprintf("%02.2X", language_code);
    QByteArray lc_bytes = lc_str.toLatin1();

    for ( quint16 lc_it = 0; lc_it < LANGUAGE_CODE_LENGTH; lc_it++ ) {

        if ( lc_it < lc_bytes.size() ) {
            gsi_block_data.replace((LANGUAGE_CODE_ADD + lc_it), lc_bytes[lc_it]);
        }
    }

    // Original Programme Title (OPT)
    QString opt_str = exportDialog->progTitleVO();
    if ( opt_str != "" ) {
        QByteArray opt_bytes = opt_str.toLatin1();

        for ( quint16 opt_it = 0; opt_it < ORIGINAL_PROGRAMME_TITLE_LENGTH; opt_it++ ) {

            if ( opt_it < opt_bytes.size() ) {
                gsi_block_data.replace((ORIGINAL_PROGRAMME_TITLE_ADD + opt_it), opt_bytes[opt_it]);
            }
        }
    }

    // Original Episode Title (OET)
    QString oet_str = exportDialog->epTitleVO();
    if ( oet_str != "" ) {
        QByteArray oet_bytes = oet_str.toLatin1();

        for ( quint16 oet_it = 0; oet_it < ORIGINAL_EPISODE_TITLE_LENGTH; oet_it++ ) {

            if ( oet_it < oet_bytes.size() ) {
                gsi_block_data.replace((ORIGINAL_EPISODE_TITLE_ADD + oet_it), oet_bytes[oet_it]);
            }
        }
    }

    // Translated Programme Title (TPT)
    QString tpt_str = exportDialog->progTitleTR();
    if ( tpt_str != "" ) {
        QByteArray tpt_bytes = tpt_str.toLatin1();

        for ( quint16 tpt_it = 0; tpt_it < TRANSLATED_PROGRAMME_TITLE_LENGTH; tpt_it++ ) {

            if ( tpt_it < tpt_bytes.size() ) {
                gsi_block_data.replace((TRANSLATED_PROGRAMME_TITLE_ADD + tpt_it), tpt_bytes[tpt_it]);
            }
        }
    }

    // Translated Episode Title (TET)
    QString tet_str = exportDialog->epTitleTR();
    if ( tet_str != "" ) {
        QByteArray tet_bytes = tet_str.toLatin1();

        for ( quint16 tet_it = 0; tet_it < TRANSLATED_EPISODE_TITLE_LENGTH; tet_it++ ) {

            if ( tet_it < tet_bytes.size() ) {
                gsi_block_data.replace((TRANSLATED_EPISODE_TITLE_ADD + tet_it), tet_bytes[tet_it]);
            }
        }
    }

    // Translator's Name (TN)
    QString tn_str = exportDialog->translatorName();
    if ( tn_str != "" ) {
        QByteArray tn_bytes = tn_str.toLatin1();

        for ( quint16 tn_it = 0; tn_it < TRANSLATORS_NAME_LENGTH; tn_it++ ) {

            if ( tn_it < tn_bytes.size() ) {
                gsi_block_data.replace((TRANSLATORS_NAME_ADD + tn_it), tn_bytes[tn_it]);
            }
        }
    }

    // Translator's Contact Details (TCD)
    QString tcd_str = exportDialog->translatorContact();
    if ( tcd_str != "" ) {
        QByteArray tcd_bytes = tcd_str.toLatin1();

        for ( quint16 tcd_it = 0; tcd_it < TRANSLATORS_CONTACT_DETAILS_LENGTH; tcd_it++ ) {

            if ( tcd_it < tcd_bytes.size() ) {
                gsi_block_data.replace((TRANSLATORS_CONTACT_DETAILS_ADD + tcd_it), tcd_bytes[tcd_it]);
            }
        }
    }

    // Subtitle List Reference Code (SLR)
    QString slr_str = exportDialog->subListRef();
    if ( slr_str != "" ) {
        QByteArray slr_bytes = slr_str.toLatin1();

        for ( quint16 slr_it = 0; slr_it < SUBTITLE_LIST_REFERENCE_CODE_LENGTH; slr_it++ ) {

            if ( slr_it < slr_bytes.size() ) {
                gsi_block_data.replace((SUBTITLE_LIST_REFERENCE_CODE_ADD + slr_it), slr_bytes[slr_it]);
            }
        }
    }

    // Revision Date (RD)
    QDate current_date = QDate::currentDate();

    QString date_str = current_date.toString("yyMMdd");;
    QByteArray date_bytes = date_str.toLatin1();

    for ( quint16 date_it = 0; date_it < REVISION_DATE_LENGTH; date_it++ ) {

        if ( date_it < date_bytes.size() ) {
            gsi_block_data.replace(REVISION_DATE_ADD + date_it, date_bytes[date_it]);
        }
    }

    // Creation Date (CD)
    QString cd_str = exportDialog->creationDate();
    QByteArray cd_bytes;
    if ( cd_str != "" ) {
        cd_bytes = cd_str.toLatin1();
    }
    else {
        cd_bytes = date_str.toLatin1();
    }

    for ( quint16 cd_it = 0; cd_it < CREATION_DATE_LENGTH; cd_it++ ) {

        if ( cd_it < cd_bytes.size() ) {
            gsi_block_data.replace((CREATION_DATE_ADD + cd_it), cd_bytes[cd_it]);
        }
    }


    // Revision Number (RN)
    quint16 revision_nbr = exportDialog->revisionNbr();

    if ( revision_nbr > 99 ) {
        revision_nbr = 1;
    }

    QString rn_str = QString::number(revision_nbr);
    QByteArray rn_bytes = rn_str.toLatin1();

    for ( quint16 rn_it = 0; rn_it < REVISION_NUMBER_LENGTH; rn_it++ ) {

        if ( rn_it < rn_bytes.size() ) {
            gsi_block_data.replace(REVISION_NUMBER_ADD + rn_it, rn_bytes[rn_it]);
        }
    }

    // Total Number of Subtitle Groups (TNG) ( only 1 - group not supported)
    gsi_block_data.replace(TOTAL_NUMBER_OF_SUBTITLE_GROUPS_ADD, QChar('1').toLatin1());

    // Maximum Number of Displayable Characters in any text row (mnr)
    quint16 char_per_row_max  = qApp->property("prop_MaxCharPerLine").toInt();
    QString mnc_str = QString::number(char_per_row_max);
    QByteArray mnc_bytes = mnc_str.toLatin1();

    for ( quint16 mnc_it = 0; mnc_it < MAX_NUMBER_OF_DISPLAYABLE_CHAR_IN_ROW_LENGTH; mnc_it++ ) {

        if ( mnc_it < mnc_bytes.size() ) {
            gsi_block_data.replace(MAX_NUMBER_OF_DISPLAYABLE_CHAR_IN_ROW_ADD + mnc_it, mnc_bytes[mnc_it]);
        }
    }

    // Maximum Number of Displayable Rows (MNR)
    quint16 max_row = exportDialog->maxRow().toInt();
    QString mnr_str = QString::number(max_row);
    QByteArray mnr_bytes = mnr_str.toLatin1();

    for ( quint16 mnr_it = 0; mnr_it < MAX_NUMBER_OF_DISPLAYABLE_ROW_LENGTH; mnr_it++ ) {

        if ( mnr_it < mnr_bytes.size() ) {
            gsi_block_data.replace(MAX_NUMBER_OF_DISPLAYABLE_ROW_ADD + mnr_it, mnr_bytes[mnr_it]);
        }
    }

    // Time Code: Status (TCS)
    gsi_block_data.replace(TIME_CODE_STATUS_ADD, QChar('1').toLatin1());

    // Time Code: Start-of-Programme (TCP)
    QString tcp_str = exportDialog->startOfProg();

    if ( tcp_str == "" ) {
        tcp_str = "00000000";
    }

    QByteArray tcp_bytes = tcp_str.toLatin1();

    for ( quint16 tcp_it = 0; tcp_it < TIME_CODE_START_OF_PROG_LENGTH; tcp_it++ ) {

        if ( tcp_it < tcp_bytes.size() ) {
            gsi_block_data.replace((TIME_CODE_START_OF_PROG_ADD + tcp_it), tcp_bytes[tcp_it]);
        }
    }

    // Total Number of Disks (TND)
    gsi_block_data.replace(TOTAL_NUMBER_OF_DISK_ADD, QChar('1').toLatin1());

    // Disk Sequence Number (DSN)
    gsi_block_data.replace(DISK_SEQUENCE_NUMBER_ADD, QChar('1').toLatin1());

    // Country of Origin (CO)
    QString co_str = exportDialog->countryOrigin();
    if ( co_str != "" ) {

        QByteArray co_bytes = co_str.toLatin1();

        for ( quint16 co_it = 0; co_it < COUNTRY_OF_ORIGINE_LENGTH; co_it++ ) {

            if ( co_it < co_bytes.size() ) {
                gsi_block_data.replace((COUNTRY_OF_ORIGINE_ADD + co_it), co_bytes[co_it]);
            }
        }
    }

    // Publisher (PUB)
    QString pub_str = exportDialog->publisher();
    if ( pub_str != "" ) {

        QByteArray pub_bytes = pub_str.toLatin1();

        for ( quint16 pub_it = 0; pub_it < PUBLISHER_LENGTH; pub_it++ ) {

            if ( pub_it < pub_bytes.size() ) {
                gsi_block_data.replace((PUBLISHER_ADD + pub_it), pub_bytes[pub_it]);
            }
        }
    }

    // Editor's Name (EN)
    QString en_str = exportDialog->editorName();
    if ( en_str != "" ) {

        QByteArray en_bytes = en_str.toLatin1();

        for ( quint16 en_it = 0; en_it < EDITORS_NAME_LENGTH; en_it++ ) {

            if ( en_it < en_bytes.size() ) {
                gsi_block_data.replace((EDITORS_NAME_ADD + en_it), en_bytes[en_it]);
            }
        }
    }

    // Editor's Contact Details (ECD)
    QString ecd_str = exportDialog->editorContact();
    if ( ecd_str != "" ) {

        QByteArray ecd_bytes = ecd_str.toLatin1();

        for ( quint16 ecd_it = 0; ecd_it < EDITORS_CONTACT_DETAILS_LENGTH; ecd_it++ ) {

            if ( ecd_it < ecd_bytes.size() ) {
                gsi_block_data.replace((EDITORS_CONTACT_DETAILS_ADD + ecd_it), ecd_bytes[ecd_it]);
            }
        }
    }

    // User-Defined Area (UDA)
    // Not used

    // Total Number of Subtitles (TNS)
    quint32 sub_count = subtitlesList.count();

    if ( sub_count > 0 ) {

        QString sub_count_str = QString::number(sub_count);
        QByteArray sub_count_bytes = sub_count_str.toLatin1();

        for ( quint16 sub_count_it = 0; sub_count_it < TOTAL_NUMBER_OF_SUBTITLES_LENGTH; sub_count_it++ ) {

            if ( sub_count_it < sub_count_bytes.size() ) {
                gsi_block_data.replace(TOTAL_NUMBER_OF_SUBTITLES_ADD + sub_count_it, sub_count_bytes[sub_count_it]);
            }
        }
    }
    else {
        // No subtitles to write
        file.writeRawData(gsi_block_data);
        return;
    }

    // Time Code: First in-cue (TCF)
    QString first_in_cue_str = MyAttributesConverter::timeHMSmsToFrames(subtitlesList.first().startTime(), time_code_rate);
    first_in_cue_str = first_in_cue_str.remove(":");
    QByteArray first_in_cue_bytes = first_in_cue_str.toLatin1();

    for ( quint16 fic_it = 0; fic_it < TIME_CODE_FIRST_IN_CUE_LENGTH; fic_it++ ) {

        if ( fic_it < first_in_cue_bytes.size() ) {
            gsi_block_data.replace(TIME_CODE_FIRST_IN_CUE_ADD + fic_it, first_in_cue_bytes[fic_it]);
        }
    }


    // TTI blocks
    quint16 sub_nbr = 0;
    quint16 tti_block_count = 0;

    for ( qint32 sub_it = 0; sub_it < subtitlesList.count(); sub_it++ ) {

        QList<quint8> tti_block_data;
        MySubtitles current_subtitle = subtitlesList.at(sub_it);

        // Init the tti block
        for ( quint16 j = 0; j < TTI_BLOCK_LENGTH; j++ ) {

            tti_block_data.append(SUB_TEXT_UNUSED);
        }

        // Subtitle Group Number (SGN) (always 0x00 - not supported)
        tti_block_data.replace(SUB_GROUP_NUMBER_ADD, 0x00);

        // Subtitle Number (SN)
        tti_block_data.replace(SUB_NUMBER_ADD, (quint8)( (sub_nbr >> 8) & 0x0000FFFF ) );
        tti_block_data.replace(SUB_NUMBER_ADD + 1, (quint8)( sub_nbr & 0x0000FFFF ) );

        // Extension Block Number (EBN)
        quint16 extension_block_number = 0;
        tti_block_data.replace(EXTENSION_BLOCK_NUMBER_ADD, 0xFF);

        // Cumulative Status (CS) (always 0x00 - not supported)
        tti_block_data.replace(CUMULATIV_STATUS_ADD, CS_NOT);

        // Time Code In (TCI)
        QString timecode_in_str = MyAttributesConverter::timeHMSmsToFrames(current_subtitle.startTime(), time_code_rate);
        QStringList timecode_in_strlist = timecode_in_str.split(":");

        for ( quint16 tc_in_it = 0; tc_in_it < TIME_CODE_IN_LENGTH; tc_in_it++ ) {

            if ( tc_in_it < timecode_in_strlist.size() ) {
                tti_block_data.replace(TIME_CODE_IN_ADD + tc_in_it, (quint8)timecode_in_strlist[tc_in_it].toInt());
            }
        }

        // Time Code Out (TCO)
        QString timecode_out_str = MyAttributesConverter::timeHMSmsToFrames(current_subtitle.endTime(), time_code_rate);
        QStringList timecode_out_strlist = timecode_out_str.split(":");

        for ( quint16 tc_out_it = 0; tc_out_it < TIME_CODE_OUT_LENGTH; tc_out_it++ ) {

            if ( tc_out_it < timecode_out_strlist.size() ) {
                tti_block_data.replace(TIME_CODE_OUT_ADD + tc_out_it, (quint8)timecode_out_strlist[tc_out_it].toInt());
            }
        }

        // Vertical Position (VP)
        quint16 first_line_row;
        quint16 last_line_row;
        QString v_align = current_subtitle.text().first().textVAlign();


        qreal row_step = 100.0 / (qreal)max_row;

        if ( v_align == "top" ) {

            qreal first_line_v_pos_percent = current_subtitle.text().first().textVPosition().toDouble();
            first_line_row = qRound(first_line_v_pos_percent / row_step) + 1;

            // Display characters on two rows - "pair" rows and last row can't be used
            if ( is_chars_double_height == true ) {

                // If the first line is the last
                if ( first_line_row == max_row ) {
                    first_line_row--;
                }
            }
        }
        else {

            qreal last_line_v_pos_percent = current_subtitle.text().last().textVPosition().toDouble();
            last_line_row = max_row - qRound(last_line_v_pos_percent / row_step);

            // Display characters on two rows - last row can't be used
            if ( is_chars_double_height == true ) {

                // If the last line is the last
                if ( last_line_row == max_row ) {
                    last_line_row--;
                }

                first_line_row = last_line_row - ( 2 * ( current_subtitle.text().count() - 1 ) );
            }
            else {

                first_line_row = last_line_row - ( current_subtitle.text().count() - 1 );
            }
        }

        tti_block_data.replace(VERTICAL_POSITION_ADD, (quint8)first_line_row);


        // Justification Code (JC)
        QString h_align = current_subtitle.text().first().textHAlign();

        // Check if all the line of the subtitle have the same alignment "left", "center" or "right"
        for ( quint16 line_nbr = 0; line_nbr < current_subtitle.text().count(); line_nbr++ ) {

            QString current_h_align = current_subtitle.text()[line_nbr].textHAlign();
            if ( current_h_align == h_align ) {

                qint16 current_h_pos = (qint16)current_subtitle.text()[line_nbr].textHPosition().toDouble();

                if ( current_h_pos == 0 ) {
                    continue;
                }
                else {
                    h_align = "none";
                    break;
                }
            }
            else {
                h_align = "none";
                break;
            }
        }

        quint8 justification_code;
        qreal column_step = 0;

        if ( h_align == "left" ) {
            justification_code = JC_LEFT;
        }
        else if ( h_align == "center" ) {
            justification_code = JC_CENTER;
        }
        else if ( h_align == "right" ) {
            justification_code = JC_RIGHT;
        }
        else {
            justification_code = JC_UNCHANGED;
            column_step = 100.0 / (qreal)char_per_row_max;
        }

        tti_block_data.replace(JUSTIFICATION_CODE_ADD, justification_code);


        // Comment Flag (CF)
        tti_block_data.replace(COMMENT_FLAG_ADD, CF_DATA);


        // Text Field (TF)
        QList<quint8> text_field;

        for ( quint16 line_it = 0; line_it < current_subtitle.text().count(); line_it++ ) {

            TextLine current_line = current_subtitle.text().at(line_it);
            TextFont current_font = current_line.Font();

            // Set CR/LF - twice to jump a line if "Double height" chars
            if ( line_it != 0 ) {
                text_field.append(0x8A);
                if ( is_chars_double_height == true ) {
                    text_field.append(0x8A);
                }
            }

            // "Double height"
            if ( is_chars_double_height == true ) {
                text_field.append(DOUBLE_HEIGHT);
            }

            // Setbackground color code
            bool color_changed = false;
            if ( current_font.fontBackgroundEffect() == "yes" ) {

                QString background_color_str = MyAttributesConverter::simplifyColorStr( current_font.fontBackgroundEffectColor() );
                quint8 background_color_code = colorStrToTeletextCode(background_color_str);

                if ( background_color_code != ALPHA_BLACK ) {
                    text_field.append(background_color_code);
                    text_field.append(NEW_BACKGROUND);
                    color_changed = true;
                }
            }

            // Set foreground color code
            QString line_color = MyAttributesConverter::simplifyColorStr( current_font.fontColor() );
            quint8 current_color_code = colorStrToTeletextCode(line_color);
            if ( ( current_color_code != ALPHA_WHITE ) || ( color_changed == true ) ) {
                text_field.append(current_color_code);
            }

            // Set horizontal pasition
            if ( justification_code == JC_UNCHANGED ) {

                qreal h_pos = current_line.textHPosition().toDouble();
                quint16 nbr_of_spacing = (quint16)(h_pos / column_step);

                for ( quint16 spacing_it = 0; spacing_it < nbr_of_spacing; spacing_it++ ) {
                    text_field.append(0x20);
                }
            }

            // Set "Start box" code if teletext
            if ( teletext == true ) {
                text_field.append(START_BOX);
                text_field.append(START_BOX);
            }

            // Set text field
            QXmlStreamReader reader(current_line.Line());

            bool italic_on = false;
            bool underline_on = false;

            while ( !reader.atEnd() ) {

                QString unicode_string;

                switch (reader.readNext()) {
                case QXmlStreamReader::StartElement:

                    if ( reader.name() == "span" ) {

                        QXmlStreamAttributes attributes = reader.attributes();

                        if ( attributes.hasAttribute( QStringLiteral("style") ) ) {

                            QString style_str = attributes.value("style").toString();

                            if ( style_str.contains("color") ) {

                                QString color_str = style_str.mid( (style_str.indexOf("#") + 1), 6 );
                                color_str.prepend("FF");
                                color_str = MyAttributesConverter::simplifyColorStr(color_str);
                                quint8 color_code = colorStrToTeletextCode(color_str);

                                if ( color_code != current_color_code ) {

                                    current_color_code = color_code;
                                    text_field.append(current_color_code);
                                }
                            }

                            if ( style_str.contains("italic") ) {

                                if ( teletext == false ) {

                                    text_field.append(ITALIC_ON);
                                    italic_on = true;
                                }
                            }

                            if ( style_str.contains("underline") ) {

                                if ( teletext == false ) {

                                    text_field.append(UNDERLINE_ON);
                                    underline_on = true;
                                }
                            }
                        }
                    }
                    break;

                case QXmlStreamReader::EndElement:

                    if ( reader.name() == "span" ) {
                        if ( italic_on == true ) {
                            text_field.append(ITALIC_OFF);
                        }
                        if ( underline_on == true ) {
                            text_field.append(UNDERLINE_OFF);
                        }
                    }
                    break;

                case QXmlStreamReader::Characters:

                    unicode_string = reader.text().toString();

                    if ( cct_str == CCT_LATIN ) {

                        for ( quint16 chars_it = 0; chars_it < unicode_string.count(); chars_it++ ) {

                            quint16 current_combining_latin_char = unicodeTolatinCombinigChar(unicode_string[chars_it].unicode());

                            if ( current_combining_latin_char != 0 ) {
                                text_field.append( (quint8)((current_combining_latin_char >> 8) & 0x0000FFFF) );
                                text_field.append( (quint8)( current_combining_latin_char & 0x0000FFFF) );
                                continue;
                            }

                            quint8 current_latin_char = unicodeToLatinChar(unicode_string[chars_it].unicode());

                            if ( current_latin_char != 0x00 ) {
                                text_field.append(current_latin_char);
                                continue;
                            }

                            current_latin_char = unicode_string[chars_it].toLatin1();

                            if ( ( current_latin_char >= 0x20 ) && ( current_latin_char <= 0x7E ) ) {
                                text_field.append(current_latin_char);
                            }
                        }
                    }
                    // Cyrillic, Arabic, Greek, Hebrew characters
                    else {

                        QByteArray encoded_characters = char_codec->fromUnicode(unicode_string);

                        for ( quint16 chars_it = 0; chars_it < encoded_characters.count(); chars_it++ ) {

                            text_field.append(encoded_characters[chars_it]);
                        }
                    }

                    break;

                default:
                    break;
                }
            }
        }

        // Write the current subtitles characters list into tti bloks
        for ( qint16 tf_it = 0; tf_it < text_field.count(); tf_it++ ) {

            // Push the text field data into the tti block
            if ( tf_it < TEXT_FIELD_LENGTH ) {

                tti_block_data.replace(TEXT_FIELD_ADD + tf_it, text_field.at(tf_it));
            }
            // If the current tti block is full
            else {

                // Set extension block number
                tti_block_data.replace(EXTENSION_BLOCK_NUMBER_ADD, extension_block_number);
                extension_block_number++;
                // Append the current tti block to the gsi block
                gsi_block_data.append(tti_block_data);
                tti_block_count++;

                // Reset tti block text field
                for ( quint16 j = TEXT_FIELD_ADD; j < TTI_BLOCK_LENGTH; j++ ) {

                    tti_block_data.replace(j, SUB_TEXT_UNUSED);
                    text_field.removeFirst();
                }

                tf_it = -1;
            }
        }

        // Append the last tti block for the current subtitle
        tti_block_data.replace(EXTENSION_BLOCK_NUMBER_ADD, 0xFF);
        gsi_block_data.append(tti_block_data);

        tti_block_count++;
        sub_nbr++;
    }

    // Total Number of TTI Blocks (TNB)
    QString tnb_str = QString::number(tti_block_count);
    QByteArray tnb_bytes = tnb_str.toLatin1();

    for ( quint16 tnb_it = 0; tnb_it < TOTAL_NUMBER_OF_TTI_BLOCKS_LENGTH; tnb_it++ ) {

        if ( tnb_it < tnb_bytes.size() ) {
            gsi_block_data.replace(TOTAL_NUMBER_OF_TTI_BLOCKS_ADD + tnb_it, tnb_bytes[tnb_it]);
        }
    }

    // Write the data in the file
    file.writeRawData(gsi_block_data);
}

// Not used - used readGsiBlock() instead
bool EbuParser::readSample(MyFileReader file) {
    return false;
}

// Convert a color AARRGGBB into Teletext color code
quint8 EbuParser::colorStrToTeletextCode(QString colorStr) {

    quint8 color_code;

    // Black
    if( colorStr == "FF000000" ) {

        color_code = ALPHA_BLACK;
    }
    // Red
    else if ( colorStr == "FFFF0000" ) {

        color_code = ALPHA_RED;
    }
    // Green
    else if ( colorStr == "FF00FF00" ) {

        color_code = ALPHA_GREEN;
    }
    // Yellow
    else if ( colorStr == "FFFFFF00" ) {

        color_code = ALPHA_YELLOW;
    }
    // Blue
    else if ( colorStr == "FF0000FF" ) {

        color_code = ALPHA_BLUE;
    }
    // Magenta
    else if ( colorStr == "FFFF00FF" ) {

        color_code = ALPHA_MAGENTA;
    }
    // Cyan
    else if ( colorStr == "FF00FFFF" ) {

        color_code = ALPHA_CYAN;
    }
    // White
    else {

        color_code = ALPHA_WHITE;
    }

    return color_code;
}

// Convert an "ISO 6937-2" character into Unicode16 character
quint16 EbuParser::latinToUnicodeChar(quint8 inputCode) {

    switch (inputCode) {
    case 0xA0:
        return 0x00A0; // NO-BREAK SPACE
    case 0xA1:
        return 0x00A1; // INVERTED EXCLAMATION MARK
    case 0xA2:
        return 0x00A2; // CENT SIGN
    case 0xA3:
        return 0x00A3; // POUND SIGN
    case 0x0A4:
        return 0x0024; // DOLLAR SIGN
    case 0xA5:
        return 0x00A5; // YEN SIGN
        // 0xA6 (This position shall not be used)
    case 0xA7:
        return 0x00A7; // SECTION SIGN
    case 0xA8:
        // 0xA8 (This position shall not be used)
    case 0xA9:
        return 0x2018; // LEFT SINGLE QUOTATION MARK
    case 0xAA:
        return 0x201C; // LEFT DOUBLE QUOTATION MARK
    case 0xAB:
        return 0x00AB; // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
    case 0xAC:
        return 0x2190; // LEFTWARDS ARROW
    case 0xAD:
        return 0x2191; // UPWARDS ARROW
    case 0xAE:
        return 0x2192; // RIGHTWARDS ARROW
    case 0xAF:
        return 0x2193; // DOWNWARDS ARROW

    case 0xB0:
        return 0x00B0; // DEGREE SIGN
    case 0xB1:
        return 0x00B1; // PLUS-MINUS SIGN
    case 0xB2:
        return 0x00B2; // SUPERSCRIPT TWO
    case 0xB3:
        return 0x00B3; // SUPERSCRIPT THREE
    case 0xB4:
        return 0x00D7; // MULTIPLICATION SIGN
    case 0xB5:
        return 0x00B5; // MICRO SIGN
    case 0xB6:
        return 0x00B6; // PILCROW SIGN
    case 0xB7:
        return 0x00B7; // MIDDLE DOT
    case 0xB8:
        return 0x00F7; // DIVISION SIGN
    case 0xB9:
        return 0x2019; // RIGHT SINGLE QUOTATION MARK
    case 0xBA:
        return 0x201D; // RIGHT DOUBLE QUOTATION MARK
    case 0xBB:
        return 0x00BB; // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
    case 0xBC:
        return 0x00BC; // VULGAR FRACTION ONE QUARTER
    case 0xBD:
        return 0x00BD; // VULGAR FRACTION ONE HALF
    case 0xBE:
        return 0x00BE; // VULGAR FRACTION THREE QUARTERS
    case 0xBF:
        return 0x00BF; // INVERTED QUESTION MARK

        // 0xC0 to 0xCF diacritic characters

    case 0xD0:
        return 0x2015; // HORIZONTAL BAR
    case 0xD1:
        return 0x00B9; // SUPERSCRIPT ONE
    case 0xD2:
        return 0x2117; // REGISTERED SIGN
    case 0xD3:
        return 0x00A9; // COPYRIGHT SIGN
    case 0xD4:
        return 0x00AE; // TRADE MARK SIGN
    case 0xD5:
        return 0x266A; // EIGHTH NOTE
    case 0xD6:
        return 0x00AC; // NOT SIGN
    case 0xD7:
        return 0x00A6; // BROKEN BAR
        // 0xD8 (This position shall not be used)
        // 0xD9 (This position shall not be used)
        // 0xDA (This position shall not be used)
        // 0xDB (This position shall not be used)
    case 0xDC:
        return 0x215B; // VULGAR FRACTION ONE EIGHTH
    case 0xDD:
        return 0x215C; // VULGAR FRACTION THREE EIGHTHS
    case 0xDE:
        return 0x215D; // VULGAR FRACTION FIVE EIGHTHS
    case 0xDF:
        return 0x215E; // VULGAR FRACTION SEVEN EIGHTHS

    case 0xE0:
        return 0x2126; // OHM SIGN
    case 0xE1:
        return 0x00C6; // LATIN CAPITAL LETTER AE
    case 0xE2:
        return 0x0110; // LATIN CAPITAL LETTER D WITH STROKE
    case 0xE3:
        return 0x00AA; // FEMININE ORDINAL INDICATOR
    case 0xE4:
        return 0x0126; // LATIN CAPITAL LETTER H WITH STROKE
        // 0xE5 (This position shall not be used)
    case 0xE6:
        return 0x0132; // LATIN CAPITAL LIGATURE IJ
    case 0xE7:
        return 0x013F; // LATIN CAPITAL LETTER L WITH MIDDLE DOT
    case 0xE8:
        return 0x0141; // LATIN CAPITAL LETTER L WITH STROKE
    case 0xE9:
        return 0x00D8; // LATIN CAPITAL LETTER O WITH STROKE
    case 0xEA:
        return 0x0152; // LATIN CAPITAL LIGATURE OE
    case 0xEB:
        return 0x00BA; // MASCULINE ORDINAL INDICATOR
    case 0xEC:
        return 0x00DE; // LATIN CAPITAL LETTER THORN
    case 0xED:
        return 0x0166; // LATIN CAPITAL LETTER T WITH STROKE
    case 0xEE:
        return 0x014A; // LATIN CAPITAL LETTER ENG
    case 0xEF:
        return 0x0149; // LATIN SMALL LETTER N PRECEDED BY APOSTROPHE

    case 0xF0:
        return 0x0138; // LATIN SMALL LETTER KRA
    case 0xF1:
        return 0x00E6; // LATIN SMALL LETTER AE
    case 0xF2:
        return 0x0111; // LATIN SMALL LETTER D WITH STROKE
    case 0xF3:
        return 0x00F0; // LATIN SMALL LETTER ETH
    case 0xF4:
        return 0x0127; // LATIN SMALL LETTER H WITH STROKE
        //    case 0xF5:
        //        return 0x0131; // LATIN SMALL LETTER DOTLESS I
    case 0xF5:
        return 0x2142; // TURNED SANS-SERIF CAPITAL L
    case 0xF6:
        return 0x0133; // LATIN SMALL LIGATURE IJ
    case 0xF7:
        return 0x0140; // LATIN SMALL LETTER L WITH MIDDLE DOT
    case 0xF8:
        return 0x0142; // LATIN SMALL LETTER L WITH STROKE
    case 0xF9:
        return 0x00F8; // LATIN SMALL LETTER O WITH STROKE
    case 0xFA:
        return 0x0153; // LATIN SMALL LIGATURE OE
    case 0xFB:
        return 0x00DF; // LATIN SMALL LETTER SHARP S
    case 0xFC:
        return 0x00FE; // LATIN SMALL LETTER THORN
    case 0xFD:
        return 0x0167; // LATIN SMALL LETTER T WITH STROKE
    case 0xFE:
        return 0x014B; // LATIN SMALL LETTER ENG
    case 0xFF:
        return 0x00AD; // SOFT HYPHEN$

    default:
        return (quint16)inputCode;
    }
}

// Convert an "ISO 6937-2" combinig 16 bits character into Unicode16 character
quint16 EbuParser::latinCombinigChar(quint16 inputCode) {

    switch (inputCode) {
    // 0xC0 (This position shall not be used)

    // 0xC1 non-spacing grave accent
    case 0xC141:
        return 0x00C0; // LATIN CAPITAL LETTER A WITH GRAVE
    case 0xC145:
        return 0x00C8; // LATIN CAPITAL LETTER E WITH GRAVE
    case 0xC149:
        return 0x00CC; // LATIN CAPITAL LETTER I WITH GRAVE
    case 0xC14F:
        return 0x00D2; // LATIN CAPITAL LETTER O WITH GRAVE
    case 0xC155:
        return 0x00D9; // LATIN CAPITAL LETTER U WITH GRAVE
    case 0xC161:
        return 0x00E0; // LATIN SMALL LETTER A WITH GRAVE
    case 0xC165:
        return 0x00E8; // LATIN SMALL LETTER E WITH GRAVE
    case 0xC169:
        return 0x00EC; // LATIN SMALL LETTER I WITH GRAVE
    case 0xC16F:
        return 0x00F2; // LATIN SMALL LETTER O WITH GRAVE
    case 0xC175:
        return 0x00F9; // LATIN SMALL LETTER U WITH GRAVE

        // 0xC2 non-spacing acute accent
    case 0xC220:
        return 0x00B4; // ACUTE ACCENT
    case 0xC241:
        return 0x00C1; // LATIN CAPITAL LETTER A WITH ACUTE
    case 0xC243:
        return 0x0106; // LATIN CAPITAL LETTER C WITH ACUTE
    case 0xC245:
        return 0x00C9; // LATIN CAPITAL LETTER E WITH ACUTE
    case 0xC249:
        return 0x00CD; // LATIN CAPITAL LETTER I WITH ACUTE
    case 0xC24C:
        return 0x0139; // LATIN CAPITAL LETTER L WITH ACUTE
    case 0xC24E:
        return 0x0143; // LATIN CAPITAL LETTER N WITH ACUTE
    case 0xC24F:
        return 0x00D3; // LATIN CAPITAL LETTER O WITH ACUTE
    case 0xC252:
        return 0x0154; // LATIN CAPITAL LETTER R WITH ACUTE
    case 0xC253:
        return 0x015A; // LATIN CAPITAL LETTER S WITH ACUTE
    case 0xC255:
        return 0x00DA; // LATIN CAPITAL LETTER U WITH ACUTE
    case 0xC259:
        return 0x00DD; // LATIN CAPITAL LETTER Y WITH ACUTE
    case 0xC25A:
        return 0x0179; // LATIN CAPITAL LETTER Z WITH ACUTE
    case 0xC261:
        return 0x00E1; // LATIN SMALL LETTER A WITH ACUTE
    case 0xC263:
        return 0x0107; // LATIN SMALL LETTER C WITH ACUTE
    case 0xC265:
        return 0x00E9; // LATIN SMALL LETTER E WITH ACUTE
    case 0xC267:
        return 0x01F5; // LATIN SMALL LETTER G WITH CEDILLA(4)
    case 0xC269:
        return 0x00ED; // LATIN SMALL LETTER I WITH ACUTE
    case 0xC26C:
        return 0x013A; // LATIN SMALL LETTER L WITH ACUTE
    case 0xC26E:
        return 0x0144; // LATIN SMALL LETTER N WITH ACUTE
    case 0xC26F:
        return 0x00F3; // LATIN SMALL LETTER O WITH ACUTE
    case 0xC272:
        return 0x0155; // LATIN SMALL LETTER R WITH ACUTE
    case 0xC273:
        return 0x015B; // LATIN SMALL LETTER S WITH ACUTE
    case 0xC275:
        return 0x00FA; // LATIN SMALL LETTER U WITH ACUTE
    case 0xC279:
        return 0x00FD; // LATIN SMALL LETTER Y WITH ACUTE
    case 0xC27A:
        return 0x017A; // LATIN SMALL LETTER Z WITH ACUTE

        // 0xC3 non-spacing circumflex accent
    case 0xC341:
        return 0x00C2; // LATIN CAPITAL LETTER A WITH CIRCUMFLEX
    case 0xC343:
        return 0x0108; // LATIN CAPITAL LETTER C WITH CIRCUMFLEX
    case 0xC345:
        return 0x00CA; // LATIN CAPITAL LETTER E WITH CIRCUMFLEX
    case 0xC347:
        return 0x011C; // LATIN CAPITAL LETTER G WITH CIRCUMFLEX
    case 0xC348:
        return 0x0124; // LATIN CAPITAL LETTER H WITH CIRCUMFLEX
    case 0xC349:
        return 0x00CE; // LATIN CAPITAL LETTER I WITH CIRCUMFLEX
    case 0xC34A:
        return 0x0134; // LATIN CAPITAL LETTER J WITH CIRCUMFLEX
    case 0xC34F:
        return 0x00D4; // LATIN CAPITAL LETTER O WITH CIRCUMFLEX
    case 0xC353:
        return 0x015C; // LATIN CAPITAL LETTER S WITH CIRCUMFLEX
    case 0xC355:
        return 0x00DB; // LATIN CAPITAL LETTER U WITH CIRCUMFLEX
    case 0xC357:
        return 0x0174; // LATIN CAPITAL LETTER W WITH CIRCUMFLEX
    case 0xC359:
        return 0x0176; // LATIN CAPITAL LETTER Y WITH CIRCUMFLEX
    case 0xC361:
        return 0x00E2; // LATIN SMALL LETTER A WITH CIRCUMFLEX
    case 0xC363:
        return 0x0109; // LATIN SMALL LETTER C WITH CIRCUMFLEX
    case 0xC365:
        return 0x00EA; // LATIN SMALL LETTER E WITH CIRCUMFLEX
    case 0xC367:
        return 0x011D; // LATIN SMALL LETTER G WITH CIRCUMFLEX
    case 0xC368:
        return 0x0125; // LATIN SMALL LETTER H WITH CIRCUMFLEX
    case 0xC369:
        return 0x00EE; // LATIN SMALL LETTER I WITH CIRCUMFLEX
    case 0xC36A:
        return 0x0135; // LATIN SMALL LETTER J WITH CIRCUMFLEX
    case 0xC36F:
        return 0x00F4; // LATIN SMALL LETTER O WITH CIRCUMFLEX
    case 0xC373:
        return 0x015D; // LATIN SMALL LETTER S WITH CIRCUMFLEX
    case 0xC375:
        return 0x00FB; // LATIN SMALL LETTER U WITH CIRCUMFLEX
    case 0xC377:
        return 0x0175; // LATIN SMALL LETTER W WITH CIRCUMFLEX
    case 0xC379:
        return 0x0177; // LATIN SMALL LETTER Y WITH CIRCUMFLEX

        // 0xC4 non-spacing tilde
    case 0xC441:
        return 0x00C3; // LATIN CAPITAL LETTER A WITH TILDE
    case 0xC449:
        return 0x0128; // LATIN CAPITAL LETTER I WITH TILDE
    case 0xC44E:
        return 0x00D1; // LATIN CAPITAL LETTER N WITH TILDE
    case 0xC44F:
        return 0x00D5; // LATIN CAPITAL LETTER O WITH TILDE
    case 0xC455:
        return 0x0168; // LATIN CAPITAL LETTER U WITH TILDE
    case 0xC461:
        return 0x00E3; // LATIN SMALL LETTER A WITH TILDE
    case 0xC469:
        return 0x0129; // LATIN SMALL LETTER I WITH TILDE
    case 0xC46E:
        return 0x00F1; // LATIN SMALL LETTER N WITH TILDE
    case 0xC46F:
        return 0x00F5; // LATIN SMALL LETTER O WITH TILDE
    case 0xC475:
        return 0x0169; // LATIN SMALL LETTER U WITH TILDE

        // 0xC5 non-spacing macron
    case 0xC541:
        return 0x0100; // LATIN CAPITAL LETTER A WITH MACRON
    case 0xC545:
        return 0x0112; // LATIN CAPITAL LETTER E WITH MACRON
    case 0xC549:
        return 0x012A; // LATIN CAPITAL LETTER I WITH MACRON
    case 0xC54F:
        return 0x014C; // LATIN CAPITAL LETTER O WITH MACRON
    case 0xC555:
        return 0x016A; // LATIN CAPITAL LETTER U WITH MACRON
    case 0xC561:
        return 0x0101; // LATIN SMALL LETTER A WITH MACRON
    case 0xC565:
        return 0x0113; // LATIN SMALL LETTER E WITH MACRON
    case 0xC569:
        return 0x012B; // LATIN SMALL LETTER I WITH MACRON
    case 0xC56F:
        return 0x014D; // LATIN SMALL LETTER O WITH MACRON
    case 0xC575:
        return 0x016B; // LATIN SMALL LETTER U WITH MACRON

        // 0xC6 non-spacing breve
    case 0xC620:
        return 0x02D8; // BREVE
    case 0xC641:
        return 0x0102; // LATIN CAPITAL LETTER A WITH BREVE
    case 0xC647:
        return 0x011E; // LATIN CAPITAL LETTER G WITH BREVE
    case 0xC655:
        return 0x016C; // LATIN CAPITAL LETTER U WITH BREVE
    case 0xC661:
        return 0x0103; // LATIN SMALL LETTER A WITH BREVE
    case 0xC667:
        return 0x011F; // LATIN SMALL LETTER G WITH BREVE
    case 0xC675:
        return 0x016D; // LATIN SMALL LETTER U WITH BREVE

        // 0xC7 non-spacing dot above
    case 0xC743:
        return 0x010A; // LATIN CAPITAL LETTER C WITH DOT ABOVE
    case 0xC745:
        return 0x0116; // LATIN CAPITAL LETTER E WITH DOT ABOVE
    case 0xC747:
        return 0x0120; // LATIN CAPITAL LETTER G WITH DOT ABOVE
    case 0xC749:
        return 0x0130; // LATIN CAPITAL LETTER I WITH DOT ABOVE
    case 0xC75A:
        return 0x017B; // LATIN CAPITAL LETTER Z WITH DOT ABOVE
    case 0xC763:
        return 0x010B; // LATIN SMALL LETTER C WITH DOT ABOVE
    case 0xC765:
        return 0x0117; // LATIN SMALL LETTER E WITH DOT ABOVE
    case 0xC767:
        return 0x0121; // LATIN SMALL LETTER G WITH DOT ABOVE
    case 0xC77A:
        return 0x017C; // LATIN SMALL LETTER Z WITH DOT ABOVE

        // 0xC8 non-spacing diaeresis
    case 0xC820:
        return 0x00A8; // DIAERESIS
    case 0xC841:
        return 0x00C4; // LATIN CAPITAL LETTER A WITH DIAERESIS
    case 0xC845:
        return 0x00CB; // LATIN CAPITAL LETTER E WITH DIAERESIS
    case 0xC849:
        return 0x00CF; // LATIN CAPITAL LETTER I WITH DIAERESIS
    case 0xC84F:
        return 0x00D6; // LATIN CAPITAL LETTER O WITH DIAERESIS
    case 0xC855:
        return 0x00DC; // LATIN CAPITAL LETTER U WITH DIAERESIS
    case 0xC859:
        return 0x0178; // LATIN CAPITAL LETTER Y WITH DIAERESIS
    case 0xC861:
        return 0x00E4; // LATIN SMALL LETTER A WITH DIAERESIS
    case 0xC865:
        return 0x00EB; // LATIN SMALL LETTER E WITH DIAERESIS
    case 0xC869:
        return 0x00EF; // LATIN SMALL LETTER I WITH DIAERESIS
    case 0xC86F:
        return 0x00F6; // LATIN SMALL LETTER O WITH DIAERESIS
    case 0xC875:
        return 0x00FC; // LATIN SMALL LETTER U WITH DIAERESIS
    case 0xC879:
        return 0x00FF; // LATIN SMALL LETTER Y WITH DIAERESIS

        // 0xC9 (This position shall not be used)

        // 0xCA non-spacing ring above
    case 0xCA20:
        return 0x02DA; // RING ABOVE
    case 0xCA41:
        return 0x00C5; // LATIN CAPITAL LETTER A WITH RING ABOVE
    case 0xCAAD:
        return 0x016E; // LATIN CAPITAL LETTER U WITH RING ABOVE
    case 0xCA61:
        return 0x00E5; // LATIN SMALL LETTER A WITH RING ABOVE
    case 0xCA75:
        return 0x016F; // LATIN SMALL LETTER U WITH RING ABOVE

        // 0xCB non-spacing cedilla
    case 0xCB20:
        return 0x00B8; // CEDILLA
    case 0xCB43:
        return 0x00C7; // LATIN CAPITAL LETTER C WITH CEDILLA
    case 0xCB47:
        return 0x0122; // LATIN CAPITAL LETTER G WITH CEDILLA
    case 0xCB4B:
        return 0x0136; // LATIN CAPITAL LETTER K WITH CEDILLA
    case 0xCB4C:
        return 0x013B; // LATIN CAPITAL LETTER L WITH CEDILLA
    case 0xCB4E:
        return 0x0145; // LATIN CAPITAL LETTER N WITH CEDILLA
    case 0xCB52:
        return 0x0156; // LATIN CAPITAL LETTER R WITH CEDILLA
    case 0xCB53:
        return 0x015E; // LATIN CAPITAL LETTER S WITH CEDILLA
    case 0xCB54:
        return 0x0162; // LATIN CAPITAL LETTER T WITH CEDILLA
    case 0xCB63:
        return 0x00E7; // LATIN SMALL LETTER C WITH CEDILLA
        //          case 0xCB67: return 0x0123; // small g with cedilla
    case 0xCB6B:
        return 0x0137; // LATIN SMALL LETTER K WITH CEDILLA
    case 0xCB6C:
        return 0x013C; // LATIN SMALL LETTER L WITH CEDILLA
    case 0xCB6E:
        return 0x0146; // LATIN SMALL LETTER N WITH CEDILLA
    case 0xCB72:
        return 0x0157; // LATIN SMALL LETTER R WITH CEDILLA
    case 0xCB73:
        return 0x015F; // LATIN SMALL LETTER S WITH CEDILLA
    case 0xCB74:
        return 0x0163; // LATIN SMALL LETTER T WITH CEDILLA

        // 0xCC to do


        // 0xCD non-spacing double acute accent
    case 0xCD4F:
        return 0x0150; // LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
    case 0xCD55:
        return 0x0170; // LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
    case 0xCD6F:
        return 0x0151; // LATIN SMALL LETTER O WITH DOUBLE ACUTE
    case 0xCD75:
        return 0x0171; // LATIN SMALL LETTER U WITH DOUBLE ACUTE

        // 0xCE non-spacing ogonek
    case 0xCE20:
        return 0x02DB; // ogonek
    case 0xCE41:
        return 0x0104; // LATIN CAPITAL LETTER A WITH OGONEK
    case 0xCE45:
        return 0x0118; // LATIN CAPITAL LETTER E WITH OGONEK
    case 0xCE49:
        return 0x012E; // LATIN CAPITAL LETTER I WITH OGONEK
    case 0xCE55:
        return 0x0172; // LATIN CAPITAL LETTER U WITH OGONEK
    case 0xCE61:
        return 0x0105; // LATIN SMALL LETTER A WITH OGONEK
    case 0xCE65:
        return 0x0119; // LATIN SMALL LETTER E WITH OGONEK
    case 0xCE69:
        return 0x012F; // LATIN SMALL LETTER I WITH OGONEK
    case 0xCE75:
        return 0x0173; // LATIN SMALL LETTER U WITH OGONEK

        // 0xCF non-spacing caron
    case 0xCF20:
        return 0x02C7; // CARON
    case 0xCF43:
        return 0x010C; // LATIN CAPITAL LETTER C WITH CARON
    case 0xCF44:
        return 0x010E; // LATIN CAPITAL LETTER D WITH CARON
    case 0xCF45:
        return 0x011A; // LATIN CAPITAL LETTER E WITH CARON
    case 0xCF4C:
        return 0x013D; // LATIN CAPITAL LETTER L WITH CARON
    case 0xCF4E:
        return 0x0147; // LATIN CAPITAL LETTER N WITH CARON
    case 0xCF52:
        return 0x0158; // LATIN CAPITAL LETTER R WITH CARON
    case 0xCF53:
        return 0x0160; // LATIN CAPITAL LETTER S WITH CARON
    case 0xCF54:
        return 0x0164; // LATIN CAPITAL LETTER T WITH CARON
    case 0xCF5A:
        return 0x017D; // LATIN CAPITAL LETTER Z WITH CARON
    case 0xCF63:
        return 0x010D; // LATIN SMALL LETTER C WITH CARON
    case 0xCF64:
        return 0x010F; // LATIN SMALL LETTER D WITH CARON
    case 0xCF65:
        return 0x011B; // LATIN SMALL LETTER E WITH CARON
    case 0xCF6C:
        return 0x013E; // LATIN SMALL LETTER L WITH CARON
    case 0xCF6E:
        return 0x0148; // LATIN SMALL LETTER N WITH CARON
    case 0xCF72:
        return 0x0159; // LATIN SMALL LETTER R WITH CARON
    case 0xCF73:
        return 0x0161; // LATIN SMALL LETTER S WITH CARON
    case 0xCF74:
        return 0x0165; // LATIN SMALL LETTER T WITH CARON
    case 0xCF7A:
        return 0x017E; // LATIN SMALL LETTER Z WITH CARON

    default:
        return 0;
    }
}

// Convert a Unicode 16 bits character into "ISO 6937-2" 8 bits char
quint8 EbuParser::unicodeToLatinChar(quint16 inputCode) {

    switch (inputCode) {
    case 0x00A0:
        return 0xA0; // NO-BREAK SPACE
    case 0x00A1:
        return 0xA1; // INVERTED EXCLAMATION MARK
    case 0x00A2:
        return 0xA2; // CENT SIGN
    case 0x00A3:
        return 0xA3; // POUND SIGN
    case 0x00A4:
        return 0x24; // CURRENCY SIGN
    case 0x0024:
        return 0xA4; // DOLLAR SIGN
    case 0x00A5:
        return 0xA5; // YEN SIGN
        // 0xA6 (This position shall not be used)
    case 0x00A7:
        return 0xA7; // SECTION SIGN
        // 0xA8 (This position shall not be used)
    case 0x2018:
        return 0xA9; // LEFT SINGLE QUOTATION MARK
    case 0x201C:
        return 0xAA; // LEFT DOUBLE QUOTATION MARK
    case 0x00AB:
        return 0xAB; // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
    case 0x2190:
        return 0xAC; // LEFTWARDS ARROW
    case 0x2191:
        return 0xAD; // UPWARDS ARROW
    case 0x2192:
        return 0xAE; // RIGHTWARDS ARROW
    case 0x2193:
        return 0xAF; // DOWNWARDS ARROW

    case 0x00B0:
        return 0xB0; // DEGREE SIGN
    case 0x00B1:
        return 0xB1; // PLUS-MINUS SIGN
    case 0x00B2:
        return 0xB2; // SUPERSCRIPT TWO
    case 0x00B3:
        return 0xB3; // SUPERSCRIPT THREE
    case 0x00D7:
        return 0xB4; // MULTIPLICATION SIGN
    case 0x00B5:
        return 0xB5; // MICRO SIGN
    case 0x00B6:
        return 0xB6; // PILCROW SIGN
    case 0x00B7:
        return 0xB7; // MIDDLE DOT
    case 0x00F7:
        return 0xB8; // DIVISION SIGN
    case 0x2019:
        return 0xB9; // RIGHT SINGLE QUOTATION MARK
    case 0x201D:
        return 0xBA; // RIGHT DOUBLE QUOTATION MARK
    case 0x00BB:
        return 0xBB; // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
    case 0x00BC:
        return 0xBC; // VULGAR FRACTION ONE QUARTER
    case 0x00BD:
        return 0xBD; // VULGAR FRACTION ONE HALF
    case 0x00BE:
        return 0xBE; // VULGAR FRACTION THREE QUARTERS
    case 0x00BF:
        return 0xBF; // INVERTED QUESTION MARK

        // 0xC0 to 0xCF diacritic characters

    case 0x2015:
        return 0xD0; // HORIZONTAL BAR
    case 0x00B9:
        return 0xD1; // SUPERSCRIPT ONE
    case 0x2117:
        return 0xD2; // REGISTERED SIGN
    case 0x00A9:
        return 0xD3; // COPYRIGHT SIGN
    case 0x00AE:
        return 0xD4; // TRADE MARK SIGN
    case 0x266A:
        return 0xD5; // EIGHTH NOTE
    case 0x00AC:
        return 0xD6; // NOT SIGN
    case 0x00A6:
        return 0xD7; // BROKEN BAR
        // 0xD8 (This position shall not be used)
        // 0xD9 (This position shall not be used)
        // 0xDA (This position shall not be used)
        // 0xDB (This position shall not be used)
    case 0x215B:
        return 0xDC; // VULGAR FRACTION ONE EIGHTH
    case 0x215C:
        return 0xDD; // VULGAR FRACTION THREE EIGHTHS
    case 0x215D:
        return 0xDE; // VULGAR FRACTION FIVE EIGHTHS
    case 0x215E:
        return 0xDF; // VULGAR FRACTION SEVEN EIGHTHS

    case 0x2126:
        return 0xE0; // OHM SIGN
    case 0x00C6:
        return 0xE1; // LATIN CAPITAL LETTER AE
    case 0x0110:
        return 0xE2; // LATIN CAPITAL LETTER D WITH STROKE
    case 0x00AA:
        return 0xE3; // FEMININE ORDINAL INDICATOR
    case 0x0126:
        return 0xE4; // LATIN CAPITAL LETTER H WITH STROKE
        // 0xE5 (This position shall not be used)
    case 0x0132:
        return 0xE6; // LATIN CAPITAL LIGATURE IJ
    case 0x013F:
        return 0xE7; // LATIN CAPITAL LETTER L WITH MIDDLE DOT
    case 0x0141:
        return 0xE8; // LATIN CAPITAL LETTER L WITH STROKE
    case 0x00D8:
        return 0xE9; // LATIN CAPITAL LETTER O WITH STROKE
    case 0x0152:
        return 0xEA; // LATIN CAPITAL LIGATURE OE
    case 0x00BA:
        return 0xEB; // MASCULINE ORDINAL INDICATOR
    case 0x00DE:
        return 0xEC; // LATIN CAPITAL LETTER THORN
    case 0x0166:
        return 0xED; // LATIN CAPITAL LETTER T WITH STROKE
    case 0x014A:
        return 0xEE; // LATIN CAPITAL LETTER ENG
    case 0x0149:
        return 0xEF; // LATIN SMALL LETTER N PRECEDED BY APOSTROPHE

    case 0x0138:
        return 0xF0; // LATIN SMALL LETTER KRA
    case 0x00E6:
        return 0xF1; // LATIN SMALL LETTER AE
    case 0x0111:
        return 0xF2; // LATIN SMALL LETTER D WITH STROKE
    case 0x00F0:
        return 0xF3; // LATIN SMALL LETTER ETH
    case 0x0127:
        return 0xF4; // LATIN SMALL LETTER H WITH STROKE
        //    case 0x0131:
        //        return 0xF5; // LATIN SMALL LETTER DOTLESS I
    case 0x2142:
        return 0xF5; // TURNED SANS-SERIF CAPITAL L
    case 0x0133:
        return 0xF6; // LATIN SMALL LIGATURE IJ
    case 0x0140:
        return 0xF7; // LATIN SMALL LETTER L WITH MIDDLE DOT
    case 0x0142:
        return 0xF8; // LATIN SMALL LETTER L WITH STROKE
    case 0x00F8:
        return 0xF9; // LATIN SMALL LETTER O WITH STROKE
    case 0x0153:
        return 0xFA; // LATIN SMALL LIGATURE OE
    case 0x00DF:
        return 0xFB; // LATIN SMALL LETTER SHARP S
    case 0x00FE:
        return 0xFC; // LATIN SMALL LETTER THORN
    case 0x0167:
        return 0xFD; // LATIN SMALL LETTER T WITH STROKE
    case 0x014B:
        return 0xFE; // LATIN SMALL LETTER ENG
    case 0x00AD:
        return 0xFF; // SOFT HYPHEN$

    default:
        return 0x00;
    }
}

// Convert Unicode character to "ISO 6937-2" combining chararacters
quint16 EbuParser::unicodeTolatinCombinigChar(quint16 inputCode) {

    switch (inputCode) {
    // 0xC0 (This position shall not be used)

    // 0xC1 non-spacing grave accent
    case 0x00C0:
        return 0xC141; // LATIN CAPITAL LETTER A WITH GRAVE
    case 0x00C8:
        return 0xC145; // LATIN CAPITAL LETTER E WITH GRAVE
    case 0x00CC:
        return 0xC149; // LATIN CAPITAL LETTER I WITH GRAVE
    case 0x00D2:
        return 0xC14F; // LATIN CAPITAL LETTER O WITH GRAVE
    case 0x00D9:
        return 0xC155; // LATIN CAPITAL LETTER U WITH GRAVE
    case 0x00E0:
        return 0xC161; // LATIN SMALL LETTER A WITH GRAVE
    case 0x00E8:
        return 0xC165; // LATIN SMALL LETTER E WITH GRAVE
    case 0x00EC:
        return 0xC169; // LATIN SMALL LETTER I WITH GRAVE
    case 0x00F2:
        return 0xC16F; // LATIN SMALL LETTER O WITH GRAVE
    case 0x00F9:
        return 0xC175; // LATIN SMALL LETTER U WITH GRAVE

        // 0xC2 non-spacing acute accent
    case 0x00B4:
        return 0xC220; // ACUTE ACCENT
    case 0x00C1:
        return 0xC241; // LATIN CAPITAL LETTER A WITH ACUTE
    case 0x0106:
        return 0xC243; // LATIN CAPITAL LETTER C WITH ACUTE
    case 0x00C9:
        return 0xC245; // LATIN CAPITAL LETTER E WITH ACUTE
    case 0x00CD:
        return 0xC249; // LATIN CAPITAL LETTER I WITH ACUTE
    case 0x0139:
        return 0xC24C; // LATIN CAPITAL LETTER L WITH ACUTE
    case 0x0143:
        return 0xC24E; // LATIN CAPITAL LETTER N WITH ACUTE
    case 0x00D3:
        return 0xC24F; // LATIN CAPITAL LETTER O WITH ACUTE
    case 0x0154:
        return 0xC252; // LATIN CAPITAL LETTER R WITH ACUTE
    case 0x015A:
        return 0xC253; // LATIN CAPITAL LETTER S WITH ACUTE
    case 0x00DA:
        return 0xC255; // LATIN CAPITAL LETTER U WITH ACUTE
    case 0x00DD:
        return 0xC259; // LATIN CAPITAL LETTER Y WITH ACUTE
    case 0x0179:
        return 0xC25A; // LATIN CAPITAL LETTER Z WITH ACUTE
    case 0x00E1:
        return 0xC261; // LATIN SMALL LETTER A WITH ACUTE
    case 0x0107:
        return 0xC263; // LATIN SMALL LETTER C WITH ACUTE
    case 0x00E9:
        return 0xC265; // LATIN SMALL LETTER E WITH ACUTE
    case 0x01F5:
        return 0xC267; // LATIN SMALL LETTER G WITH CEDILLA(4)
    case 0x00ED:
        return 0xC269; // LATIN SMALL LETTER I WITH ACUTE
    case 0x013A:
        return 0xC26C; // LATIN SMALL LETTER L WITH ACUTE
    case 0x0144:
        return 0xC26E; // LATIN SMALL LETTER N WITH ACUTE
    case 0x00F3:
        return 0xC26F; // LATIN SMALL LETTER O WITH ACUTE
    case 0x0155:
        return 0xC272; // LATIN SMALL LETTER R WITH ACUTE
    case 0x015B:
        return 0xC273; // LATIN SMALL LETTER S WITH ACUTE
    case 0x00FA:
        return 0xC275; // LATIN SMALL LETTER U WITH ACUTE
    case 0x00FD:
        return 0xC279; // LATIN SMALL LETTER Y WITH ACUTE
    case 0x017A:
        return 0xC27A; // LATIN SMALL LETTER Z WITH ACUTE

        // 0xC3 non-spacing circumflex accent
    case 0x00C2:
        return 0xC341; // LATIN CAPITAL LETTER A WITH CIRCUMFLEX
    case 0x0108:
        return 0xC343; // LATIN CAPITAL LETTER C WITH CIRCUMFLEX
    case 0x00CA:
        return 0xC345; // LATIN CAPITAL LETTER E WITH CIRCUMFLEX
    case 0x011C:
        return 0xC347; // LATIN CAPITAL LETTER G WITH CIRCUMFLEX
    case 0x0124:
        return 0xC348; // LATIN CAPITAL LETTER H WITH CIRCUMFLEX
    case 0x00CE:
        return 0xC349; // LATIN CAPITAL LETTER I WITH CIRCUMFLEX
    case 0x0134:
        return 0xC34A; // LATIN CAPITAL LETTER J WITH CIRCUMFLEX
    case 0x00D4:
        return 0xC34F; // LATIN CAPITAL LETTER O WITH CIRCUMFLEX
    case 0x015C:
        return 0xC353; // LATIN CAPITAL LETTER S WITH CIRCUMFLEX
    case 0x00DB:
        return 0xC355; // LATIN CAPITAL LETTER U WITH CIRCUMFLEX
    case 0x0174:
        return 0xC357; // LATIN CAPITAL LETTER W WITH CIRCUMFLEX
    case 0x0176:
        return 0xC359; // LATIN CAPITAL LETTER Y WITH CIRCUMFLEX
    case 0x00E2:
        return 0xC361; // LATIN SMALL LETTER A WITH CIRCUMFLEX
    case 0x0109:
        return 0xC363; // LATIN SMALL LETTER C WITH CIRCUMFLEX
    case 0x00EA:
        return 0xC365; // LATIN SMALL LETTER E WITH CIRCUMFLEX
    case 0x011D:
        return 0xC367; // LATIN SMALL LETTER G WITH CIRCUMFLEX
    case 0x0125:
        return 0xC368; // LATIN SMALL LETTER H WITH CIRCUMFLEX
    case 0x00EE:
        return 0xC369; // LATIN SMALL LETTER I WITH CIRCUMFLEX
    case 0x0135:
        return 0xC36A; // LATIN SMALL LETTER J WITH CIRCUMFLEX
    case 0x00F4:
        return 0xC36F; // LATIN SMALL LETTER O WITH CIRCUMFLEX
    case 0x015D:
        return 0xC373; // LATIN SMALL LETTER S WITH CIRCUMFLEX
    case 0x00FB:
        return 0xC375; // LATIN SMALL LETTER U WITH CIRCUMFLEX
    case 0x0175:
        return 0xC377; // LATIN SMALL LETTER W WITH CIRCUMFLEX
    case 0x0177:
        return 0xC379; // LATIN SMALL LETTER Y WITH CIRCUMFLEX

        // 0xC4 non-spacing tilde
    case 0x00C3:
        return 0xC441; // LATIN CAPITAL LETTER A WITH TILDE
    case 0x0128:
        return 0xC449; // LATIN CAPITAL LETTER I WITH TILDE
    case 0x00D1:
        return 0xC44E; // LATIN CAPITAL LETTER N WITH TILDE
    case 0x00D5:
        return 0xC44F; // LATIN CAPITAL LETTER O WITH TILDE
    case 0x0168:
        return 0xC455; // LATIN CAPITAL LETTER U WITH TILDE
    case 0x00E3:
        return 0xC461; // LATIN SMALL LETTER A WITH TILDE
    case 0x0129:
        return 0xC469; // LATIN SMALL LETTER I WITH TILDE
    case 0x00F1:
        return 0xC46E; // LATIN SMALL LETTER N WITH TILDE
    case 0x00F5:
        return 0xC46F; // LATIN SMALL LETTER O WITH TILDE
    case 0x0169:
        return 0xC475; // LATIN SMALL LETTER U WITH TILDE

        // 0xC5 non-spacing macron
    case 0x0100:
        return 0xC541; // LATIN CAPITAL LETTER A WITH MACRON
    case 0x0112:
        return 0xC545; // LATIN CAPITAL LETTER E WITH MACRON
    case 0x012A:
        return 0xC549; // LATIN CAPITAL LETTER I WITH MACRON
    case 0x014C:
        return 0xC54F; // LATIN CAPITAL LETTER O WITH MACRON
    case 0x016A:
        return 0xC555; // LATIN CAPITAL LETTER U WITH MACRON
    case 0x0101:
        return 0xC561; // LATIN SMALL LETTER A WITH MACRON
    case 0x0113:
        return 0xC565; // LATIN SMALL LETTER E WITH MACRON
    case 0x012B:
        return 0xC569; // LATIN SMALL LETTER I WITH MACRON
    case 0x014D:
        return 0xC56F; // LATIN SMALL LETTER O WITH MACRON
    case 0x016B:
        return 0xC575; // LATIN SMALL LETTER U WITH MACRON

        // 0xC6 non-spacing breve
    case 0x02D8:
        return 0xC620; // BREVE
    case 0x0102:
        return 0xC641; // LATIN CAPITAL LETTER A WITH BREVE
    case 0x011E:
        return 0xC647; // LATIN CAPITAL LETTER G WITH BREVE
    case 0x016C:
        return 0xC655; // LATIN CAPITAL LETTER U WITH BREVE
    case 0x0103:
        return 0xC661; // LATIN SMALL LETTER A WITH BREVE
    case 0x011F:
        return 0xC667; // LATIN SMALL LETTER G WITH BREVE
    case 0x016D:
        return 0xC675; // LATIN SMALL LETTER U WITH BREVE

        // 0xC7 non-spacing dot above
    case 0x010A:
        return 0xC743; // LATIN CAPITAL LETTER C WITH DOT ABOVE
    case 0x0116:
        return 0xC745; // LATIN CAPITAL LETTER E WITH DOT ABOVE
    case 0x0120:
        return 0xC747; // LATIN CAPITAL LETTER G WITH DOT ABOVE
    case 0x0130:
        return 0xC749; // LATIN CAPITAL LETTER I WITH DOT ABOVE
    case 0x017B:
        return 0xC75A; // LATIN CAPITAL LETTER Z WITH DOT ABOVE
    case 0x010B:
        return 0xC763; // LATIN SMALL LETTER C WITH DOT ABOVE
    case 0x0117:
        return 0xC765; // LATIN SMALL LETTER E WITH DOT ABOVE
    case 0x0121:
        return 0xC767; // LATIN SMALL LETTER G WITH DOT ABOVE
    case 0x017C:
        return 0xC77A; // LATIN SMALL LETTER Z WITH DOT ABOVE

        // 0xC8 non-spacing diaeresis
    case 0x00A8:
        return 0xC820; // DIAERESIS
    case 0x00C4:
        return 0xC841; // LATIN CAPITAL LETTER A WITH DIAERESIS
    case 0x00CB:
        return 0xC845; // LATIN CAPITAL LETTER E WITH DIAERESIS
    case 0x00CF:
        return 0xC849; // LATIN CAPITAL LETTER I WITH DIAERESIS
    case 0x00D6:
        return 0xC84F; // LATIN CAPITAL LETTER O WITH DIAERESIS
    case 0x00DC:
        return 0xC855; // LATIN CAPITAL LETTER U WITH DIAERESIS
    case 0x0178:
        return 0xC859; // LATIN CAPITAL LETTER Y WITH DIAERESIS
    case 0x00E4:
        return 0xC861; // LATIN SMALL LETTER A WITH DIAERESIS
    case 0x00EB:
        return 0xC865; // LATIN SMALL LETTER E WITH DIAERESIS
    case 0x00EF:
        return 0xC869; // LATIN SMALL LETTER I WITH DIAERESIS
    case 0x00F6:
        return 0xC86F; // LATIN SMALL LETTER O WITH DIAERESIS
    case 0x00FC:
        return 0xC875; // LATIN SMALL LETTER U WITH DIAERESIS
    case 0x00FF:
        return 0xC879; // LATIN SMALL LETTER Y WITH DIAERESIS

        // 0xC9 (This position shall not be used)

        // 0xCA non-spacing ring above
    case 0x02DA:
        return 0xCA20; // RING ABOVE
    case 0x00C5:
        return 0xCA41; // LATIN CAPITAL LETTER A WITH RING ABOVE
    case 0x016E:
        return 0xCAAD; // LATIN CAPITAL LETTER U WITH RING ABOVE
    case 0x00E5:
        return 0xCA61; // LATIN SMALL LETTER A WITH RING ABOVE
    case 0x016F:
        return 0xCA75; // LATIN SMALL LETTER U WITH RING ABOVE

        // 0xCB non-spacing cedilla
    case 0x00B8:
        return 0xCB20; // CEDILLA
    case 0x00C7:
        return 0xCB43; // LATIN CAPITAL LETTER C WITH CEDILLA
    case 0x0122:
        return 0xCB47; // LATIN CAPITAL LETTER G WITH CEDILLA
    case 0x0136:
        return 0xCB4B; // LATIN CAPITAL LETTER K WITH CEDILLA
    case 0x013B:
        return 0xCB4C; // LATIN CAPITAL LETTER L WITH CEDILLA
    case 0x0145:
        return 0xCB4E; // LATIN CAPITAL LETTER N WITH CEDILLA
    case 0x0156:
        return 0xCB52; // LATIN CAPITAL LETTER R WITH CEDILLA
    case 0x015E:
        return 0xCB53; // LATIN CAPITAL LETTER S WITH CEDILLA
    case 0x0162:
        return 0xCB54; // LATIN CAPITAL LETTER T WITH CEDILLA
    case 0x00E7:
        return 0xCB63; // LATIN SMALL LETTER C WITH CEDILLA
    case 0x0123:
        return 0xCB67; // small g with cedilla
    case 0x0137:
        return 0xCB6B; // LATIN SMALL LETTER K WITH CEDILLA
    case 0x013C:
        return 0xCB6C; // LATIN SMALL LETTER L WITH CEDILLA
    case 0x0146:
        return 0xCB6E; // LATIN SMALL LETTER N WITH CEDILLA
    case 0x0157:
        return 0xCB72; // LATIN SMALL LETTER R WITH CEDILLA
    case 0x015F:
        return 0xCB73; // LATIN SMALL LETTER S WITH CEDILLA
    case 0x0163:
        return 0xCB74; // LATIN SMALL LETTER T WITH CEDILLA

        // 0xCC to do


        // 0xCD non-spacing double acute accent
    case 0x0150:
        return 0xCD4F; // LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
    case 0x0170:
        return 0xCD55; // LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
    case 0x0151:
        return 0xCD6F; // LATIN SMALL LETTER O WITH DOUBLE ACUTE
    case 0x0171:
        return 0xCD75; // LATIN SMALL LETTER U WITH DOUBLE ACUTE

        // 0xCE non-spacing ogonek
    case 0x02DB:
        return 0xCE20; // ogonek
    case 0x0104:
        return 0xCE41; // LATIN CAPITAL LETTER A WITH OGONEK
    case 0x0118:
        return 0xCE45; // LATIN CAPITAL LETTER E WITH OGONEK
    case 0x012E:
        return 0xCE49; // LATIN CAPITAL LETTER I WITH OGONEK
    case 0x0172:
        return 0xCE55; // LATIN CAPITAL LETTER U WITH OGONEK
    case 0x0105:
        return 0xCE61; // LATIN SMALL LETTER A WITH OGONEK
    case 0x0119:
        return 0xCE65; // LATIN SMALL LETTER E WITH OGONEK
    case 0x012F:
        return 0xCE69; // LATIN SMALL LETTER I WITH OGONEK
    case 0x0173:
        return 0xCE75; // LATIN SMALL LETTER U WITH OGONEK

        // 0xCF non-spacing caron
    case 0x02C7:
        return 0xCF20; // CARON
    case 0x010C:
        return 0xCF43; // LATIN CAPITAL LETTER C WITH CARON
    case 0x010E:
        return 0xCF44; // LATIN CAPITAL LETTER D WITH CARON
    case 0x011A:
        return 0xCF45; // LATIN CAPITAL LETTER E WITH CARON
    case 0x013D:
        return 0xCF4C; // LATIN CAPITAL LETTER L WITH CARON
    case 0x0147:
        return 0xCF4E; // LATIN CAPITAL LETTER N WITH CARON
    case 0x0158:
        return 0xCF52; // LATIN CAPITAL LETTER R WITH CARON
    case 0x0160:
        return 0xCF53; // LATIN CAPITAL LETTER S WITH CARON
    case 0x0164:
        return 0xCF54; // LATIN CAPITAL LETTER T WITH CARON
    case 0x017D:
        return 0xCF5A; // LATIN CAPITAL LETTER Z WITH CARON
    case 0x010D:
        return 0xCF63; // LATIN SMALL LETTER C WITH CARON
    case 0x010F:
        return 0xCF64; // LATIN SMALL LETTER D WITH CARON
    case 0x011B:
        return 0xCF65; // LATIN SMALL LETTER E WITH CARON
    case 0x013E:
        return 0xCF6C; // LATIN SMALL LETTER L WITH CARON
    case 0x0148:
        return 0xCF6E; // LATIN SMALL LETTER N WITH CARON
    case 0x0159:
        return 0xCF72; // LATIN SMALL LETTER R WITH CARON
    case 0x0161:
        return 0xCF73; // LATIN SMALL LETTER S WITH CARON
    case 0x0165:
        return 0xCF74; // LATIN SMALL LETTER T WITH CARON
    case 0x017E:
        return 0xCF7A; // LATIN SMALL LETTER Z WITH CARON

    default:
        return 0;
    }
}
