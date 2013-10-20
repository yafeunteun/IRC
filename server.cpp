#include "server.h"
#include "bdPlatformLog.h"
#include <iostream>
#include <QRegularExpression>

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




/*********************************************
 *    IMPLEMENTATION OF THE COMMANDS         *
 *********************************************/
quint8 Server::nick(Client* c, QString& nickname)
{
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Server has received request to change nickname !!! ");

    QRegularExpression reg;
    reg.setPattern("toto");
    reg.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    if(!reg.isValid())
        bdPlatformLog::bdLogMessage(_WARNING, "warn/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Pattern for regex is invalid !");
    if(!(reg.match(nickname).hasMatch())){
        bdPlatformLog::bdLogMessage(_WARNING, "warn/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Invalid nickname !");

        return 3;        // Invalid nickname !
    }

    for(std::list<Client*>::iterator it = m_listClients.begin(); it!=m_listClients.end(); ++it)
    {
        if((*it)->getNickname() == nickname){
            bdPlatformLog::bdLogMessage(_WARNING, "warn/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Nickname already used !");
            return 2;
        }
    }

    c->setNickname(nickname);
    return 0;
}



