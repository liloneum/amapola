#include "mytextedit.h"
#include <QKeyEvent>

MyTextEdit::MyTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
    // Add an event filter on the subtitles textedit object
    this->installEventFilter(this);

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateEditor()));
}

bool MyTextEdit::eventFilter(QObject* watched, QEvent* event) {

    if ( watched == this ) {
        if ( event->type() == QEvent::KeyPress ) {

            QKeyEvent* key_event = static_cast<QKeyEvent*>(event);

            if ( ( key_event->matches(QKeySequence::InsertParagraphSeparator) ) &&
                 ( this->document()->blockCount() == 2 ) ) {
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}


void MyTextEdit::updateEditor() {
     this->setAlignment(Qt::AlignHCenter);
}
