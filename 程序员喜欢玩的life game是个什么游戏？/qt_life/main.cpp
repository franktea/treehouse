#include "lifecells.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LifeCells w;
    w.show();
    return a.exec();
}
