#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include "channel.h"
#include <iostream>

class Client : public  QTcpSocket
{
private:
    QTcpSocket *address;
    std::string nick;

public:
    Client( QTcpSocket *remoteHost );
    ~Client();

    void setNick( const std::string& _nick );
};

#endif // CLIENT_H
