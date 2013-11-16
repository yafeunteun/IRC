#include <iostream>
#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include "command.h"
#include "server.h"


/*********************
 *    SINGLETON      *
 *********************/
Server* Server::_instance = 0;

Server* Server::Instance() {
    if (_instance == 0) {
        _instance = new Server();
    }

    return _instance;
}

/********************
 *    init          *
 ********************/

/*!
 *  \brief Initialize the server from the server.conf file.
 *
 *  The file server.conf must be placed in the folder of the executable program.
 */
void Server::init(void)
{
    QFile conf("./server.conf");
    if( conf.open(QFile::ReadOnly) )
    {
        QTextStream in(&conf);

        QStringList args;

        while(!in.atEnd())
        {
            QString data;
            data = in.read(1);
            if(data == ">")
            {
                data = in.readLine();
                data.simplified();
                args.append(data);
            }
            else
                in.readLine();
        }

        qint16 port, numConnections;
        port = args[0].toInt();
        if(port == 0)
            port = 3074;
        QHostAddress addr(args[1]);
        numConnections = args[2].toInt();
        m_tcpServer = new QTcpServer(this);
        m_tcpServer->setMaxPendingConnections(numConnections);
        m_tcpServer->listen(addr, port);

        conf.close();
        return;

    }
    else
    {
        std::cout<<"[CRITICAL] "<<conf.fileName().toStdString()<<" : "<<conf.errorString().toStdString()<<std::endl;
        exit(-1);
    }

}


/********************
 *    CONSTRUCTOR   *
 ********************/
/*!
*  \brief Constructor
*/
Server::Server(QObject* parent) : QObject(parent)
{
    this->init();

    if(m_tcpServer->isListening()){
        QObject::connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    }
    else{
        std::cout<<"[CRITICAL] "<<m_tcpServer->errorString().toStdString()<<std::endl;
        exit(-1);
    }
}

/********************
 *    DESTRUCTOR    *
 ********************/
/*!
*  \brief Destructor
*/
Server::~Server()
{
    while(!m_listClients.empty()) delete m_listClients.front(), m_listClients.pop_front();
    while(!m_listChannels.empty()) delete m_listChannels.front(), m_listChannels.pop_front();
    delete(m_tcpServer);
}

/********************
 *    SLOTS         *
 ********************/
/*!
*  \brief Create and add a client to the server
*/
void Server::onNewConnection(void)
{

     QTcpSocket* socket = m_tcpServer->nextPendingConnection();

     Client* c = new Client(socket, this);
     m_listClients.push_back(c);
}


/********************
 *    METHODS       *
 ********************/
/*!
*  \return the channel corresponding to the name given or null if channel doesn't exist.
*/
Channel* Server::getChannelFromName(QString& name)
{
    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it != m_listChannels.end(); ++it)
    {
        if((*it)->getChannelName().compare(name) == 0)
            return (*it);
    }

    return NULL;
}

/*!
*  \return the client corresponding to the name given or null if channel doesn't exist.
*/
Client* Server::getClientFromName(QString& name)
{
    for(std::list<Client*>::iterator it = m_listClients.begin(); it != m_listClients.end(); ++it)
    {
        if((*it)->getNickname().compare(name) == 0)
            return (*it);
    }

    return NULL;
}


/*!
*  \brief Remove a client from the server list of clients.
*/
void Server::delClient(Client* c)
{
    for(std::list<Channel*>::iterator it = m_listChannels.begin(); it !=m_listChannels.end(); ++it)
    {
        if((*it)->isStatus(c, REGULAR))
        {
            (*it)->removeClient(c);
            if((*it)->isEmpty())
                m_listChannels.remove(*it--);
        }
    }
    m_listClients.remove(c);
}
/*!
*  \brief Broadcast a message on several channels/to several clients.
*
*  \param message : The message to broadcast.
*  \param id : the id of the message (in our protocol, always 255).
*  \param code : see protocol description document.
*  \param chan : if chan is not specified, message will be sent to all clients connected to the server else,
*    the message will be sent to all the clients connected to the channel.
*  \param sender : if a sender is specified, message will not be sent to him.
*  false sinon
*/
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
        QString data = c->getNickname() + "\n" + message;
        QByteArray response = Frame::getReadyToSendFrame(data, 255, 129);
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
        if(dest_channel->isEmpty())
            this->m_listChannels.remove(dest_channel);
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
    QRegExp reg(filter, Qt::CaseSensitive, QRegExp::WildcardUnix);

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
    QRegExp reg(filter, Qt::CaseSensitive, QRegExp::WildcardUnix);

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

quint8 Server::kick(Client* c, QString& dest_channel, QString& filter)
{
    QString response("");
    QString name;                           // a nickname found corresponding to the filter
    QRegExp reg(filter, Qt::CaseSensitive, QRegExp::WildcardUnix);
    bool match = false;                     // true if one client correponding to the filter is found (used for the returned value)

    Channel* chan = getChannelFromName(dest_channel);
    if(chan == NULL)
    {
        return ERROR::eNotExist;
    }


    if (chan->isStatus(c, OPERATOR) != true)
    {
        return ERROR::eNotAuthorised;
    }


    for (std::list<Client*>::iterator it = chan->getClientList(REGULAR).begin(); it != chan->getClientList(REGULAR).end(); ++it)
    {
        if(reg.exactMatch((*it)->getNickname()) &&  !(chan->isStatus((*it), OPERATOR)))
        {
            name = (*it)->getNickname();
            response = "#" + dest_channel + "\n" + name + "\n" + c->getNickname();
            chan->removeClient(*it--);
            broadCast(response, 255, 134, chan, c);
            match = true;
        }
    }

    if(match)
        return ERROR::esuccess;
    else
        return ERROR::eNotExist;

}



/*********************************************
 *                  BAN                      *
 *********************************************/
quint8 Server::ban(Client* c, QString& dest_channel, QString& filter)
{
    QString response("");
    QRegExp reg(filter, Qt::CaseSensitive, QRegExp::WildcardUnix);

    Channel* chan = getChannelFromName(dest_channel);
    if(chan == NULL)
    {
        return ERROR::eNotExist;
    }


    if (chan->isStatus(c, OPERATOR) != true)
    {
        return ERROR::eNotAuthorised;
    }


    for (std::list<Client*>::iterator it = chan->getClientList(REGULAR).begin(); it != chan->getClientList(REGULAR).end(); ++it)
    {
        if(reg.exactMatch((*it)->getNickname()) &&  !(chan->isStatus((*it), OPERATOR)))
        {
            chan->addClient(*it, BANNED);
            chan->removeClient(*it--);
        }
    }

    response = "#" + dest_channel + "\n" + "-" + "\n" + filter;
    broadCast(response, 255, 135, chan, c);
    return ERROR::esuccess;

}


/*********************************************
 *                  UNBAN                    *
 *********************************************/
quint8 Server::unban(Client* c, QString& dest_channel, QString& filter)
{
    QString response("");
    QRegExp reg(filter, Qt::CaseSensitive, QRegExp::WildcardUnix);

    Channel* chan = getChannelFromName(dest_channel);
    if(chan == NULL)
    {
        return ERROR::eNotExist;
    }


    if (chan->isStatus(c, OPERATOR) != true)
    {
        return ERROR::eNotAuthorised;
    }


    for (std::list<Client*>::iterator it = chan->getClientList(BANNED).begin(); it != chan->getClientList(BANNED).end(); ++it)
    {
        if( reg.exactMatch((*it)->getNickname()) )
        {
            chan->unbanClient(*it--);
        }
    }

    response = "#" + dest_channel + "\n" + "-" + "\n" + filter;
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
    response = c->getNickname() + "\n" + chan->getChannelName() + "\n" + "op : " + cli->getNickname() ;
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
    response = c->getNickname() + "\n" + chan->getChannelName() + "\n" + "deop : " + cli->getNickname();
    broadCast(response, 255, 130, chan, c);

    return ERROR::esuccess;
}
