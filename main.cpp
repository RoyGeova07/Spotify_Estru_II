#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include"menuinicio.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MenuInicio w;
    w.show();
    return a.exec();
}
