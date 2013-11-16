#ifndef CHANNEL_H
#define CHANNEL_H


/*!
 * \file channel.h
 * \brief This file gathers tools to manage clients inside channels.
 */


#include <list>
#include <iostream>
#include "client.h"

class Client;


/**
 * \enum status
 * \brief Status of a client on the channel
 */
typedef enum
{
    BANNED,     /*!< Client is banned of the channel */
    REGULAR,    /*!< Client is connected to the channel */
    OPERATOR    /*!< Client is operator on the channel note that it means the client is also connected to the channel */
}
status;


class Channel
{

/*! \class Channel
* \brief Class reprensenting a channel
*
*  This class manage 3 lists of clients : banned, regular (connected to the channel including operators) and operator clients.
*/

private:
    std::list<Client*> m_clientList;        /*!< Contains both the regular clients and operators */
    std::list<Client*> m_operatorList;      /*!< Contains only operators of the channel */
    std::list<Client*> m_banList;           /*!< Contains the clients who are banned from the channel */
    QString m_topic;                        /*!< The topic of the channel */
    QString m_channel_name;                 /*!< The name of the channel */

public:
    Channel( const QString& name );
    ~Channel() {}

    void setTopic(const QString &topic );
    QString& getTopic( void );
    QString& getChannelName( void );
    std::list<Client*>& getClientList( status s = REGULAR );
    void addClient( Client *c, status s = REGULAR );
    void removeClient(Client *c);
    void unbanClient(Client *c);
    bool isStatus(Client * c, status s = REGULAR);
    void setOperator(Client* c);
    void unsetOperator(Client* c);
    bool isEmpty(void);
};

#endif // CHANNEL_H
