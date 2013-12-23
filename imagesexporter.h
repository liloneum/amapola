#ifndef IMAGESEXPORTER_H
#define IMAGESEXPORTER_H

#include <QWidget>
#include "mysubtitles.h"
#include <QTextEdit>

class ImagesExporter : public QWidget
{
    Q_OBJECT
public:
    explicit ImagesExporter(QSize widgetSize, QWidget *parent = 0);
    void createImage(MySubtitles subtitle, QString fileName, QSize imageSize, bool fullSize, QString format, QColor backgroundColor, quint16 colorDepth);
    QRect imageRect();

signals:
    
public slots:

private:

    QTextEdit* createNewTextEdit();
    void setText(MySubtitles subtitle);
    void setTextPosition(QTextEdit* textEdit, TextLine textLine, QSize widgetSize);
    void setTextFont(QTextEdit* textEdit, TextFont textFont, QSize widgetSize);

    // Number of pixels per inch for the current hardware
    qint32 mPxPerInch;
    QList<QTextEdit*> mTextLinesList;
    QRect* mpImageRect;
    
};

#endif // IMAGESEXPORTER_H
