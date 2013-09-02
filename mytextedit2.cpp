#include "mytextedit2.h"
#include "ui_mytextedit2.h"
#include "myattributesconverter.h"
#include <QKeyEvent>
#include <QTextDocumentFragment>

#define MAX_CHAR_BY_LINE 50

#define POS1_X 30
#define POS1_Y 180
#define POS2_X 30
#define POS2_Y 200

#define FONT_ID_DEFAULT_VALUE ""
#define FONT_COLOR_DEFAULT_VALUE "FFFFFFFF"
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

#define TEXT2_DIRECTION_DEFAULT_VALUE "horizontal"
#define TEXT2_HALIGN_DEFAULT_VALUE "center"
#define TEXT2_HPOSITION_DEFAULT_VALUE "0"
#define TEXT2_VALIGN_DEFAULT_VALUE "bottom"
#define TEXT2_VPOSITION_DEFAULT_VALUE "14"


MyTextEdit2::MyTextEdit2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyTextEdit2)
{
    ui->setupUi(this);

    //ui->stEditDisplay->setStyleSheet("background: transparent; color: yellow");

    // Add an event filter on the subtitles textedit object
    ui->textLine1->installEventFilter(this);
    ui->textLine2->installEventFilter(this);

    ui->textLine2->setEnabled(false);
    ui->textLine2->hide();
    mColor = ui->textLine1->textColor();
    ui->textLine1->move(POS2_X, POS2_Y);

    mIsSettingLines = false;

    this->defaultSub();

    connect(ui->textLine1, SIGNAL(cursorPositionChanged()), this, SLOT(newCursorPosition()));
    connect(ui->textLine2, SIGNAL(cursorPositionChanged()), this, SLOT(newCursorPosition()));
}

MyTextEdit2::~MyTextEdit2()
{
    delete ui;
}

void MyTextEdit2::defaultSub() {

    TextLine default_line;
    TextLine default_line2;
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

    default_line2.setTextDirection( TEXT2_DIRECTION_DEFAULT_VALUE );
    default_line2.setTextHAlign( TEXT2_HALIGN_DEFAULT_VALUE );
    default_line2.setTextVAlign( TEXT2_VALIGN_DEFAULT_VALUE );
    default_line2.setTextHPosition( TEXT2_HPOSITION_DEFAULT_VALUE );
    default_line2.setTextVPosition( TEXT2_VPOSITION_DEFAULT_VALUE );

    mDefaultSub.setText(default_line, default_font);
    mDefaultSub.setText(default_line2, default_font);
}

QList<TextLine> MyTextEdit2::text() {

    QList<TextLine> text_lines;

   if ( ui->textLine2->isEnabled() ) {

        QString text1 = ui->textLine1->toHtml();
        TextLine line1;
        line1.setLine(text1);
        text_lines.append(line1);

        QString text2 = ui->textLine2->toHtml();
        TextLine line2;
        line2.setLine(text2);
        text_lines.append(line2);
    }
    else {

        QString text1 = ui->textLine1->toHtml();
        TextLine line1;
        line1.setLine(text1);
        text_lines.append(line1);
    }
    return text_lines;
}

MySubtitles MyTextEdit2::subtitleData() {

    MySubtitles subtitle;
    TextLine new_line = this->text().first();
    TextFont new_font;

    QTextDocumentFragment text_fragment = QTextDocumentFragment::fromHtml(new_line.Line());

    if ( !text_fragment.toPlainText().isEmpty() ) {
        //TODO retrieve font, position data
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

                    this->setTextPosition(ui->textLine1, mDefaultSub.text().first());
                    //ui->textLine1->move(POS1_X, POS1_Y);
                    ui->textLine2->setEnabled(true);
                    this->setTextPosition(ui->textLine2, mDefaultSub.text().last());
                    ui->textLine2->show();
                    ui->textLine2->setFocus(Qt::OtherFocusReason);

                    //ui->textLine2->move(POS2_X, POS2_Y);
                }
                return true;
            }
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
                ui->textLine1->move(POS2_X, POS2_Y);
                ui->textLine2->setEnabled(false);
                ui->textLine2->hide();
                emit cursorPositionChanged();
                ui->textLine1->setFocus(Qt::OtherFocusReason);
                ui->textLine1->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

void MyTextEdit2::setText(MySubtitles subtitle) {

    if ( subtitle.isValid() ) {

        bool ok;

        QList<TextLine> TextLines = subtitle.text();

        mIsSettingLines = true;

        if ( TextLines.count() == 2 ) {

            QColor color = QColor::fromRgba( TextLines.first().Font().fontColor().toUInt(&ok,16) );
            ui->textLine1->setTextColor(color);
            color = QColor::fromRgba( TextLines.last().Font().fontColor().toUInt(&ok,16) );
            ui->textLine2->setTextColor(color);

            ui->textLine1->setFontItalic( MyAttributesConverter::isItalic( TextLines.first().Font().fontItalic() ) );
            ui->textLine2->setFontItalic( MyAttributesConverter::isItalic( TextLines.last().Font().fontItalic() ) );

            ui->textLine1->setText( TextLines.first().Line() );
            ui->textLine2->setText( TextLines.last().Line() );

            this->setTextPosition(ui->textLine1, TextLines.first());

//            ui->textLine1->setAlignment( MyAttributesConverter::hAlignFromString( TextLines.first().textHAlign() ) );
//            ui->textLine2->setAlignment( MyAttributesConverter::hAlignFromString( TextLines.last().textHAlign() ) );

//            ui->textLine1->move(POS1_X, POS1_Y);
            ui->textLine2->setEnabled(true);
            this->setTextPosition(ui->textLine2, TextLines.last());
//            ui->textLine2->move(POS2_X, POS2_Y);
            ui->textLine2->show();
        }
        else if ( TextLines.count() == 1 ) {

            QColor color = QColor::fromRgba( TextLines.first().Font().fontColor().toUInt(&ok,16) );
            ui->textLine1->setTextColor(color);
            ui->textLine2->setTextColor(color);

            ui->textLine1->setFontItalic( MyAttributesConverter::isItalic( TextLines.first().Font().fontItalic() ) );
            ui->textLine2->setFontItalic( MyAttributesConverter::isItalic( TextLines.first().Font().fontItalic() ) );

            ui->textLine1->setText( TextLines.first().Line() );
            ui->textLine2->setText("");

//            ui->textLine1->setAlignment( MyAttributesConverter::hAlignFromString( TextLines.first().textHAlign() ) );
//            ui->textLine2->setAlignment( MyAttributesConverter::hAlignFromString( TextLines.first().textHAlign() ) );

            this->setTextPosition(ui->textLine1, TextLines.first());

//            ui->textLine1->move(POS2_X, POS2_Y);
            ui->textLine2->setEnabled(false);
            ui->textLine2->hide();
        }
    }
    else {

        ui->textLine1->setTextColor(mColor);
        ui->textLine2->setTextColor(mColor);

        ui->textLine1->setFontItalic( MyAttributesConverter::isItalic( "no" ) );
        ui->textLine2->setFontItalic( MyAttributesConverter::isItalic( "no" ) );

        ui->textLine1->setText("");
        ui->textLine2->setText("");

        ui->textLine1->setAlignment( MyAttributesConverter::hAlignFromString("center") );
        ui->textLine2->setAlignment( MyAttributesConverter::hAlignFromString("center") );

        this->setTextPosition(ui->textLine1, mDefaultSub.text().first());
        //ui->textLine1->move(POS2_X, POS2_Y);
        ui->textLine2->setEnabled(false);
        ui->textLine2->hide();
    }

    mIsSettingLines = false;
}

void MyTextEdit2::setTextPosition(QTextEdit* textEdit, TextLine textLine ) {

    bool ok;
    qint32 pos_x_offset;
    qint32 pos_y_offset;
    qint32 pos_x;
    qint32 pos_y;
    Qt::Alignment h_align;
    Qt::Alignment v_align;

    QFontMetrics font_metrics(textEdit->font());

    h_align = MyAttributesConverter::hAlignFromString( textLine.textHAlign() );
    textEdit->setAlignment(h_align);
    pos_x_offset = (qint32)( ( (qreal)this->width() * (qreal)textLine.textHPosition().toFloat(&ok) ) / (qreal)100 );

    if ( h_align == Qt::AlignCenter ) {

        pos_x = ( ( this->width() / 2 ) + pos_x_offset );
    }
    else if ( h_align == Qt::AlignLeft ) {

        pos_x = pos_x_offset;
    }
    else if ( h_align == Qt::AlignRight ) {

        pos_x = this->width() - textEdit->width() - pos_x_offset;
    }


    v_align = MyAttributesConverter::vAlignFromString( textLine.textVAlign() );
    pos_y_offset = (qint32)( ( (qreal)this->height() * (qreal)textLine.textVPosition().toFloat(&ok) ) / (qreal)100 );

    if ( v_align == Qt::AlignCenter ) {

        QFontMetrics font_metrics(textEdit->font());

        pos_y = ( ( this->height() / 2 ) + pos_y_offset - textEdit->height() + font_metrics.descent() + ( font_metrics.leading() / 2 ) );
    }
    else if ( v_align == Qt::AlignBottom) {

        pos_y = ( this->height() - pos_y_offset - textEdit->height() + font_metrics.descent() + ( font_metrics.leading() / 2 ) );
    }
    else if ( v_align == Qt::AlignTop ) {

        pos_y = ( pos_y_offset - textEdit->height() + font_metrics.descent() + ( font_metrics.leading() / 2 ) );
    }

    pos_x = qBound ( 0, pos_x, this->width() );
    pos_y = qBound ( 0, pos_y, this->height() );

    textEdit->move(pos_x, pos_y);
}

void MyTextEdit2::resizeEvent(QResizeEvent *event) {

    qint32 row_height;
    qint32 text_edit_height;
    qint32 text_edit_width;
    QString test_string_width;

    test_string_width.fill('W', MAX_CHAR_BY_LINE);


    // Resize the height of subtitles edit zone in function of font size
    QFontMetrics font_metrics1(ui->textLine1->font());
    row_height = font_metrics1.lineSpacing();
    text_edit_height = (row_height);
    text_edit_width = font_metrics1.width(test_string_width);
    ui->textLine1->setFixedHeight(text_edit_height);
    ui->textLine1->setFixedWidth(text_edit_width);

    QFontMetrics font_metrics2(ui->textLine2->font());
    row_height = font_metrics2.lineSpacing();
    text_edit_height = (row_height);
    text_edit_width = font_metrics1.width(test_string_width);
    ui->textLine2->setFixedHeight(text_edit_height);
    ui->textLine2->setFixedWidth(text_edit_width);

    if (ui->textLine2->isEnabled()) {

        ui->textLine1->move(POS1_X, POS1_Y);
        ui->textLine2->move(POS2_X, POS2_Y);
    }
    else {
        ui->textLine1->move(POS2_X, POS2_Y);
        ui->textLine2->hide();
    }

    QWidget::resizeEvent(event);
}

void MyTextEdit2::newCursorPosition() {

    if ( mIsSettingLines == false ) {
        emit cursorPositionChanged();
    }
}
