#ifndef CHANNEL_H
#define CHANNEL_H

#include <list>
#include <iostream>
#include "client.h"

class Client;

class Channel
{
private:
    std::list<Client*> clientList;
    std::list<Client*> operatorList;
    QString topic, channel_name;

public:
    Channel( const QString& name );
    ~Channel();

    void setTopic(const QString &_topic );
    QString& getTopic( void );
    QString& getChannelName( void );
    std::list<Client*>& getClientList( void );
    void addClient( Client *c );
};

#endif // CHANNEL_H
