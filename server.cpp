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

    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "server", __FILE__, __FUNCTION__, __LINE__, "Sole instance of server has been created successfully !");

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

     bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "server", __FILE__, __FUNCTION__, __LINE__, "new client [%d] added successfully !", c->getSocket()->socketDescriptor());
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
    QString msg;

    for(std::list<Client*>::iterator it = m_listClients.begin(); it != m_listClients.end(); ++it)
    {
        if((*it)->getNickname().compare(nickname) == 0)
        {
            msg = "Nickname already used by another client !";
            c->setMsg(msg);
            return ERROR::eNickCollision;
        }
    }

    msg = c->getNickname() + "\n" + nickname;
    c->setNickname(nickname);


    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it!= m_listChannels.end(); ++it)
    {
        if((*it)->isStatus(c, REGULAR))         // if the client is connected to the channel, we send a anick message to the channel.
            broadCast(msg, 255, 132, (*it),c);
    }

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
    QString msg = "Specified client doesn't exist !";
    c->setMsg(msg);
    return ERROR::eNotExist;
}


quint8 Server::join(Client* c, QString& dest)
{

    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if((*it)->getChannelName().compare(dest) == 0)
        {
            if((*it)->isStatus(c, BANNED) == true)
            {
                QString response("You're banned from this channel !");
                c->setMsg(response);
                return ERROR::eNotAuthorised;
            }

            (*it)->addClient(c, REGULAR);
            QString msg = "#" + dest + "\n" + c->getNickname();
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

            QString msg = "#" + dest + "\n" + c->getNickname() + "\n" + message;
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
                QString msg = "#" + dest + "\n" + c->getNickname();
                broadCast(msg, 255, 133, *it, c);
                return ERROR::esuccess;
            }

        }

    }

    return ERROR::eNotExist;
}

quint8 Server::list(Client* c, QString& filter)
{
    QString response("");
    QRegularExpression reg;
    reg.setPattern(filter);

    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if(reg.match((*it)->getChannelName()).hasMatch())
        {
            response += "#" + (*it)->getChannelName() + " " + (*it)->getTopic() + "\n";
        }
    }

    response = response.trimmed();       // the last channelNmae doesn't need a separator, indeed it's the last one.
    c->setMsg(response);

    return ERROR::esuccess;
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
            QString msg = "#" + dest_channel + "\n" + topic;
            broadCast(msg, 255, 131, *it, c);
            return ERROR::esuccess;
        }
    }
    QString response("Specified channel doesn't exist !");

    c->setMsg(response);
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

    c->setMsg(response);
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
            {
                response = "You're banned from this channel !";
                c->setMsg(response);
                return ERROR::eNotAuthorised;
            }

            for (std::list<Client*>::iterator _it = (*it)->getClientList(OPERATOR).begin(); _it != (*it)->getClientList(OPERATOR).end(); ++_it)
            {
                if((*_it)->getNickname() != c->getNickname())
                    response += (*_it)->getNickname() + "\n";
            }

            for (std::list<Client*>::iterator _it = (*it)->getClientList(REGULAR).begin(); _it != (*it)->getClientList(REGULAR).end(); ++_it)
            {
                if((*_it)->getNickname() != c->getNickname())
                    response += (*_it)->getNickname() + "\n";
            }

            response = response.trimmed();       // the last client doesn't need a separator, indeed he's the last one.

            c->setMsg(response);
            return ERROR::esuccess;
        }
    }

    response = "Channel doesn't exist !";
    c->setMsg(response);
    return ERROR::eNotExist;
}


quint8 Server::kick(Client* c, QString& dest_channel, QString& dest_client, QString& reason)
{
    bool destClientFound = false;

    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if((*it)->getChannelName().compare(dest_channel) == 0) //Channel found
        {
            if((*it)->isStatus(c, BANNED) == true)
                return ERROR::eNotAuthorised;

            if ((*it)->isStatus(c, OPERATOR) != true)
            {
                c->getSocket()->write(Frame::getReadyToSendFrame("You don't have enough permissions to invoke this command.", 255, 129));
                return ERROR::eNotAuthorised;
            }

            for (std::list<Client*>::iterator _it = (*it)->getClientList(REGULAR).begin(); _it != (*it)->getClientList(REGULAR).end(); ++_it)
            {
                if((*_it)->getNickname().compare(dest_client) == 0) //Target client found
                {
                    (*it)->getClientList(REGULAR).remove(*_it); //Not sure if it will work...
                    destClientFound = true;
                    break;
                }
            }

            if (!destClientFound)
            {
                bdPlatformLog::bdLogMessage(_WARNING, "warn/", "server", __FILE__, __FUNCTION__, __LINE__, "The target client was not found in the REGULAR list but he MIGHT be an operator."); //I assume that we can't be kicked between operators
                return ERROR::eNotExist;
            }

            QString response = dest_client + " has been kicked out #" + dest_channel + " ( " + reason + ")";
            // 129 is used but we should ask M.De... what he expects, the code isn't precised in the subject...
            QByteArray ret_frame = Frame::getReadyToSendFrame(response, 255, 129);
            //Send to the whole channel
            for (std::list<Client*>::iterator _it = (*it)->getClientList(REGULAR).begin(); _it != (*it)->getClientList(REGULAR).end(); ++_it)
                (*_it)->getSocket()->write(ret_frame);
            for (std::list<Client*>::iterator _it = (*it)->getClientList(OPERATOR).begin(); _it != (*it)->getClientList(OPERATOR).end(); ++_it)
                (*_it)->getSocket()->write(ret_frame);

            return ERROR::esuccess;
        }
    }
    return ERROR::eNotExist;
}

quint8 Server::ban(Client* c, QString& dest_channel, QString& dest_client, QString& reason)
{
    bool destClientFound = false;

    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if((*it)->getChannelName().compare(dest_channel) == 0) //Channel found
        {
            if((*it)->isStatus(c, BANNED) == true)
                return ERROR::eNotAuthorised;

            if ((*it)->isStatus(c, OPERATOR) != true)
            {
                c->getSocket()->write(Frame::getReadyToSendFrame("You don't have enough permissions to invoke this command.", 255, 129));
                return ERROR::eNotAuthorised;
            }

            for (std::list<Client*>::iterator _it = (*it)->getClientList(REGULAR).begin(); _it != (*it)->getClientList(REGULAR).end(); ++_it)
            {
                if((*_it)->getNickname().compare(dest_client) == 0) //Target client found
                {
                    (*it)->getClientList(BANNED).push_front(*_it);
                    (*it)->getClientList(REGULAR).remove(*_it); //Not sure if it will work...
                    destClientFound = true;
                    break;
                }
            }

            if (!destClientFound)
            {
                bdPlatformLog::bdLogMessage(_WARNING, "warn/", "server", __FILE__, __FUNCTION__, __LINE__, "The target client was not found in the REGULAR list but he MIGHT be an operator."); //I assume that we can't be banned between operators
                return ERROR::eNotExist;
            }

            QString response = dest_client + " has been banned from #" + dest_channel + " ( " + reason + ")";
            // 129 is used but we should ask M.De... what he expects, the code isn't precised in the subject...
            QByteArray ret_frame = Frame::getReadyToSendFrame(response, 255, 129);
            //Send to the whole channel
            for (std::list<Client*>::iterator _it = (*it)->getClientList(REGULAR).begin(); _it != (*it)->getClientList(REGULAR).end(); ++_it)
                (*_it)->getSocket()->write(ret_frame);
            for (std::list<Client*>::iterator _it = (*it)->getClientList(OPERATOR).begin(); _it != (*it)->getClientList(OPERATOR).end(); ++_it)
                (*_it)->getSocket()->write(ret_frame);

            return ERROR::esuccess;
        }
    }
    return ERROR::eNotExist;
}

quint8 Server::unban(Client* c, QString& dest_channel, QString& dest_client, QString& reason)
{
    bool destClientFound = false;

    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if((*it)->getChannelName().compare(dest_channel) == 0) //Channel found
        {
            if((*it)->isStatus(c, BANNED) == true)
                return ERROR::eNotAuthorised;

            if ((*it)->isStatus(c, OPERATOR) != true)
            {
                c->getSocket()->write(Frame::getReadyToSendFrame("You don't have enough permissions to invoke this command.", 255, 129));
                return ERROR::eNotAuthorised;
            }

            for (std::list<Client*>::iterator _it = (*it)->getClientList(BANNED).begin(); _it != (*it)->getClientList(BANNED).end(); ++_it)
            {
                if((*_it)->getNickname().compare(dest_client) == 0) //Target client found
                {
                    (*it)->getClientList(BANNED).remove(*_it);
                    destClientFound = true;
                    break;
                }
            }

            if (!destClientFound)
            {
                bdPlatformLog::bdLogMessage(_WARNING, "warn/", "server", __FILE__, __FUNCTION__, __LINE__, "The target client was not found.");
                return ERROR::eNotExist;
            }

            QString response = dest_client + " has been unbanned from #" + dest_channel + " ( " + reason + ")";
            // 129 is used but we should ask M.De... what he expects, the code isn't precised in the subject...
            QByteArray ret_frame = Frame::getReadyToSendFrame(response, 255, 129);
            c->getSocket()->write(ret_frame);

            return ERROR::esuccess;
        }
    }
    return ERROR::eNotExist;
}

quint8 Server::banlist(Client* c, QString& dest_channel)
{
    QString response("");

    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if((*it)->getChannelName().compare(dest_channel) == 0)
        {
            if((*it)->isStatus(c, BANNED) == true)
                return ERROR::eNotAuthorised;

            for (std::list<Client*>::iterator _it = (*it)->getClientList(BANNED).begin(); _it != (*it)->getClientList(BANNED).end(); ++_it)
                response += (*_it)->getNickname() + "\n";

            response = response.trimmed();       // the last client doesn't need a separator, indeed he's the last one.
            QTcpSocket *sock = c->getSocket();
            // 129 is used but we should ask M.De... what he expects, the code isn't precised in the subject...
            QByteArray ret_frame = Frame::getReadyToSendFrame(response, 255, 129);
            sock->write(ret_frame);
            return ERROR::esuccess;
        }
    }

    return ERROR::eNotExist;
}

quint8 Server::op(Client* c, QString& dest_channel, QString& dest_client)
{
    bool destClientFound = false;

    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if((*it)->getChannelName().compare(dest_channel) == 0) //Channel found
        {
            if((*it)->isStatus(c, BANNED) == true)
                return ERROR::eNotAuthorised;

            if ((*it)->isStatus(c, OPERATOR) != true)
            {
                c->getSocket()->write(Frame::getReadyToSendFrame("You don't have enough permissions to invoke this command.", 255, 129));
                return ERROR::eNotAuthorised;
            }

            for (std::list<Client*>::iterator _it = (*it)->getClientList(REGULAR).begin(); _it != (*it)->getClientList(REGULAR).end(); ++_it)
            {
                if((*_it)->getNickname().compare(dest_client) == 0) //Target client found
                {
                    if ((*it)->isStatus(*_it, OPERATOR) == true)
                    {
                        c->getSocket()->write(Frame::getReadyToSendFrame("The specified client is already an operator of this channel.", 255, 129));
                        return ERROR::esuccess;
                    }

                    (*it)->getClientList(OPERATOR).push_front(*_it);
                    destClientFound = true;
                    break;
                }
            }

            if (!destClientFound)
            {
                bdPlatformLog::bdLogMessage(_WARNING, "warn/", "server", __FILE__, __FUNCTION__, __LINE__, "The target client was not found.");
                return ERROR::eNotExist;
            }

            QString response = c->getNickname() + " gives operator status to " + dest_client + ".";
            // 129 is used but we should ask M.De... what he expects, the code isn't precised in the subject...
            QByteArray ret_frame = Frame::getReadyToSendFrame(response, 255, 129);
            //Send to the whole channel
            for (std::list<Client*>::iterator _it = (*it)->getClientList(REGULAR).begin(); _it != (*it)->getClientList(REGULAR).end(); ++_it)
                (*_it)->getSocket()->write(ret_frame);
            for (std::list<Client*>::iterator _it = (*it)->getClientList(OPERATOR).begin(); _it != (*it)->getClientList(OPERATOR).end(); ++_it)
                (*_it)->getSocket()->write(ret_frame);

            return ERROR::esuccess;
        }
    }
    return ERROR::eNotExist;
}

quint8 Server::deop(Client* c, QString& dest_channel, QString& dest_client)
{
    bool destClientFound = false;

    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if((*it)->getChannelName().compare(dest_channel) == 0) //Channel found
        {
            if((*it)->isStatus(c, BANNED) == true)
                return ERROR::eNotAuthorised;

            if ((*it)->isStatus(c, OPERATOR) != true)
            {
                c->getSocket()->write(Frame::getReadyToSendFrame("You don't have enough permissions to invoke this command.", 255, 129));
                return ERROR::eNotAuthorised;
            }

            for (std::list<Client*>::iterator _it = (*it)->getClientList(OPERATOR).begin(); _it != (*it)->getClientList(OPERATOR).end(); ++_it)
            {
                if((*_it)->getNickname().compare(dest_client) == 0) //Target client found
                {

                    (*it)->getClientList(OPERATOR).remove(*_it);
                    destClientFound = true;
                    break;
                }
            }

            if (!destClientFound)
            {
                bdPlatformLog::bdLogMessage(_WARNING, "warn/", "server", __FILE__, __FUNCTION__, __LINE__, "The target client was not found.");
                return ERROR::eNotExist;
            }

            QString response = c->getNickname() + " removed operator status to " + dest_client + ".";
            // 129 is used but we should ask M.De... what he expects, the code isn't precised in the subject...
            QByteArray ret_frame = Frame::getReadyToSendFrame(response, 255, 129);
            //Send to the whole channel
            for (std::list<Client*>::iterator _it = (*it)->getClientList(REGULAR).begin(); _it != (*it)->getClientList(REGULAR).end(); ++_it)
                (*_it)->getSocket()->write(ret_frame);
            for (std::list<Client*>::iterator _it = (*it)->getClientList(OPERATOR).begin(); _it != (*it)->getClientList(OPERATOR).end(); ++_it)
                (*_it)->getSocket()->write(ret_frame);

            return ERROR::esuccess;
        }
    }
    return ERROR::eNotExist;
}
