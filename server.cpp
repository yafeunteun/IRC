#include "server.h"
#include "bdPlatformLog.h"
#include <iostream>
//#include <QRegularExpression>

/* remove the following includes when the project is closed (they are here for debugging) */
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

void Server::broadCast(Channel *chan, Client *sender, QString& message)
{
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Broadcasting message (%s) on #%s.", message.toStdString().c_str(), chan->getChannelName().toStdString().c_str());

    for(std::list<Client*>::iterator it = chan->getClientList().begin(); it != chan->getClientList().end(); ++it)
    {
        if ((*it)->getNickname().compare(sender->getNickname()) != 0)
            (*it)->getSocket()->write(message.toStdString().c_str(), message.length());     // we should format it as TTIICAAAAAAAAAA later
    }
}


/*********************************************
 *    IMPLEMENTATION OF THE COMMANDS         *
 *********************************************/
quint8 Server::nick(Client* c, QString& nickname)
{
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Server has received request to change nickname !!! ");

    /*QRegularExpression reg;
    reg.setPattern("[a-z]\\S{2,8}");
    reg.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    if(!reg.isValid())
        bdPlatformLog::bdLogMessage(_WARNING, "warn/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Pattern for regex is invalid !");
    if(!(reg.match(nickname).hasMatch())){
        bdPlatformLog::bdLogMessage(_WARNING, "warn/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Invalid nickname !");

        return 3;        // Invalid nickname !
    }*/

    for(std::list<Client*>::iterator it = m_listClients.begin(); it != m_listClients.end(); ++it)
    {
        if((*it)->getNickname().compare(nickname) == 0)
        {
            bdPlatformLog::bdLogMessage(_WARNING, "warn/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Nickname already used !");
            return 2;
        }
    }

    c->setNickname(nickname);
    return 0;
}

quint8 Server::privateMessage(Client* c, QString& dest, QString& message)
{
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "%s is sending a private message to %s.", c->getNickname().toStdString().c_str(), dest.toStdString().c_str());

    for(std::list<Client*>::iterator it = m_listClients.begin(); it != m_listClients.end(); ++it)
    {
        if((*it)->getNickname().compare(dest) == 0)
        {
            QTcpSocket *sock = (*it)->getSocket();
            sock->write(message.toStdString().c_str(), message.length());    // we should format it as TTIICAAAAAAAAAA later
            return 0;
        }
    }

    bdPlatformLog::bdLogMessage(_WARNING, "warn/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "The user '%s' does not exist.", dest.toStdString().c_str());
    return 2;
}

quint8 Server::channelMessage(Client* c, QString& dest, QString& message)
{
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "%s is sending a message to #%s.", c->getNickname().toStdString().c_str(), dest.toStdString().c_str());

    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if((*it)->getChannelName().compare(dest) == 0)
        {
            broadCast(*it, c, message);
            return 0;
        }
    }

    bdPlatformLog::bdLogMessage(_WARNING, "warn/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "The channel '#%s' does not exist.", dest.toStdString().c_str());
    return 2;
}


quint8 Server::joinChannel(Client* c, QString& dest)
{
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "%s is joining #%s.", c->getNickname().toStdString().c_str(), dest.toStdString().c_str());

    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if((*it)->getChannelName().compare(dest) == 0)
        {
            (*it)->addClient(c);
            return 0;
        }
    }

    bdPlatformLog::bdLogMessage(_WARNING, "warn/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "The channel '#%s' does not exist, creating it.", dest.toStdString().c_str());
    Channel *chan = new Channel(dest);
    m_listChannels.push_front(chan);

    //TODO: The new client MUST be OPed

    return 1;
}

quint8 Server::leaveChannel(Client* c, QString& dest)
{
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "%s is leaving #%s.", c->getNickname().toStdString().c_str(), dest.toStdString().c_str());

    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if((*it)->getChannelName().compare(dest) == 0)
        {
            std::list<Client*>::iterator _it = (*it)->getClientList().begin();
            for (; _it != (*it)->getClientList().end(); ++_it)
            {
                if (c->getNickname().compare((*_it)->getNickname()))
                    break;
            }
            (*it)->getClientList().remove(*_it);
            return 0;
        }
    }

    bdPlatformLog::bdLogMessage(_WARNING, "warn/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "The channel '#%s' does not exist, hax?!", dest.toStdString().c_str());

    //Remarks: If the client who leaves is the current OP, rights should not be transfered to another Client

    return 1;
}

quint8 Server::listChannel(Client* c, QString& filter)
{
    QString message("");
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "%s wants the channel list containing word '%s'.", c->getNickname().toStdString().c_str(), filter.toStdString().c_str());

    if (filter.compare("*") == 0)
    {
        for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
        {
            message += "#" + (*it)->getChannelName() + " " + (*it)->getTopic() + "\n";
        }
        c->getSocket()->write(message.toStdString().c_str(), message.length());
    }
    else
    {
        for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
        {
            if ((*it)->getChannelName().contains(filter, Qt::CaseSensitive) || (*it)->getTopic().contains(filter, Qt::CaseSensitive))
                message += "#" + (*it)->getChannelName() + " " + (*it)->getTopic() + "\n";
        }
        c->getSocket()->write(message.toStdString().c_str(), message.length());
    }

    return 0;
}
