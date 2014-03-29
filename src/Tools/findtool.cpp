#include "findtool.h"
#include "ui_findtool.h"
#include "myapplication.h"
#include "myattributesconverter.h"

FindTool::FindTool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FindTool)
{
    ui->setupUi(this);

    mMatchingSubsList.clear();
}

FindTool::~FindTool() {
    delete ui;
}

/**
 * @brief FindTool::on_findLineEdit_textChanged
 * Search the text 'searchedText' inside the subtitle list. Highlight the matching subtitle
 * with yellow color.
 * @param searchedText text to search
 */
void FindTool::on_findLineEdit_textChanged(QString searchedText) {

    MainWindow* main_window = MyApplication::getMainWindow();

    mMatchingSubsList.clear();
    mFindRegExp.setPattern("");
    int sub_count = 0;

    if ( searchedText != "" ) {

        if ( ui->findCaseCheckBox->isChecked() == true ) {
            mFindRegExp.setCaseSensitivity(Qt::CaseSensitive);
        }
        else {
            mFindRegExp.setCaseSensitivity(Qt::CaseInsensitive);
        }

        if ( ui->findWholeWordCheckBox->isChecked() == true ) {
            mFindRegExp.setPattern("\\b" +searchedText +"\\b");
        }
        else {
            mFindRegExp.setPattern(searchedText);
        }

        QList<MySubtitles> sub_list = main_window->getSubtitlesList();
        sub_count = sub_list.count();

        quint16 sub_nbr = 0;

        // Search the reg exp in the subtitle list
        QList<MySubtitles>::iterator sub_it;
        for( sub_it = sub_list.begin(); sub_it != sub_list.end(); ++sub_it ) {

            MySubtitles current_subtitle = *sub_it;

            QList<TextLine> text_lines = current_subtitle.text();

            QList<TextLine>::iterator line_it;
            for( line_it = text_lines.begin(); line_it != text_lines.end(); ++line_it ) {

                TextLine current_line = *line_it;
                QString current_text = MyAttributesConverter::htmlToPlainText(current_line.Line());

                if ( current_text.contains(mFindRegExp) ) {
                    mMatchingSubsList.append(sub_nbr);
                    break;
                }
            }
            sub_nbr++;
        }
    }

    // Highlight the matching subtitle
    sub_count = main_window->getSubtitlesCount();

    for ( int row_nbr = 0; row_nbr < sub_count; row_nbr++ ) {

        if ( mMatchingSubsList.contains(row_nbr) ) {
            main_window->setTableItemColor(row_nbr, MySubtitlesTable::SUB_TEXT_COL, Qt::yellow, QPalette::Background);
        }
        else {
            main_window->setTableItemColor(row_nbr, MySubtitlesTable::SUB_TEXT_COL, Qt::transparent, QPalette::Background);
        }
    }

    // Select the first subtitle matching
    if ( mMatchingSubsList.count() > 0 ) {
        main_window->selectSubtitle(mMatchingSubsList.first());
    }

    ui->findLineEdit->setFocus();
}

/**
 * @brief FindTool::on_findCaseCheckBox_clicked
 * Case sensitivity changed. Run a new search.
 */
void FindTool::on_findCaseCheckBox_clicked() {

    on_findLineEdit_textChanged(ui->findLineEdit->text());
}

/**
 * @brief FindTool::on_findWholeWordCheckBox_clicked
 * Whole word option changed. Run a new search.
 */
void FindTool::on_findWholeWordCheckBox_clicked() {

    on_findLineEdit_textChanged(ui->findLineEdit->text());
}

/**
 * @brief FindTool::on_findPrevArrow_clicked
 * Previous match arrow clicked. Select the previous subtitle matching the search.
 */
void FindTool::on_findPrevArrow_clicked() {

    if ( !mMatchingSubsList.isEmpty() ) {

        MainWindow* main_window = MyApplication::getMainWindow();

        quint16 selected_sub = main_window->getSelectedSub();

        for ( qint16 i = ( mMatchingSubsList.size() - 1 ); i >= 0; i-- ) {

            if ( mMatchingSubsList.at(i) < selected_sub ) {

                main_window->selectSubtitle(mMatchingSubsList.at(i));
                return;
            }
        }

        main_window->selectSubtitle(mMatchingSubsList.last());
    }
}

/**
 * @brief FindTool::on_findNextArrow_clicked
 * Next match arrow clicked. Select the next subtitle matching the search.
 */
void FindTool::on_findNextArrow_clicked() {

    if ( !mMatchingSubsList.isEmpty() ) {

        MainWindow* main_window = MyApplication::getMainWindow();

        quint16 selected_sub = main_window->getSelectedSub();

        for ( qint16 i = 0; i < mMatchingSubsList.size(); i++ ) {

            if ( mMatchingSubsList.at(i) > selected_sub ) {

                main_window->selectSubtitle(mMatchingSubsList.at(i));
                return;
            }
        }

        main_window->selectSubtitle(mMatchingSubsList.first());
    }
}

/**
 * @brief FindTool::on_replacePushButton_clicked
 * Replace the found text inside the selected subtitle (or the next matching subtitle),
 * by the replacement string
 */
void FindTool::on_replacePushButton_clicked() {

    if ( !mMatchingSubsList.isEmpty() ) {

        MainWindow* main_window = MyApplication::getMainWindow();

        QList<MySubtitles> sub_list = main_window->getSubtitlesList();

        quint16 selected_sub = main_window->getSelectedSub();

        // If the current selected subtitle doesn't match, select the first subtitle matching
        if (!mMatchingSubsList.contains(selected_sub)) {
            on_findNextArrow_clicked();
            selected_sub = main_window->getSelectedSub();
        }

        // Replace
        replaceText(sub_list, selected_sub);

        // Load the modified subtitles list
        main_window->setSubtitlesList(sub_list);

        mMatchingSubsList.removeAt(mMatchingSubsList.indexOf(selected_sub));

        // Re-highlight the matching subtitle and select the first subtitle matching
        if (!mMatchingSubsList.isEmpty()) {

            for ( int i = 0; i < mMatchingSubsList.size(); i++ ) {

                main_window->setTableItemColor(mMatchingSubsList.at(i), MySubtitlesTable::SUB_TEXT_COL, Qt::yellow, QPalette::Background);
            }

            on_findNextArrow_clicked();
        }
    }
}

/**
 * @brief FindTool::on_replaceAllPushButton_clicked
 * Replace the found text inside all matching subtitle,
 * by the replacement string
 */
void FindTool::on_replaceAllPushButton_clicked() {

    if ( !mMatchingSubsList.isEmpty() ) {

        MainWindow* main_window = MyApplication::getMainWindow();

        QList<MySubtitles> sub_list = main_window->getSubtitlesList();

        for ( qint16 i = 0; i < mMatchingSubsList.size(); i++ ) {

            replaceText(sub_list, mMatchingSubsList[i]);
        }

        main_window->setSubtitlesList(sub_list);

        mMatchingSubsList.clear();
    }
}

/**
 * @brief FindTool::replaceText
 * Helper : Replace the found text inside the given 'subList' at 'subNumber'
 * @param subList the complete subtitle list
 * @param subNumber subtitle number to treat
 */
void FindTool::replaceText(QList<MySubtitles>& subList, quint16 subNumber) {

    QList<TextLine> text_lines = subList[subNumber].text();

    QList<TextLine>::iterator line_it;
    for( line_it = text_lines.begin(); line_it != text_lines.end(); ++line_it ) {

        TextLine* current_line = &(*line_it);
        QString current_text = current_line->Line();

        current_text.replace(mFindRegExp, ui->replaceLineEdit->text());
        current_line->setLine(current_text);
    }

    subList[subNumber].setText(text_lines);
}
