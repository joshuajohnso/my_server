#include "server.h"

#include <QApplication>
//MSVCÀ©Õ¹
#ifdef _MSC_VER
#pragma comment(lib, "user32.lib")
#endif


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    server w;
    w.show();
    return a.exec();
}