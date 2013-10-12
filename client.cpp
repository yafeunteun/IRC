#include "client.h"


Client::Client( QTcpSocket* socket)
{
    m_socket = socket;
    m_state = 0;
}

Client::~Client()
{
}

void Client::setNickname( QString nickname )
{
    m_nickname = nickname;
}
