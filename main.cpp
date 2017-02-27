#include "ict_ur10.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ICT_UR10 w;
    w.show();

    return a.exec();
}
