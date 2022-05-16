#include "mainwindow.h"

#include <QApplication>

#include <QIcon>
#include <QPixmap>
#include <QDebug>

//#####################################################################################################################################################################################
int main(int argc, char *argv[])
{
    ///*
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowIcon(QIcon(QPixmap("CDC_Icon.png")));
    w.show();
    return a.exec();//*/

    /*
    RGBInstruction test;
    QString err;

    qDebug() << test.print() << "\n";

    if(!test.parse("C:ABCDEF,1100;", err))
    {
        qDebug() << err;
    }
    qDebug() << test.print() << "\n";

    if(!test.parse("F:ABCDEF,FEDCBA,2100;", err))
    {
        qDebug() << err;
    }
    qDebug() << test.print() << "\n";

    if(!test.parse("L:5,1100;", err))
    {
        qDebug() << err;
    }
    qDebug() << test.print() << "\n";

    if(!test.parse("CABCDEF,1100;", err))
    {
        qDebug() << err;
    }
    qDebug() << test.print() << "\n";

    if(!test.parse("F:ABDEF,FEDCBA,2100;", err))
    {
        qDebug() << err;
    }
    qDebug() << test.print() << "\n";

    if(!test.parse("L:d5,1100;", err))
    {
        qDebug() << err;
    }
    qDebug() << test.print() << "\n";
    //*/
}
