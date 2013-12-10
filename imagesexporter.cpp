#include "imagesexporter.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QTextEdit>
#include <QGraphicsDropShadowEffect>
#include <QFile>
#include <mysubtitles.h>
#include "myattributesconverter.h"
#include <QBitmap>


// Fonts are rendered as if the screen height is 11 inches,
// so a 72pt font would be 1/11 screen height.
#define REF_HEIGHT_INCH 11
#define PT_PER_INCH 72


ImagesExporter::ImagesExporter(QSize widgetSize, QWidget *parent) :
    QWidget(parent)
{
    // Retrieive the number of pixels per inch for the current hardware
    mPxPerInch = qApp->desktop()->logicalDpiY();

    // Set the size of the widget
//    widgetSize = widgetSize * 4;
    this->setFixedSize(widgetSize);

//    this->setStyleSheet("background: white;");
    QPalette widget_palette = this->palette();
    widget_palette.setColor(QPalette::Window, Qt::white);
    this->setPalette(widget_palette);
}

QTextEdit* ImagesExporter::createNewTextEdit() {

    QTextEdit* text_edit = new QTextEdit(this);

    // Set text zones colors
    text_edit->setStyleSheet("background-color: transparent");
//    QPalette text_edit_palette = text_edit->palette();
//    text_edit_palette.setColor(QPalette::Window, Qt::transparent);
//    text_edit->setPalette(text_edit_palette);
    text_edit->setFrameShape(QFrame::NoFrame);

    // Install a shadow effect on the text edit widget
    QGraphicsDropShadowEffect* shadow_effect = new QGraphicsDropShadowEffect(this);
    shadow_effect->setBlurRadius(3);
    shadow_effect->setColor(QColor("#" +qApp->property("prop_FontShadowColor").toString()));
    shadow_effect->setOffset(2,2);
    shadow_effect->setEnabled(false);
    text_edit->setGraphicsEffect(shadow_effect);

    // Disable scroll bar
    text_edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    text_edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    return text_edit;
}

// Display text lines in the QTextEdit widgets
void ImagesExporter::setText(MySubtitles subtitle) {

    QTextEdit* text_edit;

    QList<TextLine> text_lines = subtitle.text();

    // Remove the QTextEdit widgets useless. Always keep the first
    if ( mTextLinesList.count() > 0 ) {
        for ( qint16 text_edit_index = 0; text_edit_index < mTextLinesList.count(); text_edit_index++ ) {

            mTextLinesList.at(text_edit_index)->~QTextEdit();
        }
        mTextLinesList.clear();
    }

    // For each lines
    for ( qint16 i = 0; i < text_lines.count(); i++ ) {

        // Create a new QTextEdit
        text_edit = this->createNewTextEdit();
        mTextLinesList.append(text_edit);

        QString text_html = text_lines[i].Line();

        // Apply the text to the QTextEdit
        text_edit->setHtml(text_html);

        // Apply the font to the QTextEdit
        this->setTextFont(text_edit, text_lines[i].Font(), this->size());

        // Apply the position to the QTextEdit
        this->setTextPosition(text_edit, text_lines.at(i), this->size());

        // Give the focus
        text_edit->clearFocus();
    }
}

// Set the position of the given "textEdit" zone
// Position is defined by :
// - Vertical alignment (top, center, bottom)
// - Horizontal alignment (left, center, right)
// - Vertical coordonate (ref : top of the widget)
// - Horizontal coordonate (ref : left of the widget)
void ImagesExporter::setTextPosition(QTextEdit* textEdit, TextLine textLine, QSize widgetSize ) {

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

        pos_y = ( ( widget_height / 2 ) + pos_y_offset - ( textEdit->height() / 2 ) );
    }
    else if ( v_align == Qt::AlignBottom) {

        pos_y = (widget_height - pos_y_offset - textEdit->height());
    }
    else if ( v_align == Qt::AlignTop ) {

        pos_y = (pos_y_offset);
    }

    // Move the line to the new position
    textEdit->move(pos_x, pos_y);
}

// Set the font of the given textEdit zone
// Font is defined by :
// - Font name
// - Size
// - Color
// - Italic (yes/no)
// - Underlined (yes/no)
void ImagesExporter::setTextFont(QTextEdit *textEdit, TextFont textFont, QSize widgetSize) {

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

//    outline_width = relative_font_size / 60.0;
    outline_width = relative_font_size / 10.0;

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
        textEdit->setFixedHeight( font_metrics.height() + 4/*+ ( textEdit->frameWidth() * 2 )*/ );
    }
    else {
        textEdit->setFixedHeight( font_metrics.lineSpacing() + 4/*+ ( textEdit->frameWidth() * 2 )*/ );
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

    textEdit->selectAll();
    textEdit->mergeCurrentCharFormat(char_format);
    QTextCursor text_cursor = textEdit->textCursor();
    text_cursor.setPosition(cursor_position);
    textEdit->setTextCursor(text_cursor);
}

// Create an image of the "subtitle" at the given "format" and with the given "backgroundColor" and "colorDepth"
// Color depth = 0 -> 4 colors (white, black, red, blue). Else it's the number of byte used for colors code (1-4)
void ImagesExporter::createImage(MySubtitles subtitle, QString fileName, QString format, QColor backgroundColor, quint16 colorDepth) {

    QImage image;

    // Generate image with white background and text bordered
    QPalette widget_palette = this->palette();
    widget_palette.setColor(QPalette::Window, backgroundColor);
    this->setPalette(widget_palette);

    this->setText(subtitle);
    QPixmap pixmap_layer0;
    QImage image_layer0;

    // 8 bits or 32 bits colors
    if ( colorDepth != 0 ) {

        this->setText(subtitle);
        pixmap_layer0 = this->grab();
        image_layer0 = pixmap_layer0.toImage();

        // Generate image with transparent background and text background
        widget_palette = this->palette();
        widget_palette.setColor(QPalette::Window, Qt::transparent);
        this->setPalette(widget_palette);

        MySubtitles temp_sub = subtitle;

        QList<TextLine> text_lines = temp_sub.text();
        QList<QRgb> background_color_list;

        bool ok;

        for ( qint16 i = 0; i < text_lines.size(); i++ ) {

            TextLine text_line = text_lines.at(i);

            if ( text_line.Font().fontBorderEffect() == "yes" ) {

                text_lines[i].Font().setFontBackgroundEffect("yes");
                text_lines[i].Font().setFontBackgroundEffectColor(text_line.Font().fontBorderEffectColor());
                background_color_list.append( QRgb( text_line.Font().fontBorderEffectColor().toUInt(&ok, 16) ) );
            }
            else if ( text_line.Font().fontBackgroundEffect() == "no") {

                text_lines[i].Font().setFontBackgroundEffect("yes");
                text_lines[i].Font().setFontBackgroundEffectColor(MyAttributesConverter::colorToString(backgroundColor));
                background_color_list.append(backgroundColor.value());
            }

            text_lines[i].Font().setFontBorderEffect("no");
            text_lines[i].Font().setFontShadowEffect("no");
        }

        temp_sub.setText(text_lines);

        this->setText(temp_sub);
        QPixmap pixmap_transparent = this->grab();
        QImage image_transparent = pixmap_transparent.toImage();
        QImage mask_image;

        for ( qint16 color_it = 0; color_it < background_color_list.size(); color_it++ ) {

            mask_image = image_transparent.createMaskFromColor(background_color_list.at(color_it), Qt::MaskInColor);
            pixmap_transparent.setMask(QPixmap::fromImage(mask_image));
        }

        image_transparent = pixmap_transparent.toImage();

        image = image_layer0;

        for ( qint16 x_it = 0; x_it < image_layer0.width(); x_it++ ) {

            for ( qint16 y_it = 0; y_it < image_layer0.height(); y_it++ ) {

                QRgb current_color = image_transparent.pixel(x_it, y_it);

                if ( current_color != 0 ) {

                    image.setPixel(x_it, y_it, current_color);
                }
            }
        }

        // Covert to 256 colors
        if ( colorDepth == 1 ) {

            image = image.convertToFormat(QImage::Format_Indexed8, Qt::ThresholdDither);
        }
    }
    // 4 colors only
    else {

        QList<TextLine> text_lines = subtitle.text();
        MySubtitles temp_sub = subtitle;

        // Deactivate the border effect
        for ( qint16 line_it = 0; line_it < text_lines.size(); line_it++ ) {

            temp_sub.text()[line_it].Font().setFontBorderEffect("no");
        }

        this->setText(temp_sub);
        pixmap_layer0 = this->grab();
        image = pixmap_layer0.toImage();

        QVector<QRgb> color_table;
        color_table.append(QRgb(0xFFFFFFFF));
        color_table.append(QRgb(0xFF000000));
        color_table.append(QRgb(0xFFFF0000));
        color_table.append(QRgb(0xFF0000FF));

        // Convert to 4 colors
        image = image.convertToFormat(QImage::Format_Indexed8, color_table);

        if ( subtitle.text().first().Font().fontBorderEffect() == "yes" ) {

            // Draw the border
            for ( qint16 x_it = 0; x_it < image.width(); x_it++ ) {

                for ( qint16 y_it = 0; y_it < image.height(); y_it++ ) {

                    QRgb cur_pixel_color = image.pixel(x_it, y_it);

                    if ( ( cur_pixel_color != color_table.at(0) ) && ( cur_pixel_color != color_table.at(1) ) ) {

                        for ( qint16 xx = (x_it - 1); xx < (x_it + 2); xx++ ) {

                            if ( ( xx >= 0 ) && ( xx < image.width() ) && ( xx != x_it ) ) {

                                QRgb side_pixel_color = image.pixel(xx, y_it);

                                if ( side_pixel_color == color_table.at(0) ) {

                                    image.setPixel(xx, y_it, 1);
                                }
                            }
                        }

                        for ( qint16 yy = (y_it - 1); yy < (y_it + 2); yy++ ) {

                            if ( ( yy >= 0 ) && ( yy < image.height() ) && ( yy != y_it ) ) {

                                QRgb side_pixel_color = image.pixel(x_it, yy);

                                if ( side_pixel_color == color_table.at(0) ) {

                                    image.setPixel(x_it, yy, 1);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Write image to specified file
    QFile file_write(fileName);
    if ( !file_write.open(QIODevice::WriteOnly) ) {
        // Error
    } else {
        image.save(fileName, format.toLatin1() , 100);
        file_write.close();
    }
}
