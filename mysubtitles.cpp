#include "mysubtitles.h"

MySubtitles::MySubtitles()
{
    mStartTime = "";
    mEndTime = "";
    mText = "";
}

void MySubtitles::setStartTime(QString startTime) {

    mStartTime = startTime;
}

void MySubtitles::setEndTime(QString endTime) {

    mEndTime = endTime;
}

void MySubtitles::setText(QString text) {

    mText = text;
}

QString MySubtitles::startTime() {

    return mStartTime;
}

QString MySubtitles::endTime() {

    return mEndTime;
}

QString MySubtitles::text() {

    return mText;
}
