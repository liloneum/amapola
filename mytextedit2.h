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

public slots :
    void setText(MySubtitles subtitle);
    QList<TextLine> text();
    MySubtitles subtitleData();

private slots:
     bool eventFilter(QObject* watched, QEvent* event);
     void resizeEvent(QResizeEvent* event);
     void newCursorPosition();
     void setTextPosition(QTextEdit* textEdit, TextLine textLine);
     void defaultSub();
    
private:
    Ui::MyTextEdit2 *ui;
    QColor mColor;
    bool mIsSettingLines;
    MySubtitles mDefaultSub;
};

#endif // MYTEXTEDIT2_H
