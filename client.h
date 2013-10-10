#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include "channel.h"

class Client : public  QTcpSocket
{
public:
    Client();
};

#endif // CLIENT_H
