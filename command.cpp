#include "command.h"
#include <QRegularExpression>
#include "unixregexp.h"

using namespace CMD;

QString nick::s_regex = "^[a-z]\\S{2,8}$";  /*!< Regular expression used to verify the validity of a nickname */
QString join::s_regex = "^[a-z]\\S{2,8}$";  /*!< Regular expression used to verify the validity of a channel name created with join command */


/*!
*  \brief Constructor
*/
Command::Command()
{
}

/*!
*  \brief Get a parameterized command object from a raw Frame object.
*
*  This method uses the parameters and the command id code contained in the Frame object
*   to return a specialized command (inherited from Command) and parameterized correctly, ready to be self verified and executed.
*
*  \param c : The client who requests the Command object.
*  \param frame : The frame providing all the informations to instantiate the
*   right Command object well parameterized.
*  \return A pointer on the Command object parameterized according to the Frame object.
*/
Command* Command::getCommand(Client *c, Frame &frame)
{

    switch(frame.getCode())
    {
        case(C_PRIVMSG): return new privmsg(c, frame); break;
        case(C_PUBMSG): return new pubmsg(c, frame); break;
        case(C_GWHO): return new gwho(c, frame); break;
        case(C_CWHO): return new cwho(c, frame); break;
        case(C_LIST): return new list(c, frame); break;
        case(C_TOPIC): return new topic(c, frame); break;
        case(C_KICK): return new kick(c, frame); break;
        case(C_BAN): return new ban(c, frame); break;
        case(C_OP): return new op(c, frame); break;
        case(C_DEOP): return new deop(c, frame); break;
        case(C_JOIN): return new join(c, frame); break;
        case(C_NICK): return new nick(c, frame); break;
        case(C_LEAVE): return new leave(c, frame); break;
        case(C_UNBAN): return new unban(c, frame); break;
        case(C_BANLIST): return new banlist(c, frame); break;
        default: /* error */; return NULL;
   }

}

/*!
*  \brief Constructor
*
*  \param sender : A pointer on the client who requests the Command object.
*  \param frame : The frame providing all the informations to parameterized the command.
*
*/
nick::nick(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;
    if(frame.getNbArg() < 1)
        m_nickname = "";
    else
        m_nickname = frame.getArgList()[0];
}

/*!
*  \brief Check the validity of the command according to the parameters contained in the frame given to the construcor.
*
*  The method begin to check if one argument is missing and then check wether the nickname given is valid or not.
*
*  \return ERROR::eMissingArg if an argument is missing, ERROR::eBadArg if the nickname given isn't valid, ERROR::esuccess if
*   the command is valid.
*/
quint8 nick::verify()
{
    if(m_nickname.isEmpty())
        return ERROR::eMissingArg;

    QRegularExpression reg;
    reg.setPattern(s_regex);
    reg.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    if(!reg.isValid()){
        std::cerr<<"FATAL ERROR ! Pattern for regex is invalid !"<<std::endl;
        exit(-1);
    }

    if(!(reg.match(m_nickname).hasMatch())){
        return ERROR::eBadArg;        // Invalid nickname !
    }

    return ERROR::esuccess;
}

/*!
*  \brief Constructor
*
*  \param sender : A pointer on the client who requests the Command object.
*  \param frame : The frame providing all the informations to parameterized the command.
*
*/
privmsg::privmsg(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;

    if(frame.getNbArg() < 2)
    {
        m_dest_nickname = "";
    }
    else
    {
        m_dest_nickname = frame.getArgList()[0];
        m_message = frame.getArgList()[1];
    }
}

/*!
*  \brief Check the validity of the command according to the parameters contained in the frame given to the construcor.
*
*  The method check wether one argument is missing or not.
*
*  \return ERROR::eMissingArg if an argument is missing, ERROR::esuccess if
*   the command is valid.
*/
quint8 privmsg::verify()
{
    if(m_dest_nickname.isEmpty())
        return ERROR::eMissingArg;
    return ERROR::esuccess;
}

/*!
*  \brief Constructor
*
*  \param sender : A pointer on the client who requests the Command object.
*  \param frame : The frame providing all the informations to parameterized the command.
*
*/
join::join(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;
    if(frame.getNbArg() < 1)
        m_dest_channel = "";
    else
        m_dest_channel = frame.getArgList()[0];
}

/*!
*  \brief Check the validity of the command according to the parameters contained in the frame given to the construcor.
*
*  The method begin to check if one argument is missing and then check wether the channel name given is valid or not.
*
*  \return ERROR::eMissingArg if an argument is missing, ERROR::eBadArg if the channel name given isn't valid, ERROR::esuccess if
*   the command is valid.
*/
quint8 join::verify()
{
    if(m_dest_channel.isEmpty())
        return ERROR::eMissingArg;

    QRegularExpression reg;
    reg.setPattern(s_regex);
    reg.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    if(!reg.isValid()){
        std::cerr<<"FATAL ERROR ! Pattern for regex is invalid !"<<std::endl;
        exit(-1);
    }

    if(!(reg.match(m_dest_channel).hasMatch())){
        return ERROR::eBadArg;
    }

    return ERROR::esuccess;
}

/*!
*  \brief Constructor
*
*  \param sender : A pointer on the client who requests the Command object.
*  \param frame : The frame providing all the informations to parameterized the command.
*
*/
pubmsg::pubmsg(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;

    if(frame.getNbArg() < 2)
    {
        m_dest_channel = "";
    }
    else
    {
        m_dest_channel = frame.getArgList()[0];
        m_message = frame.getArgList()[1];
    }
}

/*!
*  \brief Check the validity of the command according to the parameters contained in the frame given to the construcor.
*
*  The method check wether one argument is missing or not.
*
*  \return ERROR::eMissingArg if an argument is missing, ERROR::esuccess if
*   the command is valid.
*/
quint8 pubmsg::verify()
{
    if(m_dest_channel.isEmpty())
        return ERROR::eMissingArg;
    return ERROR::esuccess;
}

/*!
*  \brief Constructor
*
*  \param sender : A pointer on the client who requests the Command object.
*  \param frame : The frame providing all the informations to parameterized the command.
*
*/
leave::leave(Client *sender, Frame &frame)
{

    m_receiver = Server::Instance();
    m_sender = sender;
    if(frame.getNbArg() < 1)
        m_dest_channel = "";
    else
        m_dest_channel = frame.getArgList()[0];
}

/*!
*  \brief Check the validity of the command according to the parameters contained in the frame given to the construcor.
*
*  The method check wether one argument is missing or not.
*
*  \return ERROR::eMissingArg if an argument is missing, ERROR::esuccess if
*   the command is valid.
*/
quint8 leave::verify()
{
    if(m_dest_channel.isEmpty())
        return ERROR::eMissingArg;
    return ERROR::esuccess;
}

/*!
*  \brief Constructor
*
*  \param sender : A pointer on the client who requests the Command object.
*  \param frame : The frame providing all the informations to parameterized the command.
*
*/
gwho::gwho(Client *sender, Frame &frame)
{

    m_receiver = Server::Instance();
    m_sender = sender;
    if(frame.getNbArg() < 1)
        m_filter = "";
    else
        m_filter = frame.getArgList()[0];
}

/*!
*  \brief Check the validity of the command according to the parameters contained in the frame given to the construcor.
*
*  The method begin to check if one argument is missing and then check wether the regex given is valid or not.
*
*  \return ERROR::eMissingArg if an argument is missing, ERROR::eBadArg if the regex given isn't valid, ERROR::esuccess if
*   the command is valid.
*/
quint8 gwho::verify()
{

    if(m_filter.isEmpty())
        return ERROR::eMissingArg;

    UnixRegExp regex(m_filter);
    if(!regex.isValid())
        return ERROR::eBadArg;

    return ERROR::esuccess;
}

/*!
*  \brief Constructor
*
*  \param sender : A pointer on the client who requests the Command object.
*  \param frame : The frame providing all the informations to parameterized the command.
*
*/
list::list(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;
    if(frame.getNbArg() < 1)
        m_filter = "";
    else
        m_filter = frame.getArgList()[0];
}

/*!
*  \brief Check the validity of the command according to the parameters contained in the frame given to the construcor.
*
*  The method begin to check if one argument is missing and then check wether the regex given is valid or not.
*
*  \return ERROR::eMissingArg if an argument is missing, ERROR::eBadArg if the regex given isn't valid, ERROR::esuccess if
*   the command is valid.
*/
quint8 list::verify()
{
    if(m_filter.isEmpty())
        return ERROR::eMissingArg;

    UnixRegExp regex(m_filter);
    if(!regex.isValid())
        return ERROR::eBadArg;

    return ERROR::esuccess;
}

/*!
*  \brief Constructor
*
*  \param sender : A pointer on the client who requests the Command object.
*  \param frame : The frame providing all the informations to parameterized the command.
*
*/
cwho::cwho(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;
    if(frame.getNbArg() < 1)
        m_dest_channel = "";
    else
        m_dest_channel = frame.getArgList()[0];
}

/*!
*  \brief Check the validity of the command according to the parameters contained in the frame given to the construcor.
*
*  The method check wether one argument is missing or not.
*
*  \return ERROR::eMissingArg if an argument is missing, ERROR::esuccess if
*   the command is valid.
*/
quint8 cwho::verify()
{
    if(m_dest_channel.isEmpty())
        return ERROR::eMissingArg;
    return ERROR::esuccess;
}

/*!
*  \brief Constructor
*
*  \param sender : A pointer on the client who requests the Command object.
*  \param frame : The frame providing all the informations to parameterized the command.
*
*/
topic::topic(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;

    if(frame.getNbArg() < 2)
    {
        m_dest_channel = "";
    }
    else
    {
        m_dest_channel = frame.getArgList()[0];
        m_topic = frame.getArgList()[1];
    }
}

/*!
*  \brief Check the validity of the command according to the parameters contained in the frame given to the construcor.
*
*  The method check wether one argument is missing or not.
*
*  \return ERROR::eMissingArg if an argument is missing, ERROR::esuccess if
*   the command is valid.
*/
quint8 topic::verify()
{
    if(m_dest_channel.isEmpty())
        return ERROR::eMissingArg;
    return ERROR::esuccess;
}

/*!
*  \brief Constructor
*
*  \param sender : A pointer on the client who requests the Command object.
*  \param frame : The frame providing all the informations to parameterized the command.
*
*/
kick::kick(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;
    if(frame.getNbArg() < 2)    // missing argument
    {
        m_dest_channel = "";    // thus, when the command will be verified, eMissingArg will be sent
    }

    else
    {
        m_dest_channel = frame.getArgList()[0];
        m_dest_client = frame.getArgList()[1];
    }
}

/*!
*  \brief Check the validity of the command according to the parameters contained in the frame given to the construcor.
*
*  The method check wether one argument is missing or not.
*
*  \return ERROR::eMissingArg if an argument is missing, ERROR::esuccess if
*   the command is valid.
*/
quint8 kick::verify()
{
    if(m_dest_channel.isEmpty())
        return ERROR::eMissingArg;

    return ERROR::esuccess;
}

/*!
*  \brief Constructor
*
*  \param sender : A pointer on the client who requests the Command object.
*  \param frame : The frame providing all the informations to parameterized the command.
*
*/
ban::ban(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;
    if(frame.getNbArg() < 2)    // missing arg !
    {
        m_dest_channel = "";
    }
    else
    {
        m_dest_channel = frame.getArgList()[0];
        m_dest_client = frame.getArgList()[1];
    }
}

/*!
*  \brief Check the validity of the command according to the parameters contained in the frame given to the construcor.
*
*  The method check wether one argument is missing or not.
*
*  \return ERROR::eMissingArg if an argument is missing, ERROR::esuccess if
*   the command is valid.
*/
quint8 ban::verify()
{
    if(m_dest_channel.isEmpty())
        return ERROR::eMissingArg;
    return ERROR::esuccess;
}

/*!
*  \brief Constructor
*
*  \param sender : A pointer on the client who requests the Command object.
*  \param frame : The frame providing all the informations to parameterized the command.
*
*/
unban::unban(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;
    if(frame.getNbArg() < 2)
    {
        m_dest_channel = "";
    }
    else
    {
        m_dest_channel = frame.getArgList()[0];
        m_dest_client = frame.getArgList()[1];
    }
}

/*!
*  \brief Check the validity of the command according to the parameters contained in the frame given to the construcor.
*
*  The method check wether one argument is missing or not.
*
*  \return ERROR::eMissingArg if an argument is missing, ERROR::esuccess if
*   the command is valid.
*/
quint8 unban::verify()
{
    if(m_dest_channel.isEmpty())
        return ERROR::eMissingArg;
    return ERROR::esuccess;
}

/*!
*  \brief Constructor
*
*  \param sender : A pointer on the client who requests the Command object.
*  \param frame : The frame providing all the informations to parameterized the command.
*
*/
banlist::banlist(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;
    if(frame.getNbArg() < 1)
        m_dest_channel = "";
    else
        m_dest_channel = frame.getArgList()[0];
}

/*!
*  \brief Check the validity of the command according to the parameters contained in the frame given to the construcor.
*
*  The method check wether one argument is missing or not.
*
*  \return ERROR::eMissingArg if an argument is missing, ERROR::esuccess if
*   the command is valid.
*/
quint8 banlist::verify()
{
    if(m_dest_channel.isEmpty())
        return ERROR::eMissingArg;
    return ERROR::esuccess;
}

/*!
*  \brief Constructor
*
*  \param sender : A pointer on the client who requests the Command object.
*  \param frame : The frame providing all the informations to parameterized the command.
*
*/
op::op(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;

    if (frame.getNbArg() < 2)
    {
        m_dest_channel = "";
    }
    else
    {
        m_dest_channel = frame.getArgList()[0];
        m_dest_client = frame.getArgList()[1];
    }
}

/*!
*  \brief Check the validity of the command according to the parameters contained in the frame given to the construcor.
*
*  The method check wether one argument is missing or not.
*
*  \return ERROR::eMissingArg if an argument is missing, ERROR::esuccess if
*   the command is valid.
*/
quint8 op::verify()
{
    if(m_dest_channel.isEmpty())
        return ERROR::eMissingArg;
    return ERROR::esuccess;
}

/*!
*  \brief Constructor
*
*  \param sender : A pointer on the client who requests the Command object.
*  \param frame : The frame providing all the informations to parameterized the command.
*
*/
deop::deop(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;

    if (frame.getNbArg() < 2)
    {
        m_dest_channel = "";
    }
    else
    {
        m_dest_channel = frame.getArgList()[0];
        m_dest_client = frame.getArgList()[1];
    }
}

/*!
*  \brief Check the validity of the command according to the parameters contained in the frame given to the construcor.
*
*  The method check wether one argument is missing or not.
*
*  \return ERROR::eMissingArg if an argument is missing, ERROR::esuccess if
*   the command is valid.
*/
quint8 deop::verify()
{
    if(m_dest_channel.isEmpty())
        return ERROR::eMissingArg;
    return ERROR::esuccess;
}
