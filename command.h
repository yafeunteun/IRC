#ifndef COMMAND_H
#define COMMAND_H

/*!
* \file command.h
* \brief File containing the declaration of the commands
* \author yann feunteun
*/

#include "server.h"
#include "frame.h"


/*! \namespace ERROR
*
* Namespace gathering error codes binded to the commands
*/
namespace ERROR {

/**
 * \enum
 * \brief Error code constants.
 */
    enum{
        esuccess = 0,       /*!< Command has been executed successfully */
        eBadArg = 250,      /*!< The parameter is non-compliant */
        eNickCollision,     /*!< The nickname is already in use by another client */
        eNotAuthorised,     /*!< Not enough rights to use this command */
        eMissingArg,        /*!< A parameter is missing to use this command correctly */
        eNotExist,          /*!< The argument refers to a client/channel that doesn't exist */
        error               /*!< Other errors */
    };

}

/*! \namespace CMD
*
* Namespace gathering command codes refering to the commands
*/
namespace CMD {

/**
 * \enum
 * \brief Command code constants.
 */
    enum{
            C_PRIVMSG = 1,  /*!< Command private message */
            C_PUBMSG,       /*!< Command public message */
            C_GWHO,         /*!< Command general who */
            C_CWHO,         /*!< Command who on a channel */
            C_LIST,         /*!< Command list */
            C_TOPIC,        /*!< Command topic */
            C_KICK,         /*!< Command kick */
            C_BAN,          /*!< Command ban */
            C_OP,           /*!< Command op */
            C_DEOP = 20,    /*!< Command deop */
            C_JOIN,         /*!< Command join */
            C_NICK,         /*!< Command nick */
            C_LEAVE,        /*!< Command leave */
            C_UNBAN,        /*!< Command unban */
            C_BANLIST       /*!< Command banlist */
        };
}

class Command
{

/*! \class Command
* \brief Class representing an abstract command
*
* This class encapsulate a request as an object. It uses the pattern Command.
*/

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

class list : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_filter;
public:
    list(Client* sender, Frame &frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->list(m_sender, m_filter); }
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
    QString m_dest_channel, m_filter;
public:
    kick(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->kick(m_sender, m_dest_channel, m_filter); }
};

class ban : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_dest_channel, m_filter;
public:
    ban(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->ban(m_sender, m_dest_channel, m_filter); }
};

class unban : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_dest_channel, m_filter;
public:
    unban(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->unban(m_sender, m_dest_channel, m_filter); }
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

class op : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_dest_channel, m_dest_client;
public:
    op(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->op(m_sender, m_dest_channel, m_dest_client); }
};

class deop : public Command
{
private:
    Client* m_sender;
    Server* m_receiver;
    QString m_dest_channel, m_dest_client;
public:
    deop(Client* sender, Frame& frame);
    virtual quint8 verify();
    virtual quint8 execute() { return m_receiver->deop(m_sender, m_dest_channel, m_dest_client); }
};

#endif // COMMAND_H
