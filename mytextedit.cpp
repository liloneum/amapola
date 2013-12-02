#include "mytextedit.h"
#include "myattributesconverter.h"
#include <QKeyEvent>
#include <QTextDocumentFragment>
#include <QDesktopWidget>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QTextCharFormat>
#include <QMenu>
#include <QColorDialog>

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
#define FONT_SHADOW_EFFECT_DEFAULT_VALUE "yes"
#define FONT_SHADOW_EFFECT_COLOR_DEFAULT_VALUE "FF000000"
#define FONT_BORDER_EFFECT_DEFAULT_VALUE "yes"
#define FONT_BORDER_EFFECT_COLOR_DEFAULT_VALUE "FF000000"
#define FONT_BACKGROUND_EFFECT_DEFAULT_VALUE "no"
#define FONT_BACKGROUND_EFFECT_COLOR_DEFAULT_VALUE "FF000000"
#define FONT_ITALIC_DEFAULT_VALUE "no"
#define FONT_SCRIPT_DEFAULT_VALUE "normal"
#define FONT_SIZE_DEFAULT_VALUE "42"
#define FONT_UNDERLINED_DEFAULT_VALUE "no"

// Temp : Default position
#define TEXT1_DIRECTION_DEFAULT_VALUE "ltr"
#define TEXT1_HALIGN_DEFAULT_VALUE "center"
#define TEXT1_HPOSITION_DEFAULT_VALUE "0"
#define TEXT1_VALIGN_DEFAULT_VALUE "bottom"
#define TEXT1_VPOSITION_DEFAULT_VALUE "8"


// This widget class manage the edit zone.
// Manage many lines of editable text, with position and font attributes
MyTextEdit::MyTextEdit(QWidget *parent) :
    QWidget(parent)
{
    // Retrieive the number of pixels per inch for the current hardware
    QDesktopWidget *mydesk = QApplication::desktop();
    mPxPerInch = mydesk->logicalDpiY();

    mTextLinesList.clear();

    mTextLinesList.append( this->createNewTextEdit() );

    // Temp : Init a default subtitle container with the default font and position
    //this->defaultSub();

    // Test flag : text is updating from the database
    mIsSettingLines = false;
    // Test flag : : font is updating. Cursor maybe moved by soft
    mIsChangingFont = false;

    // Init copies of the last texts wrote from the database
    mPreviousTextList.append("");

    // Init the focus on textline1
    mpLastFocused = mTextLinesList.first();
}


MyTextEdit::~MyTextEdit()
{
//    delete ui;
}

QTextEdit* MyTextEdit::createNewTextEdit() {

    QTextEdit* text_edit = new QTextEdit(this);

    // Set text zones colors
    text_edit->setStyleSheet("background: transparent; border: 1px solid white;");

    // Install a shadow effect on the text edit widget
    QGraphicsDropShadowEffect* shadow_effect = new QGraphicsDropShadowEffect(this);
    shadow_effect->setBlurRadius(3);
    shadow_effect->setColor(QColor("#" +qApp->property("prop_FontShadowColor").toString()));
    shadow_effect->setOffset(2,2);
    shadow_effect->setEnabled(false);
    text_edit->setGraphicsEffect(shadow_effect);

    text_edit->setAcceptRichText(false);
    // Disable scroll bar
    text_edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    text_edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // Add an event filter on the subtitles textedit object
    text_edit->installEventFilter(this);
    text_edit->setContextMenuPolicy(Qt::CustomContextMenu);
    // Init connections
    connect(text_edit, SIGNAL(cursorPositionChanged()), this, SLOT(newCursorPosition()));
    connect(text_edit, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showCustomContextMenu(QPoint)));
    text_edit->show();

    return text_edit;
}

// Save the default position and font parameter
void MyTextEdit::updateDefaultSub() {

    TextLine default_line;
    TextFont default_font;

    mDefaultSub.clear();
    mCurrentTextProp.clear();

    default_font.setFontId( qApp->property("prop_FontName").toString() );
    default_font.setFontColor( qApp->property("prop_FontColor_rgba").toString() );
    default_font.setFontShadowEffect( qApp->property("prop_FontShadow").toString() );
    default_font.setFontShadowEffectColor( qApp->property("prop_FontShadowColor").toString() );
    default_font.setFontBorderEffect( qApp->property("prop_FontBorder").toString() );
    default_font.setFontBorderEffectColor( qApp->property("prop_FontBorderColor").toString() );
    default_font.setFontBackgroundEffect( qApp->property("prop_FontBackground").toString() );
    default_font.setFontBackgroundEffectColor( qApp->property("prop_FontBackgroundColor").toString() );
    default_font.setFontItalic( qApp->property("prop_FontItalic").toString() );
    default_font.setFontScript( FONT_SCRIPT_DEFAULT_VALUE );
    default_font.setFontSize( qApp->property("prop_FontSize_pt").toString() );
    default_font.setFontUnderlined( qApp->property("prop_FontUnderlined").toString() );

    default_line.setTextDirection( TEXT1_DIRECTION_DEFAULT_VALUE );
    default_line.setTextHAlign( qApp->property("prop_Halign").toString() );
    default_line.setTextVAlign( qApp->property("prop_Valign").toString() );
    default_line.setTextHPosition( qApp->property("prop_Hposition_percent").toString() );
    default_line.setTextVPosition( qApp->property("prop_Vposition_percent").toString() );

    default_line.setLine("");

    mDefaultSub.setText(default_line, default_font);
    mCurrentTextProp = mDefaultSub;
}

MySubtitles MyTextEdit::getDefaultSub() {

    return mDefaultSub;
}

void MyTextEdit::saveCurrentTextPos(TextLine textLine, QTextEdit* textEdit) {

    qint16 line_nbr = mTextLinesList.indexOf(textEdit);

    // If line exist, modify it
    if ( line_nbr < mCurrentTextProp.text().count() ) {

        // Line number saved correspond to the current text edit number
        if ( mTextLinesList.count() <= mCurrentTextProp.text().count() ) {

            // Retrieve the current line font
            TextFont current_text_font = mCurrentTextProp.text()[line_nbr].Font();
            // Push the font properties in the new line
            textLine.setFont(current_text_font);
            // Replace the line in the subtitle container
            mCurrentTextProp.text()[line_nbr] = textLine;
        }
        // There more text edit than lines saved. A new line has added
        else {
            // Insert a new line with default font parameters
            mCurrentTextProp.insertText(textLine, mDefaultSub.text().first().Font(), line_nbr - 1);
        }
    }
    // Line don't exist, append a new line with default position parameters
    else {
        mCurrentTextProp.setText(textLine, mDefaultSub.text().first().Font());
    }
}

void MyTextEdit::saveCurrentTextFont(TextFont textFont, QTextEdit* textEdit) {

    qint16 line_nbr = mTextLinesList.indexOf(textEdit);

    // If line exist, modify it
    if ( line_nbr < mCurrentTextProp.text().count() ) {

        // Line number saved correspond to the current text edit number
        if ( mTextLinesList.count() <= mCurrentTextProp.text().count() ) {

            // Replace the font for "line_nbr" in current subtitle
            mCurrentTextProp.text()[line_nbr].setFont(textFont);
        }
        // There more text edit than lines saved. A new line has added
        else {
            // Insert a new line with default position parameters
            mCurrentTextProp.insertText(mDefaultSub.text().first(), textFont, line_nbr - 1);
        }
    }
    // Line don't exist, append a new line with default position parameters
    else {
        mCurrentTextProp.setText(mDefaultSub.text().first(), textFont);
    }
}

// Get the text from the text edit zones
QList<TextLine> MyTextEdit::text() {

    QList<TextLine> text_lines;

    for ( qint16 i = 0; i < mTextLinesList.count(); i++ ) {

//        QString text = mTextLinesList[i]->toPlainText();
        QString text = MyAttributesConverter::simplifyRichTextFilter( mTextLinesList[i]->toHtml() );
        TextLine line;
        line.setLine(text);
        text_lines.append(line);
    }

    return text_lines;
}

// Get the text properties (font and position)
// Return a subtitle container
MySubtitles MyTextEdit::subtitleData() {

    return mCurrentTextProp;
}


bool MyTextEdit::eventFilter(QObject* watched, QEvent* event) {

    QTextEdit* text_edit = static_cast<QTextEdit*>(watched);

    if ( event->type() == QEvent::KeyPress ) {

        QKeyEvent* key_event = static_cast<QKeyEvent*>(event);

        // Key event is insert new line
        if ( ( key_event->matches(QKeySequence::InsertParagraphSeparator) ) &&
             ( text_edit->document()->blockCount() == 1 ) ) {

            this->addLine(text_edit);
            return true;

        }
        // Key event is move to next line (down arrow)
        else if ( ( key_event->matches(QKeySequence::MoveToNextLine) ) &&
                  ( text_edit != mTextLinesList.last() ) ) {

            // Move cursor to next line and give the focus
            int next_text_line_index = mTextLinesList.indexOf(text_edit) + 1;
            mTextLinesList[next_text_line_index]->setFocus(Qt::OtherFocusReason);
            mTextLinesList[next_text_line_index]->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            return true;
        }
        // If key event is backspace & the text is empty
        else if ( ( key_event->key() == Qt::Key_Backspace ) &&
                  ( text_edit->toPlainText().isEmpty() ) &&
                  ( text_edit != mTextLinesList.first() ) ) {

            this->removeLine(text_edit);
            return true;

        }
        // Key event is move to previous line (up arrow)
        else if ( ( key_event->matches(QKeySequence::MoveToPreviousLine) ) &&
                  ( text_edit != mTextLinesList.first() ) ) {

            // Move cursor to previous line and give focus
            int previous_text_line_index = mTextLinesList.indexOf(text_edit) - 1;
            mTextLinesList[previous_text_line_index]->setFocus(Qt::OtherFocusReason);
            mTextLinesList[previous_text_line_index]->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            return true;
        }

    } // Line 1 get the focus
    else if ( event->type() == QEvent::FocusIn) {

        // Send the line properties (font, position)
        mpLastFocused = text_edit;
        emit textLineFocusChanged();
    }

    return QWidget::eventFilter(watched, event);
}

// Display text lines in the QTextEdit widgets
void MyTextEdit::setText(MySubtitles subtitle) {

    QTextEdit* text_edit;

    // Indicate that text is updating
    mIsSettingLines = true;

    // Force re-focus to send new position and font
    mpLastFocused->clearFocus();

    mPreviousTextList.clear();

    // If there are no text to display
    if ( !subtitle.isValid() ) {
        subtitle = mDefaultSub;
    }

    mCurrentTextProp = subtitle;

    QList<TextLine> text_lines = subtitle.text();

    // Remove the QTextEdit widgets useless. Always keep the first
    for ( qint16 text_edit_index = ( mTextLinesList.count() - 1 ); text_edit_index > 0; text_edit_index-- ) {

        if ( text_edit_index >= text_lines.count() ) {
            mTextLinesList.at(text_edit_index)->~QTextEdit();
            mTextLinesList.removeAt(text_edit_index);
        }
    }

    // For each lines
    for ( qint16 i = 0; i < text_lines.count(); i++ ) {

        // If there are more lines than QTextEdit, create them
        if ( i > ( mTextLinesList.count() - 1) ) {
            text_edit = this->createNewTextEdit();
            mTextLinesList.append(text_edit);
        }
        else {
            text_edit = mTextLinesList[i];
        }

        QString text_html = text_lines[i].Line();
        QString plain_text = MyAttributesConverter::htmlToPlainText(text_html);

        // Change the text edit border color function of the number of character
        if ( plain_text.count() > qApp->property("prop_MaxCharPerLine").toInt() ) {
            text_edit->setStyleSheet("background: transparent; border: 1px solid red;");
        }
        else if ( plain_text.count() == qApp->property("prop_MaxCharPerLine").toInt() ) {
            text_edit->setStyleSheet("background: transparent; border: 1px solid orange;");
        }
        else {
            text_edit->setStyleSheet("background: transparent; border: 1px solid white;");
        }

        // Apply the text to the QTextEdit
        text_edit->setHtml(text_html);

        // Apply the font to the QTextEdit
        this->setTextFont(text_edit, text_lines[i].Font(), this->size());

        // Apply the position to the QTextEdit
        this->setTextPosition(text_edit, text_lines.at(i), this->size());

        // Give the focus
        text_edit->setFocus(Qt::OtherFocusReason);
        text_edit->moveCursor(QTextCursor::End);

        // Keep a backup of the last texts displayed
        mPreviousTextList.append( MyAttributesConverter::htmlToPlainText( text_lines[i].Line() ) );
    }

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

    textEdit->setFixedWidth(widgetSize.width() - 20);

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

//        QFontMetrics font_metrics(textEdit->font());

        pos_y = ( ( widget_height / 2 ) + pos_y_offset - ( textEdit->height() / 2 ) /*+ font_metrics.descent() + ( font_metrics.leading() / 2 )*/ );
    }
    else if ( v_align == Qt::AlignBottom) {

        pos_y = ( widget_height - pos_y_offset - textEdit->height() /*+ font_metrics.descent() + ( font_metrics.leading() / 2 )*/ );
    }
    else if ( v_align == Qt::AlignTop ) {

        pos_y = ( pos_y_offset /* - textEdit->height() + font_metrics.descent() + ( font_metrics.leading() / 2 )*/ );
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

//    QFontMetrics font_metrics(textEdit->font());

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

    QString v_align = textLine.textVAlign();

    if ( v_align == "center" ) {
        textLine.setTextVAlign("center");
        v_position = ( textEdit->y() - ( widget_height / 2) + ( textEdit->height() / 2 ) );
    }
    else if (  v_align == "top" ) {
        textLine.setTextVAlign("top");
        v_position = textEdit->y();
    }
    else  {
        // bottom
        textLine.setTextVAlign("bottom");
        v_position = ( widget_height - textEdit->y() - textEdit->height() /*+ font_metrics.descent() + ( font_metrics.leading() / 2 )*/ );
    }

    textLine.setTextHPosition(  QString::number(( ( (qreal)h_position * (qreal)100 ) / (qreal)widget_width ), 'f', 1  ) );
    textLine.setTextVPosition(  QString::number(( ( (qreal)v_position * (qreal)100 ) / (qreal)widget_height ), 'f', 1  ) );

    textLine.setTextDirection("horizontal");
}

// Update current or last focussed line font
void MyTextEdit::updateTextFont(TextFont textFont, TextLine textLine) {

    // Retrieive the line position before. Because if font size change,
    // the position need to be recomputed
    if ( mpLastFocused != NULL ) {
        this->setTextFont(mpLastFocused, textFont, this->size());
        this->setTextPosition(mpLastFocused, textLine, this->size());
    }
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
    qreal outline_width;

    QFont font = textEdit->font();

    if ( textFont.fontBorderEffect() == "yes" ) {
        font.setStyleStrategy(QFont::ForceOutline);
    }

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

    outline_width = relative_font_size / 60.0;

    font.setPointSizeF(relative_font_size);

    // Set Italic
    font.setItalic( textFont.fontItalic().contains("yes", Qt::CaseInsensitive) );

    // Set Underlined
    font.setUnderline( textFont.fontUnderlined().contains("yes", Qt::CaseInsensitive) );

    //textEdit->setCurrentFont(font);
    textEdit->setFont(font);

    // Set font color
    font_color = MyAttributesConverter::stringToColor( textFont.fontColor() );
    QPalette widget_palette = textEdit->palette();
    widget_palette.setColor(QPalette::Text,font_color);
    textEdit->setPalette(widget_palette);


    // Resize the height of subtitles edit zone in function of font size
    QFontMetrics font_metrics( textEdit->font() );

    if ( font_metrics.leading() < 0 ) {
        textEdit->setFixedHeight( font_metrics.height() + ( textEdit->frameWidth() * 2 ) );
    }
    else {
        textEdit->setFixedHeight( font_metrics.lineSpacing() + ( textEdit->frameWidth() * 2 ) );
    }

    // Set shadow
    if ( textFont.fontShadowEffect() == "yes" ) {

        QGraphicsDropShadowEffect* shadow_effect = static_cast<QGraphicsDropShadowEffect*> (textEdit->graphicsEffect());
        textEdit->graphicsEffect()->setEnabled(true);
        shadow_effect->setColor( MyAttributesConverter::stringToColor( textFont.fontShadowEffectColor() ) );
    }
    else {
        textEdit->graphicsEffect()->setEnabled(false);
    }

    // Set outline
    Qt::PenStyle outline_pen_style;

    if ( textFont.fontBorderEffect() == "yes" ) {
        outline_pen_style = Qt::SolidLine;
    }
    else {
        outline_pen_style = Qt::NoPen;
    }

    QColor outline_color = MyAttributesConverter::stringToColor( textFont.fontBorderEffectColor() );

    QTextCharFormat char_format;

    QPen outline_pen(outline_color, outline_width, outline_pen_style, Qt::RoundCap, Qt::RoundJoin);

    char_format.setTextOutline(outline_pen);
    if ( textFont.fontBackgroundEffect() == "yes" ) {
        char_format.setBackground( QBrush( MyAttributesConverter::stringToColor( textFont.fontBackgroundEffectColor() ) ) );
    }
    else {
        char_format.setBackground(QBrush(Qt::transparent));
    }

    qint16 cursor_position = textEdit->textCursor().position();

    mIsChangingFont = true;

    textEdit->selectAll();
    textEdit->mergeCurrentCharFormat(char_format);
    QTextCursor text_cursor = textEdit->textCursor();
    text_cursor.setPosition(cursor_position);
    textEdit->setTextCursor(text_cursor);

    mIsChangingFont = false;

    // Save this font parameter to current
    this->saveCurrentTextFont(textFont, textEdit);
}

// Retrieive the given "textEdit" font properties
// Font sized returned in "point" for 72pt = 1 inch
// Returned properties are in "String" format to be easily displayed and exported in file
void MyTextEdit::textFont(QTextEdit *textEdit, TextFont &textFont, QSize widgetSize) {

    qint16 font_size_pt;
    qreal relative_font_size_pt;
    QColor font_color;
    QColor effect_color;

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
    QPalette widget_palette = textEdit->palette();
    font_color = widget_palette.color(QPalette::Text);
    textFont.setFontColor( (QString::number( font_color.rgba(), 16 )).toUpper() );

    if ( textEdit->graphicsEffect()->isEnabled() ) {
        textFont.setFontShadowEffect("yes");
    }
    else {
        textFont.setFontShadowEffect("no");
    }

    // Get font shadow color
    QGraphicsDropShadowEffect* shadow_effect = static_cast<QGraphicsDropShadowEffect*> (textEdit->graphicsEffect());
    effect_color = shadow_effect->color();
    textFont.setFontShadowEffectColor( (QString::number( effect_color.rgba(), 16 )).toUpper() );
}

// Widget resize event
void MyTextEdit::resizeEvent(QResizeEvent *event) {

    TextLine text_line;
    TextFont text_font;

    for ( qint16 i = 0; i < mTextLinesList.count(); i++ ) {

        // Retrieive the properties non relative to widget size
        text_line = mCurrentTextProp.text().at(i);
        text_font = text_line.Font();
        // Recompute properties relative to new widget size
        this->setTextFont(mTextLinesList.at(i), text_font, this->size());
        this->setTextPosition(mTextLinesList.at(i), text_line, this->size());
    }

    QWidget::resizeEvent(event);
}

// Add a newline
void MyTextEdit::addLine(QTextEdit *textEdit) {

    // Create new line with the same properties than current line
    // Font and position, except for horizontal position
    TextLine text_line;
    TextFont text_font;
    QString current_v_align;

    qint16 text_line_index = mTextLinesList.indexOf(textEdit);

    text_line = mCurrentTextProp.text().at(text_line_index);
    text_font = text_line.Font();

    current_v_align = text_line.textVAlign();

    QTextEdit* new_text_edit = this->createNewTextEdit();
    mTextLinesList.insert(text_line_index + 1, new_text_edit);

    // Move text from the cursor to the end to the new line
    if ( !textEdit->textCursor().atBlockEnd() ) {

        mIsSettingLines = true;

        if ( textEdit->textCursor().selectedText().count() > 0 ) {
            // Nothing to do
        }
        else {
            textEdit->moveCursor(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        }

        QString html_copied = textEdit->textCursor().selection().toHtml();
        textEdit->textCursor().deleteChar();

        new_text_edit->setHtml(html_copied);

        mIsSettingLines = false;
    }

    // Move the new line to the current line position
    this->setTextFont(new_text_edit, text_font, this->size());
    this->setTextPosition(new_text_edit, text_line, this->size());

    if ( ( current_v_align == "bottom" ) || ( current_v_align == "center") ) {

        for ( qint16 i = 0; i <= text_line_index; i++ ) {

            text_line = mCurrentTextProp.text().at(i);

            if ( text_line.textVAlign() == current_v_align ) {

                // Move current line on top of the new line with spacing defined by LINE_SPACING
                mTextLinesList[i]->move( mTextLinesList[i]->x(), ( mTextLinesList[i]->y() - mTextLinesList[i]->height() - LINE_SAPACING ) );

                // Save current line new position
                this->textPosition(mTextLinesList[i], text_line, this->size());
                this->saveCurrentTextPos(text_line, mTextLinesList[i]);
            }
        }

    }
    else if ( current_v_align == "top" ) {

        for ( qint16 i = (text_line_index + 1); i < mTextLinesList.count(); i++ ) {

            if ( i != ( text_line_index + 1 ) ) {
                text_line = mCurrentTextProp.text().at(i);
            }

            if ( text_line.textVAlign() == current_v_align ) {

                // Move current line on bottom of the new line with spacing defined by LINE_SPACING
                mTextLinesList[i]->move( mTextLinesList[i]->x(), ( mTextLinesList[i]->y() + mTextLinesList[i]->height() + LINE_SAPACING ) );

                // Save current line new position
                this->textPosition(mTextLinesList[i], text_line, this->size());
                this->saveCurrentTextPos(text_line, mTextLinesList[i]);
            }
        }
    }

    mPreviousTextList.insert(text_line_index + 1, mTextLinesList[text_line_index + 1]->toPlainText());

    new_text_edit->setFocus(Qt::OtherFocusReason);
    new_text_edit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);

    emit cursorPositionChanged();
    emit subDatasChanged( subtitleData() );
}


// Remove a line
void MyTextEdit::removeLine(QTextEdit *textEdit) {

    if ( textEdit != mTextLinesList.first() ) {

        TextLine text_line;
        QString current_v_align;
        qint32 lines_sapcing;
        bool lines_to_move = true;

        qint16 text_line_index = mTextLinesList.indexOf(textEdit);

        text_line = mCurrentTextProp.text().at(text_line_index);
        current_v_align = text_line.textVAlign();

        if ( ( current_v_align == "bottom" ) || ( current_v_align == "center" ) ) {
            lines_sapcing =  mTextLinesList[text_line_index]->y() - mTextLinesList[text_line_index - 1]->y();
        }
        else if ( current_v_align == "top" ) {

            if ( (text_line_index + 1) < mTextLinesList.count() ) {

                lines_sapcing =  mTextLinesList[text_line_index + 1]->y() - mTextLinesList[text_line_index]->y();
            }
            else {
                lines_to_move = false;
            }
        }

        // Delete the line
        mTextLinesList.at(text_line_index)->~QTextEdit();
        mTextLinesList.removeOne(textEdit);
        mCurrentTextProp.removeTextAt(text_line_index);
        mPreviousTextList.removeAt(text_line_index);

        // and move the previous/next line to its position
        if ( lines_to_move == true ) {

            // Move previous lines
            if ( ( current_v_align == "bottom" ) || ( current_v_align == "center" ) ) {

                for ( qint16 i = 0; i < text_line_index; i++ ) {

                    text_line = mCurrentTextProp.text().at(i);

                    if ( text_line.textVAlign() == current_v_align ) {

                        mTextLinesList[i]->move( mTextLinesList[i]->x(), mTextLinesList[i]->y() + lines_sapcing);

                        // Save current line new position
                        this->textPosition(mTextLinesList[i], text_line, this->size());
                        this->saveCurrentTextPos(text_line, mTextLinesList[i]);
                    }
                }
            }
            // Move next lines
            else if ( current_v_align == "top" ) {

                for ( qint16 i = text_line_index; i < mTextLinesList.count(); i++ ) {

                    text_line = mCurrentTextProp.text().at(i);

                    if ( text_line.textVAlign() == current_v_align ) {

                        mTextLinesList[i]->move( mTextLinesList[i]->x(), mTextLinesList[i]->y() - lines_sapcing);

                        // Save current line new position
                        this->textPosition(mTextLinesList[i], text_line, this->size());
                        this->saveCurrentTextPos(text_line, mTextLinesList[i]);
                    }
                }
            }
        }

        mpLastFocused = NULL;

        emit cursorPositionChanged();
        emit subDatasChanged( subtitleData() );

        mTextLinesList[text_line_index - 1]->setFocus(Qt::OtherFocusReason);
        mTextLinesList[text_line_index - 1]->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    }
}


// Manage the maximum number of characters in one line
void MyTextEdit::wrapText(QTextEdit *textEdit) {

    // If the number of characters of the current line exceed the maximum specified
    if ( textEdit->toPlainText().count() > qApp->property("prop_MaxCharPerLine").toInt() ) {

        mIsSettingLines = true;

        qint16 text_line_index = mTextLinesList.indexOf(textEdit);

        // Characters added
        if ( ( textEdit->toPlainText().count() > mPreviousTextList[text_line_index].count() ) ) {

            QTextEdit temp_text_edit;
            temp_text_edit.setLineWrapMode(QTextEdit::FixedColumnWidth);
            temp_text_edit.setLineWrapColumnOrWidth(qApp->property("prop_MaxCharPerLine").toInt());

            bool cursor_at_end = true;
            qint16 original_cursor_pos = 0;

            // Select all characters superior to the maximum specified
            QTextCursor original_text_cursor = textEdit->textCursor();
            if ( original_text_cursor.atBlockEnd() == false ) {
                cursor_at_end = false;
                original_cursor_pos = original_text_cursor.position();
            }

            QTextCursor moved_text_cursor = original_text_cursor;
            moved_text_cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);

            if ( mPreviousTextList[text_line_index].count() >  qApp->property("prop_MaxCharPerLine").toInt() ) {
                moved_text_cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, textEdit->toPlainText().count() - mPreviousTextList[text_line_index].count() );
            }
            else {
                moved_text_cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, textEdit->toPlainText().count() - qApp->property("prop_MaxCharPerLine").toInt());
            }

            QTextCursor moved_text_cursor2 = moved_text_cursor;
            moved_text_cursor2.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);

            if ( moved_text_cursor2.position() <= 0 ) {
                textEdit->setTextCursor(moved_text_cursor);
            }
            else {
                textEdit->setTextCursor(moved_text_cursor2);
            }

            // Create a new line
            this->addLine(textEdit);

            if ( cursor_at_end == false ) {
                textEdit->setFocus(Qt::OtherFocusReason);
                original_text_cursor.setPosition(original_cursor_pos);
                textEdit->setTextCursor(original_text_cursor);
            }
        }

        mIsSettingLines = false;
    }
}

void MyTextEdit::newCursorPosition() {

    // If a new text line is setting, cursor has not moved by user but automatically.
    // So do nothing
    if ( ( mIsSettingLines == false ) &&
        ( mIsChangingFont == false ) ) {

        bool text_changed = false;

        // If user has changed the text, send a signal
        for ( qint16 i = 0; i < mTextLinesList.count(); i++ ) {

            QString current_text = mTextLinesList.at(i)->toPlainText();
            if ( current_text != mPreviousTextList.at(i) ) {

                // Change the text edit border color function of the number of character
                if ( current_text.count() > qApp->property("prop_MaxCharPerLine").toInt() ) {
                    mTextLinesList[i]->setStyleSheet("background: transparent; border: 1px solid red;");
                }
                else if ( current_text.count() == qApp->property("prop_MaxCharPerLine").toInt() ) {
                    mTextLinesList[i]->setStyleSheet("background: transparent; border: 1px solid orange;");
                }
                else {
                    mTextLinesList[i]->setStyleSheet("background: transparent; border: 1px solid white;");
                }

                // Conflict between "stylesheet" and font color. Force to re-set the font parameters
                this->setTextFont(mTextLinesList.at(i), mCurrentTextProp.text()[i].Font(), this->size());

                text_changed = true;
                break;
            }
        }

        if ( text_changed == true ) {

//            this->wrapText(mpLastFocused);

            mPreviousTextList.clear();

            for ( qint16 i = 0; i < mTextLinesList.count(); i++ ) {

                mPreviousTextList.append(mTextLinesList[i]->toPlainText());
            }

            emit cursorPositionChanged();
        }
    }
}

qint16 MyTextEdit::lastFocused() {

    qint16 last_focussed_index = mTextLinesList.indexOf(mpLastFocused);

    if ( last_focussed_index >= 0 ) {
        return last_focussed_index;
    }
    else {
        return 0;
    }
}

// Manage the text edit context menu
void MyTextEdit::showCustomContextMenu(const QPoint &pos) {

    QTextEdit* textEdit = mpLastFocused;

    // Set context menu position
    QPoint global_pos = textEdit->mapToGlobal(pos);

    // Build the menu
    QMenu custom_menu;
    custom_menu.addAction("Italic");
    custom_menu.actions().last()->setCheckable(true);
    custom_menu.addAction("Underline");
    custom_menu.actions().last()->setCheckable(true);
    custom_menu.addAction("Text color...");
    // ...

    bool italic_found = false;
    bool underlined_found = false;
    bool data_changed = false;


    if ( textEdit->fontItalic() ) {
        custom_menu.actions().at(0)->setChecked(true);
        italic_found = true;
    }

    if ( textEdit->fontUnderline() ) {
        custom_menu.actions().at(1)->setChecked(true);
        underlined_found = true;
    }

    // Execute the menu and retrieve the selected action
    QAction* selected_item = custom_menu.exec(global_pos);

    if ( selected_item ) {

        // "Italic"
        if ( selected_item->text() == "Italic" ) {

            if ( italic_found ) {
                textEdit->setFontItalic(false);
            }
            else {
                textEdit->setFontItalic(true);
            }

            data_changed = true;
        }
        // "Underline"
        else if ( selected_item->text() == "Underline" ) {

            if ( underlined_found ) {
                textEdit->setFontUnderline(false);
            }
            else {
                textEdit->setFontUnderline(true);
            }

            data_changed = true;
        }
        // "Text color"
        else if ( selected_item->text() == "Text color..." ) {

            // Open a color chooser dialog
            QColor font_color = QColorDialog::getColor(textEdit->palette().color(QPalette::Text), this, tr("Select text color"), QColorDialog::ShowAlphaChannel);

            // If valid color, set to all selected subtitles
            if ( font_color.isValid() ) {

                textEdit->setTextColor(font_color);

                data_changed = true;
            }
            else {
                data_changed = false;
            }
        }

        // There are change to load in the database
        if ( data_changed == true ) {

            emit cursorPositionChanged();
        }
    }
    else {
        // nothing was chosen
    }
}

