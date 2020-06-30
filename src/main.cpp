#include "videolabel.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VideoLabel w;
    w.show();

    return a.exec();
}
