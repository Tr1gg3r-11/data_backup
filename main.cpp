#include "main.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    controller c;
    c.show();
    return a.exec();
}