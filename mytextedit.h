#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QWidget>
#include <QString>
#include <QTextEdit>
#include "mysubtitles.h"
#include <QGraphicsDropShadowEffect>

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
    void lineAdded(MySubtitles);
    void lineRemoved(MySubtitles);
    void textLineFocusChanged();

public slots :
    void setText(MySubtitles subtitle);
    QList<TextLine> text();
    MySubtitles subtitleData();
    void updateTextPosition(TextLine textLine);
    void updateTextFont(TextFont textFont, TextLine textLine);
    MySubtitles getDefaultSub();
    void updateDefaultSub();
    qint16 lastFocused();

private slots:
     bool eventFilter(QObject* watched, QEvent* event);
     void resizeEvent(QResizeEvent* event);
     void newCursorPosition();
     void setTextPosition(QTextEdit* textEdit, TextLine textLine, QSize widgetSize);
     void textPosition(QTextEdit* textEdit, TextLine &textLine, QSize widgetSize);
     void setTextFont(QTextEdit* textEdit, TextFont textFont, QSize widgetSize);
     void textFont(QTextEdit* textEdit, TextFont &textFont, QSize widgetSize);
     void saveCurrentTextPos(TextLine textLine, QTextEdit *textEdit);
     void saveCurrentTextFont(TextFont textFont,QTextEdit *textEdit );
     void wrapText(QTextEdit *textEdit);
     void addLine(QTextEdit *textEdit);
     void removeLine(QTextEdit *textEdit);
     QTextEdit* createNewTextEdit();
     void showCustomContextMenu(const QPoint &pos);
    
private:

    // Test flag : text is updating from the database
    bool mIsSettingLines;
    // Save defaut subtitle container
    MySubtitles mDefaultSub;
    // Save current text edit zones position and font
    MySubtitles mCurrentTextProp;
    // Number of pixels per inch for the current hardware
    qint32 mPxPerInch;
    // Last text zone focused
    QTextEdit* mpLastFocused;
    // Copy of the last text wrote from the database
    QList<QString> mPreviousTextList;

    QList<QTextEdit*> mTextLinesList;
};

#endif // MYTEXTEDIT_H
