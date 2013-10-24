#ifndef COMMAND_H
#define COMMAND_H

#include <QString>
#include <QStringList>
#include <server.h>

namespace CMD {

    enum ACTION_LIST
    {
        UNK_CMD,
        CLMSGCL_CMD, //Private message to a client k
        CLMSGCH_CMD, //Message to a channel k
        WHOG_CMD, //General WHO k
        WHOC_CMD, //Channel WHO k
        LIST_CMD, //k
        TOPIC_CMD, //k
        KICK_CMD,
        BAN_CMD,
        OP_CMD,
        DEOP_CMD,
        JOIN_CMD, //k
        NICK_CMD, //k
        LEAVE_CMD, //k
        UNBAN_CMD,
        BANLST_CMD
    };

}


class Command
{

public:
    virtual ~Command() {}

    virtual quint8 execute() = 0;
protected:
    Command();
};


class nickCommand : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_nickname;
public:
    nickCommand(Client* sender, QStringList args) : m_sender(sender), m_nickname(args[0]){ m_receiver = Server::Instance();}
    virtual quint8 execute() { return m_receiver->nick(m_sender, m_nickname); }
};


class pmCommand : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString dest_nickname, message;
public:
    pmCommand(Client* sender, QStringList args, QStringList _args) : m_sender(sender), dest_nickname(args[0]), message(_args.join(" ")){ m_receiver = Server::Instance();}
    virtual quint8 execute() { return m_receiver->privateMessage(m_sender, dest_nickname, message); }
};

class chCommand : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString dest_channel, message;
public:
    chCommand(Client* sender, QStringList args, QStringList _args) : m_sender(sender), dest_channel(args[0]), message(_args.join(" ")){ m_receiver = Server::Instance();}
    virtual quint8 execute() { return m_receiver->channelMessage(m_sender, dest_channel, message); }
};

class joinCommand : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString dest_channel;
public:
    joinCommand(Client* sender, QStringList args) : m_sender(sender), dest_channel(args[0]){ m_receiver = Server::Instance();}
    virtual quint8 execute() { return m_receiver->joinChannel(m_sender, dest_channel); }
};

class leaveCommand : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString dest_channel;
public:
    leaveCommand(Client* sender, QStringList args) : m_sender(sender), dest_channel(args[0]){ m_receiver = Server::Instance();}
    virtual quint8 execute() { return m_receiver->leaveChannel(m_sender, dest_channel); }
};

class listCommand : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString filter;
public:
    listCommand(Client* sender, QStringList args) : m_sender(sender), filter(args[0]){ m_receiver = Server::Instance();}
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
    virtual quint8 execute() { return m_receiver->setTopic(m_sender, dest_channel, topic); }
};

class whoGCommand : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString filter;
public:
    whoGCommand(Client* sender, QStringList args) : m_sender(sender), filter(args[0]){ m_receiver = Server::Instance();}
    virtual quint8 execute() { return m_receiver->whoGeneral(m_sender, filter); }
};

class whoCCommand : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString dest_channel;
public:
    whoCCommand(Client* sender, QStringList args) : m_sender(sender), dest_channel(args[0]){ m_receiver = Server::Instance();}
    virtual quint8 execute() { return m_receiver->whoChannel(m_sender, dest_channel); }
};

#endif // COMMAND_H
