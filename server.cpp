#include "server.h"

#define max(X, Y) ((X)>Y ? (X):(Y))

Server::Server(qint16 port, int numConnections, const QHostAddress & address) : QTcpServer()
{
    if (!(this->listen(address, port)));
         //do something in case of error
    this->setMaxPendingConnections(numConnections);
}


void Server::init()
{
    /* Initialize variables for select() function */
    FD_ZERO(&m_fdset);
    m_tv.tv_sec = m_time;
    m_tv.tv_usec = 0;
    FD_SET(this->socketDescriptor(), &m_fdset);

    /* Delete clients who are not connected and add client who are connected to the fd_set */
    for(std::list<Client>::iterator it = m_listClients.begin(); it != m_listClients.end(); ++it)
    {
        if((*it).state() == QAbstractSocket::UnconnectedState)
        {
            // delete client
        }

        else
        {
            unsigned int socket_who_his_still_connected = (*it).socketDescriptor();
            FD_SET(socket_who_his_still_connected, &m_fdset);
            m_fdsMax = max(socket_who_his_still_connected, m_fdsMax);
        }

    }
}



void Server::execute(void)
{

    for(;;)
    {
        this->init();
        int retval = select(++m_fdsMax, &m_fdset, NULL, NULL, &m_tv);

        switch(retval){
        case -1 : // error
            break;
        case 0 : // no activity
            break;
        default :
            for(std::list<Client>::iterator it = m_listClients.begin(); it != m_listClients.end(); ++it)
            {
                if(FD_ISSET((*it).socketDescriptor(), &m_fdset))
                {
                    // If client as something to say ...
                }
            }
            if(this->hasPendingConnections())
            {
                // If there is Pending Connection
            }
        }

    }
}

