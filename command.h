#ifndef COMMAND_H
#define COMMAND_H

#include <QString>
#include <QStringList>
#include <server.h>
#include "frame.h"

namespace ERROR {
    enum{
        esuccess = 0,
        eBadArg = 250,
        eNickCollision,
        eNotAuthorised,
        eMissingArg,
        eNotExist,
        error
    };

}


namespace CMD {


    enum{
            C_PRIVMSG = 1,
            C_PUBMSG,
            C_GWHO,
            C_CWHO,
            C_LIST,
            C_TOPIC,
            C_KICK,
            C_BAN,
            C_OP,
            C_DEOP = 20,
            C_JOIN,
            C_NICK,
            C_LEAVE,
            C_UNBAN,
            C_BANLIST
        };
}




class Command
{

public:
    static Command* getCommand(Client* c, Frame &frame);
    virtual quint8 execute() = 0;
    virtual quint8 verify() = 0;
protected:
    Command();
    virtual ~Command() {}

};


class nick : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_nickname;
    static QString s_regex;
public:
    nick(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->nick(m_sender, m_nickname); }
};


class privmsg : public Command
{
private:
    Client* m_sender;
    Server* m_receiver ;
    QString m_dest_nickname, m_message;
public:
    privmsg(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->privmsg(m_sender, m_dest_nickname, m_message); }
};

class pubmsg : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_dest_channel, m_message;
public:
    pubmsg(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->pubmsg(m_sender, m_dest_channel, m_message); }
};

class join : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_dest_channel;
    static QString s_regex;
public:
    join(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->join(m_sender, m_dest_channel); }
};

class leave : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_dest_channel;
public:
    leave(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->leave(m_sender, m_dest_channel); }
};

class listCommand : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString filter;
public:
    listCommand(Client* sender, QStringList args) : m_sender(sender), filter(args[0]){ m_receiver = Server::Instance();}
    virtual quint8 verify() { return true;}
    virtual quint8 execute() { return m_receiver->listChannel(m_sender, filter); }
};

class topicCommand : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString dest_channel, topic;
public:
    topicCommand(Client* sender, QStringList args, QStringList _args) : m_sender(sender), dest_channel(args[0]), topic(_args.join(" ")){ m_receiver = Server::Instance();}
    virtual quint8 verify() { return true;}
    virtual quint8 execute() { return m_receiver->setTopic(m_sender, dest_channel, topic); }
};

class gwho : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_filter;
public:
    gwho(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->gwho(m_sender, m_filter); }
};

class whoCCommand : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString dest_channel;
public:
    whoCCommand(Client* sender, QStringList args) : m_sender(sender), dest_channel(args[0]){ m_receiver = Server::Instance();}
    virtual quint8 verify() { return true;}
    virtual quint8 execute() { return m_receiver->whoChannel(m_sender, dest_channel); }
};

#endif // COMMAND_H
