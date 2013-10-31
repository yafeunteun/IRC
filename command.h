#ifndef COMMAND_H
#define COMMAND_H

#include <QString>
#include <QStringList>
#include "server.h"
#include "frame.h"

namespace ERROR {
    enum{
        esuccess = 0,
        eBadArg = 250,
        eNickCollision,
        eNotAuthorised,
        eMissingArg,
        eNotExist,
        eNoSufficientPermissions,
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

class topic : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_dest_channel, m_topic;
public:
    topic(Client* sender, Frame &frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->topic(m_sender, m_dest_channel, m_topic); }
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

class cwho : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_dest_channel;
public:
    cwho(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->cwho(m_sender, m_dest_channel); }
};

class kick : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_dest_channel, m_dest_client, m_reason;
public:
    kick(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->kick(m_sender, m_dest_channel, m_dest_client, m_reason); }
};

class ban : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_dest_channel, m_dest_client, m_reason;
public:
    ban(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->ban(m_sender, m_dest_channel, m_dest_client, m_reason); }
};

class unban : public Command //We don't support multi-unban as purposed on the subject.
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_dest_channel, m_dest_client, m_reason;
public:
    unban(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->unban(m_sender, m_dest_channel, m_dest_client, m_reason); }
};

class banlist : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_dest_channel;
public:
    banlist(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->banlist(m_sender, m_dest_channel); }
};

class makeOP : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_dest_channel, m_dest_client;
public:
    makeOP(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->makeOP(m_sender, m_dest_channel, m_dest_client); }
};

class remOP : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_dest_channel, m_dest_client;
public:
    remOP(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->remOP(m_sender, m_dest_channel, m_dest_client); }
};

#endif // COMMAND_H
