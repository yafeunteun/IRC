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
    std::list<Client*> m_clientList;
    std::list<Client*> m_operatorList;
    std::list<Client*> m_banList;
    QString m_topic, m_channel_name;

public:
    Channel( const QString& name );
    ~Channel();

    void setTopic(const QString &_topic );
    QString& getTopic( void );
    QString& getChannelName( void );
    std::list<Client*>& getClientList( status s = REGULAR );
    void addClient( Client *c, status s = REGULAR );
    bool isBanned(Client * c);
};

#endif // CHANNEL_H
