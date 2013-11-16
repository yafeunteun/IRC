#ifndef SERVER_H
#define SERVER_H



/*!
 * \file server.h
 * \brief IRC Server
 *  Near-IRC server : A simplified IRC server not in accordance with RFC 1459
 */

#include <QTcpServer>
#include <list>
#include "channel.h"

class QString;



/*! \class Server
  * \brief Class defining the Server.
  * This class inherits from QObjet to use slots and signals.
  * This class uses the pattern Singleton.
  */
class Server : public QObject
{
    Q_OBJECT
private:
    static Server* _instance;
    QTcpServer *m_tcpServer;                /*! Provides a TCP-based server */
    std::list<Client*> m_listClients;       /*! The list of the clients connected to the server */
    std::list<Channel*> m_listChannels;     /*! The list of the channel of the server */

protected:
    Server(QObject *parent = 0);
    void init(); // initialize the server with the file server.conf
    void broadCast(QString& message, quint16 id, quint8 code, Channel *chan = NULL, Client *sender = NULL);
public:
    ~Server();
    static Server* Instance();
    void delClient(Client* c);
    Channel* getChannelFromName(QString& name);
    Client* getClientFromName(QString& name);


    /*************
     *  COMMANDS *
     *************/
    quint8 nick(Client* c, QString& nickname);
    quint8 privmsg(Client* c, QString& dest, QString& message);
    quint8 pubmsg(Client* c, QString& dest, QString& message);
    quint8 join(Client* c, QString& dest);
    quint8 leave(Client* c, QString& dest);
    quint8 list(Client* c, QString& filter);
    quint8 topic(Client* c, QString& dest, QString& topic);
    quint8 gwho(Client* c, QString& filter);
    quint8 cwho(Client* c, QString& dest);
    quint8 kick(Client* c, QString& dest_channel, QString& filter);
    quint8 ban(Client* c, QString& dest_channel, QString& filter);
    quint8 unban(Client* c, QString& dest_channel, QString& filter);
    quint8 banlist(Client* c, QString& dest_channel);
    quint8 op(Client* c, QString& dest_channel, QString& dest_client);
    quint8 deop(Client* c, QString& dest_channel, QString& dest_client);

public slots:
    void onNewConnection(void);

};

#endif // SERVER_H
