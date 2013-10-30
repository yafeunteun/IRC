#include "server.h"
#include "bdPlatformLog.h"
#include <iostream>
#include <QRegularExpression>

/* remove the following includes when the project is closed (they are here for debugging) */
// strings and c-strings
#include <iostream>
#include <cstring>
#include <string>
#include "command.h"


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

    QObject::connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Sole instance of server has been created successfully !");

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

// if chan is not specified, message will be sent to all clients connected to the server
// if a sender is specified, message will not be sent to him
void Server::broadCast(QString& message, quint16 id, quint8 code, Channel* chan, Client* sender)
{
    QByteArray response = Frame::getReadyToSendFrame(message, id, code);

    if(chan == NULL)        // Broadcasting to all clients
    {
        for(std::list<Client*>::iterator it = m_listClients.begin(); it != m_listClients.end(); ++it)
        {
            QTcpSocket *sock = (*it)->getSocket();
            sock->write(response);
        }

        return;
    }


    for(std::list<Client*>::iterator it = chan->getClientList(REGULAR).begin(); it != chan->getClientList(REGULAR).end(); ++it)
    {
        if(sender != NULL)  // if sender != NULL sender will not received the message
        {
            if ((*it)->getNickname().compare(sender->getNickname()) != 0)
            {
                QTcpSocket *sock = (*it)->getSocket();
                sock->write(response);
            }
        }

        else
        {
            QTcpSocket *sock = (*it)->getSocket();
            sock->write(response);
        }
    }
}


/*********************************************
 *    IMPLEMENTATION OF THE COMMANDS         *
 *********************************************/
quint8 Server::nick(Client* c, QString& nickname)
{
    for(std::list<Client*>::iterator it = m_listClients.begin(); it != m_listClients.end(); ++it)
    {
        if((*it)->getNickname().compare(nickname) == 0)
        {
            return ERROR::eNickCollision;
        }
    }
    QString msg = c->getNickname() + "\n" + nickname;
    c->setNickname(nickname);
    QTcpSocket *sock = c->getSocket();
    QByteArray response = Frame::getReadyToSendFrame(msg, 255, 132);
    sock->write(response);
    return ERROR::esuccess;
}

quint8 Server::privmsg(Client* c, QString& dest, QString& message)
{
    for(std::list<Client*>::iterator it = m_listClients.begin(); it != m_listClients.end(); ++it)
    {
        if((*it)->getNickname().compare(dest) == 0)
        {
            QTcpSocket *sock = (*it)->getSocket();
            QByteArray response = Frame::getReadyToSendFrame(c->getNickname() + "\n" + message, 255, 129);
            sock->write(response);
            return ERROR::esuccess;
        }
    }
    return ERROR::eNotExist;
}


quint8 Server::join(Client* c, QString& dest)
{

    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if((*it)->getChannelName().compare(dest) == 0)
        {
            if((*it)->isStatus(c, BANNED) == true)
                return ERROR::eNotAuthorised;

            (*it)->addClient(c, REGULAR);
            QString msg = dest + "\n" + c->getNickname();
            broadCast(msg, 255, 137, *it, c);
            return ERROR::esuccess;
        }
    }

    Channel *chan = new Channel(dest);
    m_listChannels.push_front(chan);

    chan->addClient(c, OPERATOR);

    return ERROR::esuccess;
}



quint8 Server::pubmsg(Client* c, QString& dest, QString& message)
{

    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if((*it)->getChannelName().compare(dest) == 0)
        {
            if((*it)->isStatus(c, BANNED) == true)
                return ERROR::eNotAuthorised;

            if((*it)->isStatus(c, REGULAR) == false)
                return ERROR::eNotAuthorised;

            QString msg = dest + "\n" + c->getNickname() + "\n" + message;
            broadCast(msg, 255, 128, *it, c);
            return ERROR::esuccess;
        }
    }

    return ERROR::eNotExist;
}



quint8 Server::leave(Client* c, QString& dest)
{

    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if((*it)->getChannelName().compare(dest) == 0)      // channel exists
        {

            if((*it)->isStatus(c, REGULAR) == false)        // client is not connected to channel
                return ERROR::eBadArg;                       // return bad argument
            else
            {
                (*it)->removeClient(c);
                QString msg = dest + "\n" + c->getNickname();
                broadCast(msg, 255, 133, *it, c);
                return ERROR::esuccess;
            }

        }

    }

    return ERROR::eNotExist;
}

quint8 Server::listChannel(Client* c, QString& filter)
{
    QString message("");
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "server", __FILE__, __PRETTY_FUNCTION__, __LINE__, "%s wants the channel list containing word '%s'.", c->getNickname().toStdString().c_str(), filter.toStdString().c_str());

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


quint8 Server::topic(Client* c, QString& dest_channel, QString& topic)
{

    for (std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if ((*it)->getChannelName().compare(dest_channel) == 0)//The specified channel exists
        {
            if(!((*it)->isStatus(c, OPERATOR)))
                return ERROR::eNotAuthorised;

            (*it)->setTopic(topic);
            QString msg = dest_channel + "\n" + topic;
            broadCast(msg, 255, 131, *it, c);
            return ERROR::esuccess;
        }
    }

    return ERROR::eNotExist;
}

quint8 Server::gwho(Client* c, QString& filter)
{
    QString response("");
    QRegularExpression reg;
    reg.setPattern(filter);

    for (std::list<Client*>::iterator it = m_listClients.begin(); it != m_listClients.end(); ++it)
    {
        if(reg.match((*it)->getNickname()).hasMatch() && (*it)->getNickname() != c->getNickname() )
            response += (*it)->getNickname() + "\n";
    }

    response = response.trimmed();       // the last client doesn't need a separator, indeed he's the last one.

    QTcpSocket *sock = c->getSocket();
    // 129 is used but we should ask M.De... what he expects, the code isn't precised in the subject...
    QByteArray ret_frame = Frame::getReadyToSendFrame(c->getNickname() + "\n" + response, 255, 129);
    sock->write(ret_frame);
    return ERROR::esuccess;
}

quint8 Server::cwho(Client* c, QString& dest_channel)
{
    QString response("");

    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if((*it)->getChannelName().compare(dest_channel) == 0)
        {
            if((*it)->isStatus(c, BANNED) == true)
                return ERROR::eNotAuthorised;

            for (std::list<Client*>::iterator _it = (*it)->getClientList().begin(); _it != (*it)->getClientList().end(); ++_it)
            {
                if((*_it)->getNickname() != c->getNickname())
                    response += (*_it)->getNickname() + "\n";
            }

            response = response.trimmed();       // the last client doesn't need a separator, indeed he's the last one.
            QTcpSocket *sock = c->getSocket();
            // 129 is used but we should ask M.De... what he expects, the code isn't precised in the subject...
            QByteArray ret_frame = Frame::getReadyToSendFrame(c->getNickname() + "\n" + response, 255, 129);
            sock->write(ret_frame);
            return ERROR::esuccess;
        }
    }

    return ERROR::eNotExist;
}


