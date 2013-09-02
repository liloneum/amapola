#ifndef MYSUBTITLES_H
#define MYSUBTITLES_H

#include <QString>

class MySubtitles
{
public:
    MySubtitles();
    void setStartTime(QString startTime);
    void setEndTime(QString endTime);
    void setText(QString text);
    QString startTime();
    QString endTime();
    QString text();

private:
    QString mStartTime;
    QString mEndTime;
    QString mText;
};

#endif // MYSUBTITLES_H
