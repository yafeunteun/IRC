#ifndef COMMAND_H
#define COMMAND_H

#include <QString>
#include <QStringList>
#include <server.h>

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
    nickCommand(Client* sender, QString nickname) : m_sender(sender), m_nickname(nickname){ m_receiver = Server::Instance();}
    virtual quint8 execute() { return m_receiver->nick(m_sender, m_nickname); }
};




#endif // COMMAND_H
