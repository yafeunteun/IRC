#include "channel.h"
#include "bdPlatformLog.h"



/*!
*  \brief Constructor
*
*  \param name : Name of the channel.
*/
Channel::Channel(const QString &name): m_channel_name(name)
{
}

/*!
*  \brief Set the topic of the channel.
*
*  \param topic : A string containing the topic of the channel.
*  \return void.
*/
void Channel::setTopic( const QString& topic )
{
    m_topic = topic;
}


/*!
*  \brief Getter of the topic of the channel.
*
*  \return A string containing the topic of the channel.
*/
QString& Channel::getTopic( void )
{
    return m_topic;
}

/*!
*  \brief Getter of the name of the channel.
*
*  \return A string containing the name of the channel.
*/
QString& Channel::getChannelName( void )
{
    return m_channel_name;
}


/*!
*  \brief Getter of the client lists of the channel.
*
*  For example : sampleChannel.getClientList(OPERATOR) returns the operator clients list of the channel.
*
*  If no status is given, the getter returns the connected clients list.
*
*  \param s : The client list you want to get
*  \return A list (from STL) containing the adresses of clients depending on the status given in parameter.
*
*/
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



/*!
*  \brief Allows to add and/or set the status of a client on the channel.
*
*  \param c : The address of the client you want to add and or change the status on the channel.
*  \param s : The status you want to give to the client you are adding.
*  \return void.
*/
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


/*!
*  \brief Unban a client
*
*  \param c : The address of the client you want to unban.
*  \return void.
*/
void Channel::unbanClient(Client *c)
{
    m_banList.remove(c);
}

/*!
*  \brief Remove a client from a channel.
*
*  Remove a client from the regular and operator lists if the client is operator on the channel.
*  This function doesn't intend to remove a client from the ban list, if you want to do so,
*  you should use unbanClient().
*
*  \param c : The address of the client you want to remove from the channel.
*  \return void.
*/
void Channel::removeClient(Client *c)
{ 
    if(isStatus(c, OPERATOR))
        m_operatorList.remove(c);
    m_clientList.remove(c);
}


/*!
*  \brief Check the status of a client on a channel.
*
*  \param c : The address of the client you want to check the status.
*  \param s : The status you want to check if the client has.
*  \return True if the client c is in the list corresponding to status given, false either.
*/
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
