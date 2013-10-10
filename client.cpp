#include "client.h"

using namespace std;

Client::Client( QTcpSocket *remoteHost): address(remoteHost)
{
}

Client::~Client()
{
    cout << "Client " << nick << " disconnected." << endl;
}

void Client::setNick( const std::string& _nick )
{
    nick = _nick;
}
