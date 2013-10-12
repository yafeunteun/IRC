#include <QApplication>
#include "server.h"
#include "bdPlatformLog.h"


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    Server* s1 = Server::Instance();
    Server* s2 = Server::Instance();
    Server* s3 = Server::Instance();
    Server* s4 = Server::Instance();

    return app.exec();
}
