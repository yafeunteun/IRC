#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include "channel.h"

class Client
{
private:
    QTcpSocket *m_socket;
    QString m_nickname;
    bool m_state;

public:
    Client( QTcpSocket *socket);
    ~Client();

    void setNickname(QString nickname );
};

#endif // CLIENT_H
