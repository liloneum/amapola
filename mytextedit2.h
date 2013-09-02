#ifndef MYTEXTEDIT2_H
#define MYTEXTEDIT2_H

#include <QWidget>
#include <QString>
#include <QTextEdit>
#include "mysubtitles.h"

namespace Ui {
class MyTextEdit2;
}

class MyTextEdit2 : public QWidget
{
    Q_OBJECT
    
public:
    explicit MyTextEdit2(QWidget *parent = 0);
    ~MyTextEdit2();

signals:
    void cursorPositionChanged();
    void subDatasChanged(MySubtitles);
    void textLineFocusChanged(TextFont);

public slots :
    void setText(MySubtitles subtitle);
    QList<TextLine> text();
    MySubtitles subtitleData();
    void updateTextPosition(QList<TextLine> textLines);
    void updateTextFont(TextFont textFont);

private slots:
     bool eventFilter(QObject* watched, QEvent* event);
     void resizeEvent(QResizeEvent* event);
     void newCursorPosition();
     void setTextPosition(QTextEdit* textEdit, TextLine textLine, QSize widgetSize);
     void textPosition(QTextEdit* textEdit, TextLine &textLine, QSize widgetSize);
     void setTextFont(QTextEdit* textEdit, TextFont textFont, QSize widgetSize);
     void textFont(QTextEdit* textEdit, TextFont &textFont, QSize widgetSize);

     void defaultSub();
    
private:
    Ui::MyTextEdit2 *ui;
    QColor mColor;
    bool mIsSettingLines;
    MySubtitles mDefaultSub;
    QSize mPreviousWidgetSize;
    qint32 mPxPerInch;
    QTextEdit* mpLastFocused;
};

#endif // MYTEXTEDIT2_H
