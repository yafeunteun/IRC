#include "channel.h"
#include "bdPlatformLog.h"

Channel::Channel(const QString &name): m_channel_name(name)
{
}

void Channel::setTopic( const QString& _topic )
{
    m_topic = _topic;
}

QString& Channel::getTopic( void )
{
    return m_topic;
}

QString& Channel::getChannelName( void )
{
    return m_channel_name;
}

std::list<Client*>& Channel::getClientList( status s )
{
    switch (s) {
    case BANNED:
        return m_banList;
        break;
    case REGULAR:
        return m_clientList;
        break;
    case OPERATOR:
        return m_operatorList;
        break;
    default:
        bdPlatformLog::bdLogMessage(_ERROR, "fatal_error/", "channel", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Couldn't evaluate second argument wich must be of type status (see channel.h)");
        exit(-1);
    }
}

void Channel::addClient(Client *c , status s)
{
    switch (s) {
    case BANNED:
        m_banList.push_front(c);
        break;
    case REGULAR:
        m_clientList.push_front(c);
        break;
    case OPERATOR:
        m_clientList.push_front(c);
        m_operatorList.push_front(c);
        break;
    default:
        bdPlatformLog::bdLogMessage(_ERROR, "fatal_error/", "channel", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Couldn't evaluate second argument wich must be of type status (see channel.h)");
        exit(-1);
    }
}


void Channel::unbanClient(Client *c)
{
    m_banList.remove(c);
}


void Channel::removeClient(Client *c)
{ 
    if(isStatus(c, OPERATOR))
        m_operatorList.remove(c);
    m_clientList.remove(c);
}


 bool Channel::isStatus(Client * c, status s)
 {
     switch (s) {
     case BANNED:
         for(std::list<Client*>::iterator it = m_banList.begin(); it != m_banList.end(); ++it)
         {
             if((*it)->getNickname() == c->getNickname())
                 return true;
         }
         return false;
         break;
     case REGULAR:
         for(std::list<Client*>::iterator it = m_clientList.begin(); it != m_clientList.end(); ++it)
         {
             if((*it)->getNickname() == c->getNickname())
                 return true;
         }
         return false;
         break;
     case OPERATOR:
         for(std::list<Client*>::iterator it = m_operatorList.begin(); it != m_operatorList.end(); ++it)
         {
             if((*it)->getNickname() == c->getNickname())
                 return true;
         }
         return false;
         break;
     default:
         bdPlatformLog::bdLogMessage(_ERROR, "fatal_error/", "channel", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Couldn't evaluate second argument wich must be of type status (see channel.h)");
         exit(-1);
     }





 }
