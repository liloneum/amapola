#include "mytextedit.h"
#include "ui_mytextedit.h"
#include "myattributesconverter.h"
#include <QKeyEvent>
#include <QTextDocumentFragment>
#include <QDesktopWidget>

// Maximum number of character by line
#define MAX_CHAR_BY_LINE 40

// Fonts are rendered as if the screen height is 11 inches,
// so a 72pt font would be 1/11 screen height.
#define REF_HEIGHT_INCH 11
#define PT_PER_INCH 72

// Spacing between 2 lines
#define LINE_SAPACING 2

// Temp : Default font
#define FONT_ID_DEFAULT_VALUE "Arial"
#define FONT_COLOR_DEFAULT_VALUE "FF0000FF"
#define FONT_EFFECT_DEFAULT_VALUE "none"
#define FONT_EFFECT_COLOR_DEFAULT_VALUE "FF000000"
#define FONT_ITALIC_DEFAULT_VALUE "no"
#define FONT_SCRIPT_DEFAULT_VALUE "normal"
#define FONT_SIZE_DEFAULT_VALUE "42"
#define FONT_UNDERLINED_DEFAULT_VALUE "no"

// Temp : Default position
#define TEXT1_DIRECTION_DEFAULT_VALUE "horizontal"
#define TEXT1_HALIGN_DEFAULT_VALUE "center"
#define TEXT1_HPOSITION_DEFAULT_VALUE "0"
#define TEXT1_VALIGN_DEFAULT_VALUE "bottom"
#define TEXT1_VPOSITION_DEFAULT_VALUE "8"


// This widget class manage the edit zone.
// Manage 2 lines of editable text, with position and font attributes
MyTextEdit::MyTextEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyTextEdit)
{
    ui->setupUi(this);

    // Retrieive the number of pixels per inch for the current hardware
    QDesktopWidget *mydesk = QApplication::desktop();
    mPxPerInch = mydesk->logicalDpiY();

    // Set text zones colors
    ui->textLine1->setStyleSheet("background: transparent; border: 1px solid red;");
    ui->textLine2->setStyleSheet("background: transparent; border: 1px solid blue;");

    // Temp : Init a default subtitle container with the default font and position
    //this->defaultSub();

    // Add an event filter on the subtitles textedit object
    ui->textLine1->installEventFilter(this);
    ui->textLine2->installEventFilter(this);

    // Disable and hide the 2nd text lines
    ui->textLine2->setEnabled(false);
    ui->textLine2->hide();

    // Test flag : text is updating from the database
    mIsSettingLines = false;

    // Init copies of the last texts wrote from the database
    mPreviousText1 = "";
    mPreviousText2 = "";

    // Init the focus on textline1
    mpLastFocused = ui->textLine1;

    // Init connections
    connect(ui->textLine1, SIGNAL(cursorPositionChanged()), this, SLOT(newCursorPosition()));
    connect(ui->textLine2, SIGNAL(cursorPositionChanged()), this, SLOT(newCursorPosition()));
}


MyTextEdit::~MyTextEdit()
{
    delete ui;
}

// Save the default position and font parameter
void MyTextEdit::defaultSub() {

    TextLine default_line;
    TextFont default_font;

    mDefaultSub.clear();
    mCurrentTextProp.clear();

    default_font.setFontId( qApp->property("prop_FontName").toString() );
    default_font.setFontColor( qApp->property("prop_FontColor_rgba").toString() );
    default_font.setFontEffect( FONT_EFFECT_DEFAULT_VALUE );
    default_font.setFontEffectColor( FONT_EFFECT_COLOR_DEFAULT_VALUE );
    default_font.setFontItalic( qApp->property("prop_FontItalic").toString() );
    default_font.setFontScript( FONT_SCRIPT_DEFAULT_VALUE );
    default_font.setFontSize( qApp->property("prop_FontSize_pt").toString() );
    default_font.setFontUnderlined( qApp->property("prop_FontUnderlined").toString() );

    default_line.setTextDirection( TEXT1_DIRECTION_DEFAULT_VALUE );
    default_line.setTextHAlign( qApp->property("prop_Halign").toString() );
    default_line.setTextVAlign( qApp->property("prop_Valign").toString() );
    default_line.setTextHPosition( qApp->property("prop_Hposition_percent").toString() );
    default_line.setTextVPosition( qApp->property("prop_Vposition_percent").toString() );

    mDefaultSub.setText(default_line, default_font);
    mCurrentTextProp.setText(default_line, default_font);
}

MySubtitles MyTextEdit::getDefaultSub() {

    return mDefaultSub;
}

void MyTextEdit::saveCurrentTextPos(TextLine textLine, QTextEdit* textEdit) {

    qint16 line_nbr = 0;

    if ( textEdit == ui->textLine1 ) {
        line_nbr = 1;
    }
    else if( textEdit == ui->textLine2 ) {
        line_nbr = 2;
    }

    // Clear the line2 if it doesn't exist anymore
    if ( ( ui->textLine2->isEnabled() == false ) && ( mCurrentTextProp.text().count() >=2 ) ) {
        mCurrentTextProp.text().removeLast();
    }

    if ( line_nbr <= mCurrentTextProp.text().count() ) {

        // Retrieve the current text properties lines list
        QList<TextLine> current_text_lines = mCurrentTextProp.text();
        // Retrieve the line to change
        TextLine current_line = current_text_lines.at(line_nbr - 1);
        // Retrieve its font
        TextFont current_text_font = current_line.Font();

        // Push the font properties in the new line
        textLine.setFont(current_text_font);
        // Replace the line in the list
        current_text_lines.replace(line_nbr - 1, textLine);
        // Push the changed list in the subtitle container
        mCurrentTextProp.setText(current_text_lines);
    }
    else {
        mCurrentTextProp.setText(textLine, mDefaultSub.text().first().Font());
    }
}

void MyTextEdit::saveCurrentTextFont(TextFont textFont, QTextEdit* textEdit) {

    qint16 line_nbr = 0;

    if ( textEdit == ui->textLine1 ) {
        line_nbr = 1;
    }
    else if( textEdit == ui->textLine2 ) {
        line_nbr = 2;
    }

    // Clear the line2 if it doesn't exist anymore
    if ( ( ui->textLine2->isEnabled() == false ) && ( mCurrentTextProp.text().count() >=2 ) ) {
        mCurrentTextProp.text().removeLast();
    }

    if ( line_nbr <= mCurrentTextProp.text().count() ) {

        // Retrieve the current text properties lines list
        QList<TextLine> current_text_lines = mCurrentTextProp.text();
        // Retrieve the line to change
        TextLine current_line = current_text_lines.at(line_nbr - 1);

        // Push the text font properties in the current line
        current_line.setFont(textFont);
        // Replace the line in the list
        current_text_lines.replace(line_nbr - 1, current_line);
        // Push the changed list in the subtitle container
        mCurrentTextProp.setText(current_text_lines);
    }
    else {
        mCurrentTextProp.setText(mDefaultSub.text().first(), textFont);
    }
}

// Get the text from the text edit zones
QList<TextLine> MyTextEdit::text() {

    QList<TextLine> text_lines;

   if ( ui->textLine2->isEnabled() ) {

        QString text1 = ui->textLine1->toPlainText();
        TextLine line1;
        line1.setLine(text1);
        text_lines.append(line1);

        QString text2 = ui->textLine2->toPlainText();
        TextLine line2;
        line2.setLine(text2);
        text_lines.append(line2);
    }
    else {

        QString text1 = ui->textLine1->toPlainText();
        TextLine line1;
        line1.setLine(text1);
        text_lines.append(line1);
    }
    return text_lines;
}

// Get the text properties (font and position)
// Return a subtitle container
MySubtitles MyTextEdit::subtitleData() {

    return mCurrentTextProp;
}


bool MyTextEdit::eventFilter(QObject* watched, QEvent* event) {

    // Event in line 1 zone
    if ( watched == ui->textLine1 ) {
        if ( event->type() == QEvent::KeyPress ) {

            QKeyEvent* key_event = static_cast<QKeyEvent*>(event);

            // Key event is insert new line
            if ( ( key_event->matches(QKeySequence::InsertParagraphSeparator) ) &&
                 ( ui->textLine1->document()->blockCount() == 1 ) ) {

                // If line 2 doesn't exist, create it with the same properties than line 1
                // Font and position, except for horizontal position
                if ( !ui->textLine2->isEnabled() ) {

                    TextLine text_line;
                    TextFont text_font;

                    text_line = mCurrentTextProp.text().first();
                    text_font = text_line.Font();

                    ui->textLine2->setEnabled(true);
                    ui->textLine2->show();

                    // Move line 2 to line 1 position
                    this->setTextFont(ui->textLine2, text_font, this->size());
                    this->setTextPosition(ui->textLine2, text_line, this->size());

                    // Move line 1 on top of line with spacing defined by LINE_SPACING
                    ui->textLine1->move( ui->textLine1->x(), ( ui->textLine1->y() - ui->textLine1->height() - LINE_SAPACING ) );

                    // Save line 1 new position
                    this->textPosition(ui->textLine1, text_line, this->size());
                    this->saveCurrentTextPos(text_line, ui->textLine1);

                    ui->textLine2->setFocus(Qt::OtherFocusReason);

                    emit cursorPositionChanged();
                    emit subDatasChanged( subtitleData() );
                }
                return true;
            } // Key event is move to next line (down arrow)
            else if ( ( key_event->matches(QKeySequence::MoveToNextLine) ) &&
                      ( ui->textLine2->isEnabled() ) ) {

                // Move cursor to line 2 and give the focus
                ui->textLine2->setFocus(Qt::OtherFocusReason);
                ui->textLine2->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);

                return true;
            }
        } // Line 1 get the focus
        else if ( event->type() == QEvent::FocusIn) {

            // Send the line 1 properties (font, position)
            mpLastFocused = ui->textLine1;
            emit textLineFocusChanged();
        }
    } // Event in line 2 zone
    else if ( watched == ui->textLine2) {
        if ( event->type() == QEvent::KeyPress ) {

            QKeyEvent* key_event = static_cast<QKeyEvent*>(event);

            // If key event is insert new line. Do nothing.
            if ( ( key_event->matches(QKeySequence::InsertParagraphSeparator) ) &&
                 ( ui->textLine2->document()->blockCount() == 1 ) ) {
                return true;
            } // If key event is backspace & the text is empty
            else if ( ( key_event->key() == (Qt::Key_Backspace) ) &&
                      ( ui->textLine2->toPlainText().isEmpty() ) ) {

                // Delete line 2 and move line 1 to line 2 position
                ui->textLine2->setEnabled(false);
                ui->textLine2->hide();

                TextLine text_line = mCurrentTextProp.text().last();
                this->setTextPosition(ui->textLine1, text_line, this->size());

                emit cursorPositionChanged();
                emit subDatasChanged( subtitleData() );

                ui->textLine1->setFocus(Qt::OtherFocusReason);
                ui->textLine1->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);

                return true;
            }// Key event is move to previous line (up arrow)
            else if ( key_event->matches(QKeySequence::MoveToPreviousLine) ) {

                // Move cursor to line 1 and give focus
                ui->textLine1->setFocus(Qt::OtherFocusReason);
                ui->textLine1->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
                return true;
            }
        } // Line 2 get the focus
        else if ( event->type() == QEvent::FocusIn) {

            // Send the line 2 properties (font, position)
            mpLastFocused = ui->textLine2;
            emit textLineFocusChanged();
        }
    }

    return QWidget::eventFilter(watched, event);
}

// Display text in line 1 and line 2
void MyTextEdit::setText(MySubtitles subtitle) {

    // Indicate that text is updating
    mIsSettingLines = true;

    ui->textLine1->clearFocus();
    ui->textLine2->clearFocus();

    // If there are text to display
    if ( subtitle.isValid() ) {

        QList<TextLine> TextLines = subtitle.text();

        mIsSettingLines = true;

        // There are two lines
        if ( TextLines.count() == 2 ) {

            ui->textLine2->setEnabled(true);
            ui->textLine2->show();

            this->setTextFont(ui->textLine1, TextLines.first().Font(), this->size());
            this->setTextFont(ui->textLine2, TextLines.last().Font(), this->size());

            ui->textLine1->setText( TextLines.first().Line() );
            ui->textLine2->setText( TextLines.last().Line() );

            this->setTextPosition(ui->textLine1, TextLines.first(), this->size());
            this->setTextPosition(ui->textLine2, TextLines.last(), this->size());

            mCurrentTextProp.text().first().setLine(TextLines.first().Line());
            mCurrentTextProp.text().last().setLine(TextLines.last().Line());

            ui->textLine2->setFocus(Qt::OtherFocusReason);
            ui->textLine2->moveCursor(QTextCursor::End);

        } // There are one line
        else if ( TextLines.count() == 1 ) {

            ui->textLine2->setEnabled(false);
            ui->textLine2->hide();

            this->setTextFont(ui->textLine1, TextLines.first().Font(), this->size());

            ui->textLine1->setText( TextLines.first().Line() );
            ui->textLine2->setText("");

            this->setTextPosition(ui->textLine1, TextLines.first(), this->size());

            mCurrentTextProp.text().first().setLine(TextLines.first().Line());

            ui->textLine1->setFocus(Qt::OtherFocusReason);
            ui->textLine1->moveCursor(QTextCursor::End);
        }
    } // No text to display. Hide the line 2 and display empty text
    else {

        ui->textLine1->setText("");
        ui->textLine2->setText("");

        ui->textLine2->setEnabled(false);
        ui->textLine2->hide();

        // Set default font and position to line 1
        this->setTextFont(ui->textLine1, mDefaultSub.text().first().Font(), this->size());

        this->setTextPosition(ui->textLine1, mDefaultSub.text().first(), this->size());

        mCurrentTextProp.text().first().setLine("");

        ui->textLine1->setFocus(Qt::OtherFocusReason);
        ui->textLine1->moveCursor(QTextCursor::End);
    }

    // Keep a backup of the last texts displayed
    mPreviousText1 = ui->textLine1->toPlainText();
    mPreviousText2 = ui->textLine2->toPlainText();

    mIsSettingLines = false;
}

// Update current or last focussed line postion
void MyTextEdit::updateTextPosition(TextLine textLine) {

    // Change the position
    this->setTextPosition(mpLastFocused, textLine, this->size());
}

// Set the position of the given "textEdit" zone
// Position is defined by :
// - Vertical alignment (top, center, bottom)
// - Horizontal alignment (left, center, right)
// - Vertical coordonate (ref : top of the widget)
// - Horizontal coordonate (ref : left of the widget)
void MyTextEdit::setTextPosition(QTextEdit* textEdit, TextLine textLine, QSize widgetSize ) {

    bool ok;
    qint32 pos_x_offset;
    qint32 pos_y_offset;
    qint32 pos_x;
    qint32 pos_y;
    Qt::Alignment h_align;
    Qt::Alignment v_align; 
    qint32 widget_width;
    qint32 widget_height;

    // Retrieive the widget size
    widget_width = widgetSize.width();
    widget_height = widgetSize.height();

    QFontMetrics font_metrics(textEdit->font());

    // Set the horizontal alignment
    h_align = MyAttributesConverter::hAlignFromString( textLine.textHAlign() );
    textEdit->setAlignment(h_align);
    // Convert horizontal position from % to number of pixels
    pos_x_offset = qRound( ( (qreal)widget_width * (qreal)textLine.textHPosition().toFloat(&ok) ) / (qreal)100 );

    // The position is given relative to horizontal alignment (left, center, right)
    // Calculate the position relative to the left of the widget
    if ( h_align == Qt::AlignCenter ) {

        pos_x = ( ( ( widget_width - textEdit->width() ) / 2 ) + pos_x_offset );
    }
    else if ( h_align == Qt::AlignLeft ) {

        pos_x = pos_x_offset;
    }
    else if ( h_align == Qt::AlignRight ) {

        pos_x = widget_width - textEdit->width() - pos_x_offset;
    }

    // Retrieive the vertical alignment
    v_align = MyAttributesConverter::vAlignFromString( textLine.textVAlign() );
    // Convert vertical position from % to number of pixels
    pos_y_offset = qRound( ( (qreal)widget_height * (qreal)textLine.textVPosition().toFloat(&ok) ) / (qreal)100 );

    // The position is given relative to vertical alignment (top, center, bottom)
    // Calculate the position relative to the top of the widget
    if ( v_align == Qt::AlignCenter ) {

        QFontMetrics font_metrics(textEdit->font());

        pos_y = ( ( widget_height / 2 ) + pos_y_offset - textEdit->height() + font_metrics.descent() /*+ ( font_metrics.leading() / 2 )*/ );
    }
    else if ( v_align == Qt::AlignBottom) {

        pos_y = ( widget_height - pos_y_offset - textEdit->height() + font_metrics.descent() /*+ ( font_metrics.leading() / 2 )*/ );
    }
    else if ( v_align == Qt::AlignTop ) {

        pos_y = ( pos_y_offset - textEdit->height() + font_metrics.descent() + ( font_metrics.leading() / 2 ) );
    }

    // Move the line to the new position
    textEdit->move(pos_x, pos_y);

    // Save this position parameter to current
    this->saveCurrentTextPos(textLine, textEdit);
}

// Retrieive the "textEdit" position properties,
// not relative to the current video displayed but in %.
// Returned properties are in "String" format to be easily displayed and exported in file
void MyTextEdit::textPosition(QTextEdit* textEdit, TextLine &textLine, QSize widgetSize) {

    Qt::Alignment h_align;
    qint32 h_position;
    qint32 v_position;
    qint32 widget_width;
    qint32 widget_height;

    widget_width = widgetSize.width();
    widget_height = widgetSize.height();

    QFontMetrics font_metrics(textEdit->font());

    // Horizontal position
    h_align = textEdit->alignment();

    if ( h_align == Qt::AlignCenter ) {

        textLine.setTextHAlign("center");
        h_position = ( textEdit->x() - ( widget_width - textEdit->width() ) / 2 );
    }
    else if ( h_align == Qt::AlignLeft ) {

        textLine.setTextHAlign("left");
        h_position = textEdit->x();
    }
    else if ( h_align == Qt::AlignRight ) {

        textLine.setTextHAlign("right");
        h_position = widget_width - textEdit->width() - textEdit->x();
    }

    // Vertical position : alignment is always "bottom"
    textLine.setTextVAlign("bottom");

    v_position = ( widget_height - textEdit->y() - textEdit->height() + font_metrics.descent() /*+ ( font_metrics.leading() / 2 )*/ );

    textLine.setTextHPosition(  QString::number(( ( (qreal)h_position * (qreal)100 ) / (qreal)widget_width ), 'f', 1  ) );
    textLine.setTextVPosition(  QString::number(( ( (qreal)v_position * (qreal)100 ) / (qreal)widget_height ), 'f', 1  ) );

    textLine.setTextDirection("horizontal");
}

// Update current or last focussed line font
void MyTextEdit::updateTextFont(TextFont textFont, TextLine textLine) {

    // Retrieive the line position before. Because if font size change,
    // the position need to be recomputed
    this->setTextFont(mpLastFocused, textFont, this->size());
    this->setTextPosition(mpLastFocused, textLine, this->size());
}

// Set the font of the given textEdit zone
// Font is defined by :
// - Font name
// - Size
// - Color
// - Italic (yes/no)
// - Underlined (yes/no)
void MyTextEdit::setTextFont(QTextEdit *textEdit, TextFont textFont, QSize widgetSize) {

    qint16 font_size_pt;
    qreal relative_font_size;
    QColor font_color;
    bool ok;

//    QString text = textEdit->toPlainText();

    QFont font = textEdit->font();

    // Set font name
    font.setFamily( textFont.fontId() );

    // Set font size
    font_size_pt = textFont.fontSize().toInt();

    // The font size is given in "point", the number of point per inch depending of the screen.
    // The font size is given as if the screen height is 11 inches,
    // so a 72pt font would be 1/11 screen height.
    // Recompute the font size in point, relative to the real displayed video size in pixel
    qint32 widget_height_px = widgetSize.height();
    relative_font_size = ( font_size_pt * ( ( (qreal)widget_height_px / ( (qreal)mPxPerInch  * (qreal)REF_HEIGHT_INCH ) ) ) );
    font.setPointSizeF(relative_font_size);

    // Set Italic
    font.setItalic( textFont.fontItalic().contains("yes", Qt::CaseInsensitive) );

    // Set Underlined
    font.setUnderline( textFont.fontUnderlined().contains("yes", Qt::CaseInsensitive) );

    //textEdit->setCurrentFont(font);
    textEdit->setFont(font);

    // Set font color
    font_color = QColor::fromRgba( textFont.fontColor().toUInt(&ok, 16) );
    QPalette widget_palette = textEdit->palette();
    widget_palette.setColor(QPalette::Text,font_color);
    textEdit->setPalette(widget_palette);


    // Resize the height of subtitles edit zone in function of font size
    //QString test_string_width;

    //test_string_width.fill('W', MAX_CHAR_BY_LINE);
    QFontMetrics font_metrics( textEdit->font() );

    textEdit->setFixedHeight( font_metrics.height() + ( textEdit->frameWidth() * 2 ) );
    //textEdit->setFixedWidth( font_metrics.width(test_string_width) + ( textEdit->frameWidth() * 2 ) );
    textEdit->setFixedWidth(widgetSize.width() - 20);

    // Save this font parameter to current
    this->saveCurrentTextFont(textFont, textEdit);

//    textEdit->setText(text);
}

// Retrieive the given "textEdit" font properties
// Font sized returned in "point" for 72pt = 1 inch
// Returned properties are in "String" format to be easily displayed and exported in file
void MyTextEdit::textFont(QTextEdit *textEdit, TextFont &textFont, QSize widgetSize) {

    qint16 font_size_pt;
    qreal relative_font_size_pt;
    QColor font_color;

//    QFont font = textEdit->currentFont();
    QFont font = textEdit->font();
    // Get font name
    textFont.setFontId( font.family() );

    // Get font size
    relative_font_size_pt = font.pointSizeF();
    qint32 widget_height_px = widgetSize.height();
    font_size_pt = qRound( relative_font_size_pt * ( ( (qreal)mPxPerInch  * (qreal)REF_HEIGHT_INCH ) / (qreal)widget_height_px ) );
    textFont.setFontSize( QString::number(font_size_pt) );

    // Get Italic
    textFont.setFontItalic( MyAttributesConverter::isItalic( font.italic() ) );
    // Get Underlined
    textFont.setFontUnderlined( MyAttributesConverter::isUnderlined( font.underline() ) );

    // Get font color
    //font_color = textEdit->textColor();
    QPalette widget_palette = textEdit->palette();
    font_color = widget_palette.color(QPalette::Text);
    textFont.setFontColor( (QString::number( font_color.rgba(), 16 )).toUpper() );
}

// Widget resize event
void MyTextEdit::resizeEvent(QResizeEvent *event) {

    TextLine text_line;
    TextFont text_font;

    // Retrieive the properties non relative to widget size
    text_line = mCurrentTextProp.text().first();
    text_font = text_line.Font();
    // Recompute properties relative to new widget size
    this->setTextFont(ui->textLine1, text_font, this->size());
    this->setTextPosition(ui->textLine1, text_line, this->size());

    if ( ui->textLine2->isEnabled() ) {

        text_line = mCurrentTextProp.text().last();
        text_font = text_line.Font();
        this->setTextFont(ui->textLine2, text_font, this->size());
        this->setTextPosition(ui->textLine2, text_line, this->size());
    }

    QWidget::resizeEvent(event);
}


void MyTextEdit::newCursorPosition() {

    // If a new text line is setting, cursor has not moved by user but automatically.
    // So do nothing
    if ( mIsSettingLines == false ) {

        // If user has changed the text, send a signal
        if ( ( ui->textLine1->toPlainText() != mPreviousText1 ) || (ui->textLine2->toPlainText() != mPreviousText2) ) {

            mPreviousText1 = ui->textLine1->toPlainText();
            mPreviousText2 = ui->textLine2->toPlainText();
            emit cursorPositionChanged();
        }
    }
}

qint16 MyTextEdit::lastFocused() {

    if ( mpLastFocused == ui->textLine1 ) {
        return 1;
    }
    else if ( mpLastFocused == ui->textLine2 ) {
        return 2;
    }
    else return 0;
}

