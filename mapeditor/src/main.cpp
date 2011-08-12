#include <QtGui/QApplication>
#include <QLocale>
#include <QTranslator>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString qtFilename = QString("qt_%1.qm").arg(QLocale::system().name());
    QString appFilename = QString("tankme_%1.qm").arg(QLocale::system().name());
    QTranslator qtTranslator;
    QTranslator appTranslator;
    qtTranslator.load(qtFilename.toLower(), qApp->applicationDirPath() + QString("/translations"));
    appTranslator.load(appFilename.toLower(), qApp->applicationDirPath() + QString("/translations"));
    a.installTranslator(&qtTranslator);
    a.installTranslator(&appTranslator);

    MainWindow w;
    w.show();

    return a.exec();
}
