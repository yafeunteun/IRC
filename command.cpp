#include "command.h"
#include <QRegularExpression>

using namespace CMD;

QString nick::s_regex = "^[a-z]\\S{2,8}$";
QString join::s_regex = "^[a-z]\\S{2,8}$";



Command::Command()
{

}



Command* Command::getCommand(Client *c, Frame &frame)
{

    switch(frame.getCode())
    {
        case(C_PRIVMSG): return new privmsg(c, frame); break;
        case(C_PUBMSG): return new pubmsg(c, frame); break;
        case(C_GWHO): return new gwho(c, frame); break;
        case(C_CWHO): return new cwho(c, frame); break;
        //case(C_LIST): return new list(c, frame); break;
        //case(C_TOPIC): return new topic(c, frame); break;
        //case(C_KICK): return new kick(c, frame); break;
        //case(C_BAN): return new ban(c, frame); break;
        //case(C_OP): return new op(c, frame); break;
        //case(C_DEOP): return new deop(c, frame); break;
        case(C_JOIN): return new join(c, frame); break;
        case(C_NICK): return new nick(c, frame); break;
        case(C_LEAVE): return new leave(c, frame); break;
        //case(C_UNBAN): return new unban(c, frame); break;
        //case(C_BANLIST): return new banlist(c, frame); break;
        default: /* error */; return NULL;
   }

}


nick::nick(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;
    if(frame.getNbArg() < 1)
        m_nickname = "";
    else
        m_nickname = frame.getArgList()[0];
}

quint8 nick::verify()
{
    if(m_nickname.isEmpty())
        return ERROR::eMissingArg;

    QRegularExpression reg;
    reg.setPattern(s_regex);
    reg.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    if(!reg.isValid()){
        std::cout<<"FATAL ERROR ! Pattern for regex is invalid !"<<std::endl;
        exit(-1);
    }

    if(!(reg.match(m_nickname).hasMatch())){
        return ERROR::eBadArg;        // Invalid nickname !
    }

    return ERROR::esuccess;
}

privmsg::privmsg(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;
    if(frame.getNbArg() < 1)
    {
        m_dest_nickname = "";
        m_message = "";
    }
    else if(frame.getNbArg() < 2)
    {
        m_message = "";
    }
    else
    {
        m_dest_nickname = frame.getArgList()[0];
        m_message = frame.getArgList()[1];
    }
}

quint8 privmsg::verify()
{
    if(m_dest_nickname.isEmpty() || m_message.isEmpty())
        return ERROR::eMissingArg;
    return ERROR::esuccess;
}

join::join(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;
    if(frame.getNbArg() < 1)
        m_dest_channel = "";
    else
        m_dest_channel = frame.getArgList()[0];     // we can join one channel at a time
}

quint8 join::verify()
{
    if(m_dest_channel.isEmpty())
        return ERROR::eMissingArg;

    QRegularExpression reg;
    reg.setPattern(s_regex);
    reg.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    if(!reg.isValid()){
        std::cout<<"FATAL ERROR ! Pattern for regex is invalid !"<<std::endl;
        exit(-1);
    }

    if(!(reg.match(m_dest_channel).hasMatch())){
        return ERROR::eBadArg;
    }


    return ERROR::esuccess;
}

pubmsg::pubmsg(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;
    if(frame.getNbArg() < 1)
    {
        m_dest_channel = "";
        m_message = "";
    }
    else if(frame.getNbArg() < 2)
    {
        m_message = "";
    }
    else
    {
        m_dest_channel = frame.getArgList()[0];
        m_message = frame.getArgList()[1];
    }
}

quint8 pubmsg::verify()
{
    if(m_dest_channel.isEmpty() || m_message.isEmpty())
        return ERROR::eMissingArg;
    return ERROR::esuccess;
}


leave::leave(Client *sender, Frame &frame)
{

    m_receiver = Server::Instance();
    m_sender = sender;
    if(frame.getNbArg() < 1)
        m_dest_channel = "";
    else
        m_dest_channel = frame.getArgList()[0];
}

quint8 leave::verify()
{
    if(m_dest_channel.isEmpty())
        return ERROR::eMissingArg;
    return ERROR::esuccess;
}

gwho::gwho(Client *sender, Frame &frame)
{

    m_receiver = Server::Instance();
    m_sender = sender;
    if(frame.getNbArg() < 1)
        m_filter = "";
    else
        m_filter = frame.getArgList()[0];
}

quint8 gwho::verify()
{

    if(m_filter.isEmpty())
        return ERROR::eMissingArg;

    QRegularExpression regex(m_filter);
    if(!regex.isValid())
        return ERROR::eBadArg;

    return ERROR::esuccess;
}

cwho::cwho(Client *sender, Frame &frame)
{
    m_receiver = Server::Instance();
    m_sender = sender;
    if(frame.getNbArg() < 1)
        m_dest_channel = "";
    else
        m_dest_channel = frame.getArgList()[0];
}

quint8 cwho::verify()
{
    if(m_dest_channel.isEmpty())
        return ERROR::eMissingArg;
    return ERROR::esuccess;
}
