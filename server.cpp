#include "server.h"
#include "bdPlatformLog.h"
#include <iostream>

/* remove the followinf includes when the project is close (they are here for debugging) */
// strings and c-strings
#include <iostream>
#include <cstring>
#include <string>

/********************
 *  STATIC MEMBERS  *
 ********************/
Server* Server::_instance = 0;

Server* Server::Instance() {
    if (_instance == 0) {
        _instance = new Server();
    }

    return _instance;
}

/********************
 *    CONSTRUCTOR   *
 ********************/
Server::Server(QObject* parent) : QObject(parent)
{
    qint16 port = 3074;
    int numConnections = 50;
    m_tcpServer = new QTcpServer(this);

    m_tcpServer->listen(QHostAddress::Any, port);
    m_tcpServer->setMaxPendingConnections(numConnections);

    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Sole instance of server has been created successfully !");

    QObject::connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

/********************
 *    DESTRUCTOR    *
 ********************/
Server::~Server()
{
}

/********************
 *    SLOTS         *
 ********************/
void Server::onNewConnection(void)
{
     QTcpSocket* socket = m_tcpServer->nextPendingConnection();

     Client* c = new Client(socket, this);
     m_listClients.push_back(c);

     bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "new client [%d] added successfully !", c->getSocket()->socketDescriptor());
}


/********************
 *    MEMBER         *
 ********************/
void Server::delClient(Client* c)
{
    m_listClients.remove(c);
}

void Server::readData(Client *c)
{
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Client [%d] has sent data !", c->getSocket()->socketDescriptor());

    QString line = QString::fromUtf8(c->getSocket()->readLine()).trimmed();

    if(line.at(0) == '/'){
        bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Client [%d] has sent a command !", c->getSocket()->socketDescriptor());
        QString command = line.mid(1, 4);
        if(command!=NULL)
        {
           // debug section erase the following 3 lines later
                char * sstr = new char [command.toStdString().length()+1];
                std::strcpy (sstr, command.toStdString().c_str());
            bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Client [%d] has sent command %s ", c->getSocket()->socketDescriptor(), sstr);
        }

    }

    // debug section erase the following 3 lines later
    char * cstr = new char [line.toStdString().length()+1];
    std::strcpy (cstr, line.toStdString().c_str());
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Client [%d] has sent : %s", c->getSocket()->socketDescriptor(), cstr);

}


