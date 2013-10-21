#ifndef COMMAND_H
#define COMMAND_H

#include <QString>
#include <QStringList>
#include <server.h>

namespace CMD {

    enum ACTION_LIST
    {
        UNK_CMD,
        CLMSGCL_CMD, //Private message to a client
        CLMSGCH_CMD, //Message to a channel
        WHOG_CMD, //General WHO
        WHOC_CMD, //Channel WHO
        LIST_CMD,
        TOPIC_CMD,
        KICK_CMD,
        BAN_CMD,
        OP_CMD,
        DEOP_CMD,
        JOIN_CMD,
        NICK_CMD,
        LEAVE_CMD,
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




#endif // COMMAND_H
