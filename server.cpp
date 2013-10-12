#include "server.h"
#include "bdPlatformLog.h"

Server* Server::_instance = 0;

Server* Server::Instance() {
    if (_instance == 0) {
        _instance = new Server;
    }

    return _instance;
}


Server::Server() : QTcpServer()
{
    qint16 port = 3074;
    int numConnections = 50;
    QHostAddress address("127.0.0.1");

    this->listen(address, port);
    this->setMaxPendingConnections(numConnections);

    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Sole instance of server has been created successfully !");

}


