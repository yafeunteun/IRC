#include "channel.h"

Channel::Channel()
{
}

void Channel::setTopic( const std::string& _topic )
{
    topic = _topic;
}

std::string Channel::getTopic( void )
{
    return topic;
}
