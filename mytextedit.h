#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QWidget>
#include <QString>
#include <QTextEdit>
#include "mysubtitles.h"

namespace Ui {
class MyTextEdit;
}

// This widget class manage the edit zone.
// Manage 2 lines of editable text, with position and font attributes
class MyTextEdit : public QWidget
{
    Q_OBJECT
    
public:
    explicit MyTextEdit(QWidget *parent = 0);
    ~MyTextEdit();

signals:
    void cursorPositionChanged();
    void subDatasChanged(MySubtitles);
    void textLineFocusChanged(TextFont, TextLine);

public slots :
    void setText(MySubtitles subtitle);
    QList<TextLine> text();
    MySubtitles subtitleData();
    void updateTextPosition(TextLine textLine);
    void updateTextFont(TextFont textFont);
    MySubtitles getDefaultSub();
    void defaultSub();

private slots:
     bool eventFilter(QObject* watched, QEvent* event);
     void resizeEvent(QResizeEvent* event);
     void newCursorPosition();
     void setTextPosition(QTextEdit* textEdit, TextLine textLine, QSize widgetSize);
     void textPosition(QTextEdit* textEdit, TextLine &textLine, QSize widgetSize);
     void setTextFont(QTextEdit* textEdit, TextFont textFont, QSize widgetSize);
     void textFont(QTextEdit* textEdit, TextFont &textFont, QSize widgetSize);
    
private:
    Ui::MyTextEdit *ui;

    // Test flag : text is updating from the database
    bool mIsSettingLines;
    // Temp : defaut subtitle container
    MySubtitles mDefaultSub;
    // Save of the size of the widget before resizing
    QSize mPreviousWidgetSize;
    // Number of pixels per inch for the current hardware
    qint32 mPxPerInch;
    // Last text zone focused
    QTextEdit* mpLastFocused;
    // Copy of the last text wrote from the database
    QString mPreviousText1;
    QString mPreviousText2;
};

#endif // MYTEXTEDIT_H
