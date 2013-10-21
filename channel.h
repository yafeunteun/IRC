#ifndef CHANNEL_H
#define CHANNEL_H

#include <list>
#include <iostream>
#include "client.h"

class Channel
{
private:
    std::list<Client*> clientList;
    std::list<Client*> operatorList;
    std::string topic;

public:
    Channel();
    ~Channel();

    void setTopic( const std::string& _topic );
    std::string getTopic( void );
};

#endif // CHANNEL_H
