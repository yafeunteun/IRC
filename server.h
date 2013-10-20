#ifndef SERVER_H
#define SERVER_H

/*!
 * \file server.h
 * \brief IRC Server
 *  Near-IRC server : A simplified IRC server not in accordance with RFC 1459
 * \author yann feunteun
 * \version 0.1
 */

#include <QTcpServer>
#include <list>
#include "client.h"
#include "channel.h"



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
    QTcpServer *m_tcpServer;
    std::list<Client*> m_listClients;
protected:
    Server(QObject *parent = 0);
public:
    ~Server();
    static Server* Instance();
    void delClient(Client* c);
    quint8 nick(Client* c, QString& nickname);
public slots:
    void onNewConnection(void);

};

#endif // SERVER_H
