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

// returns the channel corresponding to the name given or null if channel doesn't exist.
Channel* Server::getChannelFromName(QString& name)
{
    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if((*it)->getChannelName().compare(name) == 0)
            return (*it);
    }

    return NULL;
}

Client* Server::getClientFromName(QString& name)
{
    for(std::list<Client*>::iterator it = m_listClients.begin(); it != m_listClients.end(); ++it)
    {
        if((*it)->getNickname().compare(name) == 0)
            return (*it);
    }

    return NULL;
}



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

    Client* tmp;
    tmp = this->getClientFromName(nickname);

    if(tmp != NULL)
    {
        msg = "Nickname already used by another client !";
        c->setMsg(msg);
        return ERROR::eNickCollision;
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

    Client *dest_client = getClientFromName(dest);

    if(dest_client == NULL)
    {
        QString msg = "Specified client doesn't exist !";
        c->setMsg(msg);
        return ERROR::eNotExist;
    }

    else
    {
        QTcpSocket *sock = dest_client->getSocket();
        QByteArray response = Frame::getReadyToSendFrame(c->getNickname() + "\n" + message, 255, 129);
        sock->write(response);
        return ERROR::esuccess;
    }
}


quint8 Server::join(Client* c, QString& dest)
{

    Channel* dest_channel = getChannelFromName(dest);

    if(dest_channel == NULL)        // if the channel doesn't exist
    {
        dest_channel = new Channel(dest);       // it is created
        m_listChannels.push_front(dest_channel);
        dest_channel->addClient(c, OPERATOR);       // and the client is Operator on this channel
        return ERROR::esuccess;
    }

    else  // dest_channel already exist
    {
        if(dest_channel->isStatus(c, BANNED) == true)   // if the status of the client on this channel is BANNED
        {
            QString response("You're banned from this channel !");
            c->setMsg(response);
            return ERROR::eNotAuthorised;
        }

        else    // if the client is not banned fromthis channel
        {
            dest_channel->addClient(c, REGULAR);
            QString msg = "#" + dest + "\n" + c->getNickname();
            broadCast(msg, 255, 137, dest_channel, c);
            return ERROR::esuccess;
        }
    }
}



quint8 Server::pubmsg(Client* c, QString& dest, QString& message)
{
    Channel* dest_channel = getChannelFromName(dest);

    if(dest_channel == NULL)
        return ERROR::eNotExist;

    if(dest_channel->isStatus(c, BANNED) == true)       // if the client is banned from this channel
        return ERROR::eNotAuthorised;

    if(dest_channel->isStatus(c, REGULAR) == false)     // if the client isn't connected to the channel
        return ERROR::eNotAuthorised;

    QString msg = "#" + dest + "\n" + c->getNickname() + "\n" + message;
    broadCast(msg, 255, 128, dest_channel, c);
    return ERROR::esuccess;
}



quint8 Server::leave(Client* c, QString& dest)
{

    Channel* dest_channel = getChannelFromName(dest);

    if(dest_channel == NULL)
        return ERROR::eNotExist;

    if(dest_channel->isStatus(c, REGULAR) == false)        // client is not connected to channel
        return ERROR::eBadArg;                       // return bad argument
    else
    {
        dest_channel->removeClient(c);
        QString msg = "#" + dest + "\n" + c->getNickname();
        broadCast(msg, 255, 133, dest_channel, c);
        return ERROR::esuccess;
    }


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


quint8 Server::topic(Client* c, QString& dest, QString& topic)
{

    Channel* dest_channel = getChannelFromName(dest);

    if(dest_channel == NULL)
    {
        QString response("Specified channel doesn't exist !");
        c->setMsg(response);
        return ERROR::eNotExist;
    }

    if(!(dest_channel->isStatus(c, OPERATOR)))
        return ERROR::eNotAuthorised;

    dest_channel->setTopic(topic);
    QString msg = "#" + dest + "\n" + topic;
    broadCast(msg, 255, 131, dest_channel, c);
    return ERROR::esuccess;
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

quint8 Server::cwho(Client* c, QString& dest)
{
    QString response("");
    Channel* dest_channel = getChannelFromName(dest);

    if(dest_channel == NULL)
    {
        response = "Channel doesn't exist !";
        c->setMsg(response);
        return ERROR::eNotExist;
    }

    if(dest_channel->isStatus(c, BANNED) == true)
    {
        response = "You're banned from this channel !";
        c->setMsg(response);
        return ERROR::eNotAuthorised;
    }

    for (std::list<Client*>::iterator _it = dest_channel->getClientList(REGULAR).begin(); _it != dest_channel->getClientList(REGULAR).end(); ++_it)
    {
        if((*_it)->getNickname() != c->getNickname())   // remove this line if you want to display your nick if you're in the channel
            response += (*_it)->getNickname() + "\n";
    }

    response = response.trimmed();       // the last client doesn't need a separator, indeed he's the last one.

    c->setMsg(response);
    return ERROR::esuccess;

}


quint8 Server::kick(Client* c, QString& dest_channel, QString& dest_client)
{
    QString response("");

    Channel* chan = getChannelFromName(dest_channel);
    if(chan == NULL)
    {
        return ERROR::eNotExist;
    }

    Client* cli = getClientFromName(dest_client);
    if(cli == NULL)
    {
        return ERROR::eNotExist;
    }

    if (chan->isStatus(c, OPERATOR) != true)
    {
        return ERROR::eNotAuthorised;
    }

    if(!chan->isStatus(cli, REGULAR))
    {
        response = "Target client is not on this channel.";
        c->setMsg(response);
        return ERROR::eNotExist;
    }

    if(chan->isStatus(cli, OPERATOR))
    {
        return ERROR::eNotAuthorised;
    }

    chan->removeClient(cli);

    response = "#" + dest_channel + "\n" + dest_client + "\n" + c->getNickname();
    broadCast(response, 255, 134, chan, c);

    return ERROR::esuccess;


}

quint8 Server::ban(Client* c, QString& dest_channel, QString& dest_client)
{
    for(std::list<Channel*>::iterator channelIterator = m_listChannels.begin(); channelIterator != m_listChannels.end(); ++channelIterator)
    {
        if((*channelIterator)->getChannelName().compare(dest_channel) == 0) //Channel found
        {
            if((*channelIterator)->isStatus(c, OPERATOR) != true)   // if the sender is not operator, he's not authorized to ban
                return ERROR::eNotAuthorised;

            for (std::list<Client*>::iterator clientIterator = (*channelIterator)->getClientList(REGULAR).begin(); clientIterator != (*channelIterator)->getClientList(REGULAR).end(); ++clientIterator)
            {
                if((*clientIterator)->getNickname().compare(dest_client) == 0 ) //Target client found
                {
                    if((*channelIterator)->isStatus((*clientIterator), OPERATOR))   // if the client is OPERATOR, return ERROR::eNotAuthorised
                        return ERROR::eNotAuthorised;
                    else{
                        (*channelIterator)->getClientList(BANNED).push_front(*clientIterator);
                        (*channelIterator)->removeClient(*clientIterator);
                        return ERROR::esuccess;
                    }
                }
            }


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
