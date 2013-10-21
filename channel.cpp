#include "channel.h"

Channel::Channel(const QString &name): channel_name(name)
{
}

void Channel::setTopic( const QString& _topic )
{
    topic = _topic;
}

QString& Channel::getTopic( void )
{
    return topic;
}

QString& Channel::getChannelName( void )
{
    return channel_name;
}

std::list<Client*>& Channel::getClientList( void )
{
    return clientList;
}

void Channel::addClient( Client *c )
{
    clientList.push_front(c);
}
