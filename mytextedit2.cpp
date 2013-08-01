#include "mytextedit2.h"
#include "ui_mytextedit2.h"
#include "myattributesconverter.h"
#include <QKeyEvent>
#include <QTextDocumentFragment>
#include <QDesktopWidget>

#define MAX_CHAR_BY_LINE 40
#define REF_HEIGHT_INCH 11
#define PT_PER_INCH 72

#define LINE_SAPACING 2

#define FONT_ID_DEFAULT_VALUE "Arial"
#define FONT_COLOR_DEFAULT_VALUE "FF0000FF"
#define FONT_EFFECT_DEFAULT_VALUE "none"
#define FONT_EFFECT_COLOR_DEFAULT_VALUE "FF000000"
#define FONT_ITALIC_DEFAULT_VALUE "no"
#define FONT_SCRIPT_DEFAULT_VALUE "normal"
#define FONT_SIZE_DEFAULT_VALUE "42"
#define FONT_UNDERLINED_DEFAULT_VALUE "no"

#define TEXT1_DIRECTION_DEFAULT_VALUE "horizontal"
#define TEXT1_HALIGN_DEFAULT_VALUE "center"
#define TEXT1_HPOSITION_DEFAULT_VALUE "0"
#define TEXT1_VALIGN_DEFAULT_VALUE "bottom"
#define TEXT1_VPOSITION_DEFAULT_VALUE "8"

//#define TEXT2_DIRECTION_DEFAULT_VALUE "horizontal"
//#define TEXT2_HALIGN_DEFAULT_VALUE "center"
//#define TEXT2_HPOSITION_DEFAULT_VALUE "0"
//#define TEXT2_VALIGN_DEFAULT_VALUE "bottom"
//#define TEXT2_VPOSITION_DEFAULT_VALUE "8"


MyTextEdit2::MyTextEdit2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyTextEdit2)
{
    ui->setupUi(this);

    QDesktopWidget *mydesk = QApplication::desktop();
    mPxPerInch = mydesk->logicalDpiY();

    ui->textLine1->setStyleSheet("background: transparent; border: 1px solid red;");
    ui->textLine2->setStyleSheet("background: transparent; border: 1px solid blue;");

    this->defaultSub();

    // Add an event filter on the subtitles textedit object
    ui->textLine1->installEventFilter(this);
    ui->textLine2->installEventFilter(this);

    ui->textLine2->setEnabled(false);
    ui->textLine2->hide();

    mIsSettingLines = false;

    mPreviousText1 = "";
    mPreviousText2 = "";

    connect(ui->textLine1, SIGNAL(cursorPositionChanged()), this, SLOT(newCursorPosition()));
    connect(ui->textLine2, SIGNAL(cursorPositionChanged()), this, SLOT(newCursorPosition()));
}

MyTextEdit2::~MyTextEdit2()
{
    delete ui;
}

//TEMP :: DEBUG FUNCTION
void MyTextEdit2::defaultSub() {

    TextLine default_line;
//    TextLine default_line2;
    TextFont default_font;

    default_font.setFontId( FONT_ID_DEFAULT_VALUE );
    default_font.setFontColor( FONT_COLOR_DEFAULT_VALUE );
    default_font.setFontEffect( FONT_EFFECT_DEFAULT_VALUE );
    default_font.setFontEffectColor( FONT_EFFECT_COLOR_DEFAULT_VALUE );
    default_font.setFontItalic( FONT_ITALIC_DEFAULT_VALUE );
    default_font.setFontScript( FONT_SCRIPT_DEFAULT_VALUE );
    default_font.setFontSize( FONT_SIZE_DEFAULT_VALUE );
    default_font.setFontUnderlined( FONT_UNDERLINED_DEFAULT_VALUE );

    default_line.setTextDirection( TEXT1_DIRECTION_DEFAULT_VALUE );
    default_line.setTextHAlign( TEXT1_HALIGN_DEFAULT_VALUE );
    default_line.setTextVAlign( TEXT1_VALIGN_DEFAULT_VALUE );
    default_line.setTextHPosition( TEXT1_HPOSITION_DEFAULT_VALUE );
    default_line.setTextVPosition( TEXT1_VPOSITION_DEFAULT_VALUE );

//    default_line2.setTextDirection( TEXT2_DIRECTION_DEFAULT_VALUE );
//    default_line2.setTextHAlign( TEXT2_HALIGN_DEFAULT_VALUE );
//    default_line2.setTextVAlign( TEXT2_VALIGN_DEFAULT_VALUE );
//    default_line2.setTextHPosition( TEXT2_HPOSITION_DEFAULT_VALUE );
//    default_line2.setTextVPosition( TEXT2_VPOSITION_DEFAULT_VALUE );

    mDefaultSub.setText(default_line, default_font);
//    mDefaultSub.setText(default_line2, default_font);
}

MySubtitles MyTextEdit2::getDefaultSub() {

    return mDefaultSub;
}

QList<TextLine> MyTextEdit2::text() {

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


MySubtitles MyTextEdit2::subtitleData() {

    MySubtitles subtitle;
    TextLine new_line;
    TextFont new_font;

    new_line = this->text().first();

    this->textPosition(ui->textLine1, new_line, this->size());
    this->textFont(ui->textLine1, new_font, this->size());

    subtitle.setText(new_line, new_font);

    if ( ui->textLine2->isEnabled() ) {

        this->textPosition(ui->textLine2, new_line, this->size());
        this->textFont(ui->textLine2, new_font, this->size());
        subtitle.setText(new_line, new_font);
    }

    return subtitle;
}


bool MyTextEdit2::eventFilter(QObject* watched, QEvent* event) {

    if ( watched == ui->textLine1 ) {
        if ( event->type() == QEvent::KeyPress ) {

            QKeyEvent* key_event = static_cast<QKeyEvent*>(event);

            if ( ( key_event->matches(QKeySequence::InsertParagraphSeparator) ) &&
                 ( ui->textLine1->document()->blockCount() == 1 ) ) {

                if ( !ui->textLine2->isEnabled() ) {

                    TextLine text_line;
                    TextFont text_font;

                    this->textPosition(ui->textLine1, text_line, this->size());
                    this->textFont(ui->textLine1, text_font, this->size());
                    this->setTextFont(ui->textLine2, text_font, this->size());
                    this->setTextPosition(ui->textLine2, text_line, this->size());

                    ui->textLine2->move(ui->textLine1->pos());
                    ui->textLine1->move( ui->textLine1->x(), ( ui->textLine1->y() - ui->textLine1->height() - LINE_SAPACING ) );

                    ui->textLine2->setEnabled(true);
                    ui->textLine2->show();
                    ui->textLine2->setFocus(Qt::OtherFocusReason);
                    emit cursorPositionChanged();
                    emit subDatasChanged( subtitleData() );
                }
                return true;
            }
            else if ( ( key_event->matches(QKeySequence::MoveToNextLine) ) &&
                      ( ui->textLine2->isEnabled() ) ) {

                ui->textLine2->setFocus(Qt::OtherFocusReason);
                ui->textLine2->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);

                return true;
            }
        }
        else if ( event->type() == QEvent::FocusIn) {

            mpLastFocused = ui->textLine1;
            TextFont text_font;
            TextLine text_line;
            this->textFont(ui->textLine1, text_font, this->size());
            this->textPosition(ui->textLine1, text_line, this->size());
            emit textLineFocusChanged(text_font, text_line);
        }
    }
    else if ( watched == ui->textLine2) {
        if ( event->type() == QEvent::KeyPress ) {

            QKeyEvent* key_event = static_cast<QKeyEvent*>(event);

            if ( ( key_event->matches(QKeySequence::InsertParagraphSeparator) ) &&
                 ( ui->textLine2->document()->blockCount() == 1 ) ) {
                return true;
            }
            else if ( ( key_event->key() == (Qt::Key_Backspace) ) &&
                      ( ui->textLine2->toPlainText().isEmpty() ) ) {

                ui->textLine1->move(ui->textLine2->pos());
                ui->textLine2->setEnabled(false);
                ui->textLine2->hide();
                emit cursorPositionChanged();
                emit subDatasChanged( subtitleData() );
                ui->textLine1->setFocus(Qt::OtherFocusReason);
                ui->textLine1->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
                return true;
            }
            else if ( key_event->matches(QKeySequence::MoveToPreviousLine) ) {

                ui->textLine1->setFocus(Qt::OtherFocusReason);
                ui->textLine1->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
                return true;
            }
        }
        else if ( event->type() == QEvent::FocusIn) {

            mpLastFocused = ui->textLine2;
            TextFont text_font;
            TextLine text_line;
            this->textFont(ui->textLine2, text_font, this->size());
            this->textPosition(ui->textLine2, text_line, this->size());
            emit textLineFocusChanged(text_font, text_line);
        }
    }

    return QWidget::eventFilter(watched, event);
}


void MyTextEdit2::setText(MySubtitles subtitle) {

    mIsSettingLines = true;

    if ( subtitle.isValid() ) {

        QList<TextLine> TextLines = subtitle.text();

        mIsSettingLines = true;

        if ( TextLines.count() == 2 ) {

            this->setTextFont(ui->textLine1, TextLines.first().Font(), this->size());
            this->setTextFont(ui->textLine2, TextLines.last().Font(), this->size());

            ui->textLine1->setText( TextLines.first().Line() );
            ui->textLine2->setText( TextLines.last().Line() );

            this->setTextPosition(ui->textLine1, TextLines.first(), this->size());
            this->setTextPosition(ui->textLine2, TextLines.last(), this->size());

            ui->textLine2->setEnabled(true);
            ui->textLine2->show();

            ui->textLine2->setFocus(Qt::OtherFocusReason);
            ui->textLine2->moveCursor(QTextCursor::End);

        }
        else if ( TextLines.count() == 1 ) {

            this->setTextFont(ui->textLine1, TextLines.first().Font(), this->size());

            ui->textLine1->setText( TextLines.first().Line() );
            ui->textLine2->setText("");

            this->setTextPosition(ui->textLine1, TextLines.first(), this->size());

            ui->textLine2->setEnabled(false);
            ui->textLine2->hide();

            ui->textLine1->setFocus(Qt::OtherFocusReason);
            ui->textLine1->moveCursor(QTextCursor::End);
        }
    }
    else {

        ui->textLine1->setText("");
        ui->textLine2->setText("");

        this->setTextFont(ui->textLine1, mDefaultSub.text().first().Font(), this->size());

        this->setTextPosition(ui->textLine1, mDefaultSub.text().first(), this->size());

//        QPalette palette = ui->textLine1->palette();
//        palette.setColor(QPalette::Foreground,Qt::red);
//        ui->textLine1->setPalette(palette);

//        palette = ui->textLine2->palette();
//        palette.setColor(QPalette::Foreground,Qt::blue);
//        ui->textLine2->setPalette(palette);

        ui->textLine2->setEnabled(false);
        ui->textLine2->hide();

        ui->textLine1->setFocus(Qt::OtherFocusReason);
        ui->textLine1->moveCursor(QTextCursor::End);
    }

    mPreviousText1 = ui->textLine1->toPlainText();
    mPreviousText2 = ui->textLine2->toPlainText();

    mIsSettingLines = false;
}

void MyTextEdit2::updateTextPosition(TextLine textLine) {

    this->setTextPosition(mpLastFocused, textLine, this->size());
    emit subDatasChanged( subtitleData() );
}

void MyTextEdit2::setTextPosition(QTextEdit* textEdit, TextLine textLine, QSize widgetSize ) {

    bool ok;
    qint32 pos_x_offset;
    qint32 pos_y_offset;
    qint32 pos_x;
    qint32 pos_y;
    Qt::Alignment h_align;
    Qt::Alignment v_align; 
    qint32 widget_width;
    qint32 widget_height;

    widget_width = widgetSize.width();
    widget_height = widgetSize.height();

    QFontMetrics font_metrics(textEdit->font());

    h_align = MyAttributesConverter::hAlignFromString( textLine.textHAlign() );
    textEdit->setAlignment(h_align);
    pos_x_offset = qRound( ( (qreal)widget_width * (qreal)textLine.textHPosition().toFloat(&ok) ) / (qreal)100 );

    if ( h_align == Qt::AlignCenter ) {

        pos_x = ( ( ( widget_width - textEdit->width() ) / 2 ) + pos_x_offset );
    }
    else if ( h_align == Qt::AlignLeft ) {

        pos_x = pos_x_offset;
    }
    else if ( h_align == Qt::AlignRight ) {

        pos_x = widget_width - textEdit->width() - pos_x_offset;
    }


    v_align = MyAttributesConverter::vAlignFromString( textLine.textVAlign() );
    pos_y_offset = qRound( ( (qreal)widget_height * (qreal)textLine.textVPosition().toFloat(&ok) ) / (qreal)100 );

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

//    pos_x = qBound ( 0, pos_x, this->width() );
//    pos_y = qBound ( 0, pos_y, this->height() );

    textEdit->move(pos_x, pos_y);
}

void MyTextEdit2::textPosition(QTextEdit* textEdit, TextLine &textLine, QSize widgetSize) {

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

    // Vertical position
    textLine.setTextVAlign("bottom");
    v_position = ( widget_height - textEdit->y() - textEdit->height() + font_metrics.descent() /*+ ( font_metrics.leading() / 2 )*/ );

    textLine.setTextHPosition(  QString::number(( ( (qreal)h_position * (qreal)100 ) / (qreal)widget_width ), 'f', 1  ) );
    textLine.setTextVPosition(  QString::number(( ( (qreal)v_position * (qreal)100 ) / (qreal)widget_height ), 'f', 1  ) );

    textLine.setTextDirection("horizontal");
}

void MyTextEdit2::updateTextFont(TextFont textFont) {

    TextLine text_line;

    this->textPosition(mpLastFocused, text_line, this->size());
    this->setTextFont(mpLastFocused, textFont, this->size());
    this->setTextPosition(mpLastFocused, text_line, this->size());
    emit subDatasChanged( subtitleData() );
}

void MyTextEdit2::setTextFont(QTextEdit *textEdit, TextFont textFont, QSize widgetSize) {

    qint16 font_size_pt;
    qreal relative_font_size;
    QColor font_color;
    bool ok;

//    QString text = textEdit->toPlainText();

//    QFont font = textEdit->currentFont();
    QFont font = textEdit->font();

    // Set font name
    //font.setFamily("Arial");
    font.setFamily( textFont.fontId() );
    // Set font size
    font_size_pt = textFont.fontSize().toInt();

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
    QString test_string_width;

    test_string_width.fill('W', MAX_CHAR_BY_LINE);
//    QFontMetrics font_metrics( textEdit->currentFont() );
    QFontMetrics font_metrics( textEdit->font() );

    textEdit->setFixedHeight( font_metrics.height() + ( textEdit->frameWidth() * 2 ) ) ;
    textEdit->setFixedWidth( font_metrics.width(test_string_width) + ( textEdit->frameWidth() * 2 ) );

//    textEdit->setText(text);
}

void MyTextEdit2::textFont(QTextEdit *textEdit, TextFont &textFont, QSize widgetSize) {

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

void MyTextEdit2::resizeEvent(QResizeEvent *event) {

    TextLine text_line;
    TextFont text_font;

    this->textPosition(ui->textLine1, text_line, mPreviousWidgetSize);
    this->textFont(ui->textLine1, text_font, mPreviousWidgetSize);
    this->setTextFont(ui->textLine1, text_font, this->size());
    this->setTextPosition(ui->textLine1, text_line, this->size());

    this->textPosition(ui->textLine2, text_line, mPreviousWidgetSize);
    this->textFont(ui->textLine2, text_font, mPreviousWidgetSize);
    this->setTextFont(ui->textLine2, text_font, this->size());
    this->setTextPosition(ui->textLine2, text_line, this->size());

    mPreviousWidgetSize = this->size();

    QWidget::resizeEvent(event);
}

void MyTextEdit2::newCursorPosition() {

    if ( mIsSettingLines == false ) {

        if ( ( ui->textLine1->toPlainText() != mPreviousText1 ) || (ui->textLine2->toPlainText() != mPreviousText2) ) {

            mPreviousText1 = ui->textLine1->toPlainText();
            mPreviousText2 = ui->textLine2->toPlainText();
            emit cursorPositionChanged();
        }
    }
}

