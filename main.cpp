#include "server.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server *myServer = new Server();
    myServer->startServer();
    return a.exec();
}
