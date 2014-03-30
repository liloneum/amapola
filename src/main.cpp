#include <QTranslator>

#include "mainwindow.h"
#include "application.h"

int main(int argc, char *argv[])
{
    MyApplication app(argc, argv);

    // Load translations (i18n) from system locale
    QString locale = QLocale::system().name();
    QTranslator translator;
    if (translator.load(locale, TRANSLATIONS_PATH)) {
        app.installTranslator(&translator);
    }

    return app.exec();
}
