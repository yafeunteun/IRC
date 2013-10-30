#ifndef CHANNEL_H
#define CHANNEL_H

#include <list>
#include <iostream>
#include "client.h"

class Client;

typedef enum {BANNED, REGULAR, OPERATOR} status;


class Channel
{
private:
    std::list<Client*> m_clientList;        // contains both the regular clients and operators
    std::list<Client*> m_operatorList;      // contains only operators of the channel
    std::list<Client*> m_banList;           // contains the clients who are banned from the channel
    QString m_topic, m_channel_name;

public:
    Channel( const QString& name );
    ~Channel();

    void setTopic(const QString &_topic );
    QString& getTopic( void );
    QString& getChannelName( void );
    std::list<Client*>& getClientList( status s = REGULAR );
    void addClient( Client *c, status s = REGULAR );
    bool isStatus(Client * c, status s = REGULAR);  // true if the client c is in the list corresponding to status given.
};

#endif // CHANNEL_H
