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
    fd_set m_fdset;
    std::list<Client> m_listClients;
    struct timeval m_tv;
    unsigned int m_time;
    unsigned int m_fdsMax;
public:
/*!
*  \brief Constructor
*  \param port : Tells the server to listen for incoming connections on port given
*  \param numConnections : Sets the maximum number of pending accepted connections to numConnections
*  \param address : Tells the server to listen for incoming connections on address given
*/
    Server(qint16 port = 1234, int numConnections = 30, const QHostAddress & address = QHostAddress::Any);
    void init();
    void execute(void);
};

#endif // SERVER_H
