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
  * \brief Class defining the Server
  */
class Server : public QTcpServer
{
private:
    static Server* _instance;
    std::list<Client*> m_listClients;
protected:
    Server();
public:
    static Server* Instance();
    //Server(qint16 port = 1234, int numConnections = 30, const QHostAddress & address = QHostAddress::Any);

};

#endif // SERVER_H
