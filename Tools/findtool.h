#ifndef FINDTOOL_H
#define FINDTOOL_H

#include <QWidget>
#include "mainwindow.h"

namespace Ui {
class FindTool;
}

class FindTool : public QWidget
{
    Q_OBJECT

public:
    explicit FindTool(QWidget *parent = 0);
    ~FindTool();

private:
    Ui::FindTool *ui;
    QList<quint16> mMatchingSubsList;
    QRegExp mFindRegExp;

private slots:
    void on_findLineEdit_textChanged(QString searchedText);
    void on_findCaseCheckBox_clicked();
    void on_findWholeWordCheckBox_clicked();
    void on_findPrevArrow_clicked();
    void on_findNextArrow_clicked();
    void on_replacePushButton_clicked();
    void on_replaceAllPushButton_clicked();
    void replaceText(QList<MySubtitles>& subList, quint16 sub_number);
};

#endif // FINDTOOL_H
