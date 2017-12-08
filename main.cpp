#include "mainwindow.h"
#include <QApplication>
#include <QDateTime>
#include <stdio.h>
#include <stdlib.h>

void myMessageOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "%s [D] %s\n", QDateTime::currentDateTime().toString().toStdString().c_str(), msg);
        break;
    case QtWarningMsg:
        fprintf(stderr, "%s [W] %s\n", QDateTime::currentDateTime().toString().toStdString().c_str(), msg);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "%s [E] %s\n", QDateTime::currentDateTime().toString().toStdString().c_str(), msg);
        break;
    case QtFatalMsg:
        fprintf(stderr, "%s [E] %s\n", QDateTime::currentDateTime().toString().toStdString().c_str(), msg);
        abort();
    }
}

int main(int argc, char **argv)
{
    qInstallMsgHandler(myMessageOutput);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
