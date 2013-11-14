#include "server.h"
#include "bdPlatformLog.h"
#include <iostream>
//#include <QRegularExpression>
#include "unixregexp.h"

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
 *    METHODS       *
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
 *                  NICK                     *
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

    c->setState(1); // this line means the client has a nickname now and is able to send commands (other than nick)
    return ERROR::esuccess;
}



/*********************************************
 *                  PRIVMSG                  *
 *********************************************/
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

/*********************************************
 *                  JOIN                     *
 *********************************************/
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



/*********************************************
 *                  PUBMSG                   *
 *********************************************/
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


/*********************************************
 *                  LEAVE                    *
 *********************************************/
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


/*********************************************
 *                  LIST                     *
 *********************************************/
quint8 Server::list(Client* c, QString& filter)
{
    QString response("");
    UnixRegExp reg(filter);

    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if(reg.exactMatch((*it)->getChannelName()))
        {
            response += "#" + (*it)->getChannelName() + " " + (*it)->getTopic() + "\n";
        }
    }

    response = response.trimmed();       // the last channelNmae doesn't need a separator, indeed it's the last one.
    c->setMsg(response);

    return ERROR::esuccess;
}



/*********************************************
 *                  TOPIC                    *
 *********************************************/
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


/*********************************************
 *                  GWHO                     *
 *********************************************/
quint8 Server::gwho(Client* c, QString& filter)
{
    QString response("");
    UnixRegExp reg(filter);

    for (std::list<Client*>::iterator it = m_listClients.begin(); it != m_listClients.end(); ++it)
    {
        if(reg.exactMatch((*it)->getNickname()))
            response += (*it)->getNickname() + "\n";
    }

    response = response.trimmed();       // the last client doesn't need a separator, indeed he's the last one.

    c->setMsg(response);
    return ERROR::esuccess;
}


/*********************************************
 *                  CWHO                     *
 *********************************************/
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
        response += (*_it)->getNickname() + "\n";
    }

    response = response.trimmed();       // the last client doesn't need a separator, indeed he's the last one.

    c->setMsg(response);
    return ERROR::esuccess;

}


/*********************************************
 *                  KICK                     *
 *********************************************/
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



/*********************************************
 *                  BAN                      *
 *********************************************/
quint8 Server::ban(Client* c, QString& dest_channel, QString& dest_client)
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

    if(chan->isStatus(cli, OPERATOR))
    {
        return ERROR::eNotAuthorised;
    }

    chan->removeClient(cli);
    chan->addClient(cli, BANNED);

    response = "#" + dest_channel + "\n" + "+" + "\n" + dest_client;
    broadCast(response, 255, 135, chan, c);

    return ERROR::esuccess;
}


/*********************************************
 *                  UNBAN                    *
 *********************************************/
quint8 Server::unban(Client* c, QString& dest_channel, QString& dest_client)
{
    QString response("");

    Channel* chan = getChannelFromName(dest_channel);
    if(chan == NULL)
    {
        return ERROR::eNotExist;
    }

    Client* cli = getClientFromName(dest_client);
    if(cli == NULL || !(chan->isStatus(cli, BANNED)))
    {
        return ERROR::eNotExist;
    }

    if (chan->isStatus(c, OPERATOR) != true)
    {
        return ERROR::eNotAuthorised;
    }

    chan->unbanClient(cli);

    response = "#" + dest_channel + "\n" + "-" + "\n" + dest_client;
    broadCast(response, 255, 135, chan, c);

    return ERROR::esuccess;
}


/*********************************************
 *                  BANLIST                  *
 *********************************************/
quint8 Server::banlist(Client* c, QString& dest_channel)
{
    {
        QString response("");
        Channel* chan = getChannelFromName(dest_channel);

        if(chan == NULL)
        {
            response = "Channel doesn't exist !";
            c->setMsg(response);
            return ERROR::eNotExist;
        }

        for (std::list<Client*>::iterator _it = chan->getClientList(BANNED).begin(); _it != chan->getClientList(BANNED).end(); ++_it)
        {
            response += (*_it)->getNickname() + "\n";
        }

        response = response.trimmed();       // the last client doesn't need a separator, indeed he's the last one.

        c->setMsg(response);
        return ERROR::esuccess;

    }
}


/*********************************************
 *                  OP                       *
 *********************************************/
quint8 Server::op(Client* c, QString& dest_channel, QString& dest_client)
{
    QString response("");

    Channel* chan = getChannelFromName(dest_channel);
    if(chan == NULL)
    {
        response = "Unknown channel";
        c->setMsg(response);
        return ERROR::eNotExist;
    }

    Client* cli = getClientFromName(dest_client);
    if(cli == NULL)
    {
        response = "Unknown client";
        c->setMsg(response);
        return ERROR::eNotExist;
    }

    if (chan->isStatus(c, OPERATOR) != true)
    {
        return ERROR::eNotAuthorised;
    }

    if(!chan->isStatus(cli))
    {
        response = "Client " + dest_client + " is not on channel " + dest_channel + "." ;
        c->setMsg(response);
        return ERROR::eNotExist;
    }

    chan->setOperator(cli);
    response = c->getNickname() + "\n" + chan->getChannelName() + "\n" + "op";
    broadCast(response, 255, 130, chan, c);

    return ERROR::esuccess;
}


/*********************************************
 *                  DEOP                     *
 *********************************************/
quint8 Server::deop(Client* c, QString& dest_channel, QString& dest_client)
{
    QString response("");

    Channel* chan = getChannelFromName(dest_channel);
    if(chan == NULL)
    {
        response = "Unknown channel";
        c->setMsg(response);
        return ERROR::eNotExist;
    }

    Client* cli = getClientFromName(dest_client);
    if(cli == NULL)
    {
        response = "Unknown client";
        c->setMsg(response);
        return ERROR::eNotExist;
    }

    if (chan->isStatus(c, OPERATOR) != true)
    {
        return ERROR::eNotAuthorised;
    }


    if(!chan->isStatus(cli))
    {
        response = "Client " + dest_client + " is not on channel " + dest_channel + "." ;
        c->setMsg(response);
        return ERROR::eNotExist;
    }

    if(!(chan->isStatus(cli, OPERATOR)))
    {
        response = "Client " + dest_client + " is not operator on channel " + dest_channel + ".";
        c->setMsg(response);
        return ERROR::error;
    }

    chan->unsetOperator(cli);
    response = c->getNickname() + "\n" + chan->getChannelName() + "\n" + "deop";
    broadCast(response, 255, 130, chan, c);

    return ERROR::esuccess;
}
