#include "client.h"
#include "server.h"
#include "bdPlatformLog.h"
#include "command.h"
#include "qframe.h"

using namespace CMD;

/********************
 *    CONSTRUCTOR   *
 ********************/
Client::Client(QTcpSocket *socket, QObject *parent) : QObject(parent)
{
    m_socket = socket;
    m_state = 0;
    QObject::connect(m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnection()));
    QObject::connect(m_socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    QObject::connect(m_socket, SIGNAL(readyRead()), this, SLOT(onDataReady()));
}

/********************
 *    DESTRUCTOR    *
 ********************/
Client::~Client()
{
    delete m_socket;
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Client has been destroyed !");
}

/********************
 *      SLOTS       *
 ********************/
void Client::onDisconnection()
{
    Server* s = Server::Instance();
    s->delClient(this);
}

void Client::onDataReady()
{
    QString data = QString::fromUtf8(this->getSocket()->readAll());
    quint8 codeCmd = QFrame::getCmdCode(data);
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Frame size : %u ", QFrame::getFrameSize(data));
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Command ID : %u", QFrame::getCmdId(data));
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Command code : %u", QFrame::getCmdCode(data));



/*
    switch(codeCmd){
    case NICK_CMD:
        nickCommand cmd(this, QFrame::getArg(data, 0, 0));
        quint8 ret_val = cmd.execute();
        if(ret_val == 0 )
            bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Nickname has been changed successfully !!! ");
        break;
    }
*/
}

/********************
 *      GETTERS     *
 ********************/
QTcpSocket* Client::getSocket(void) const
{
    return m_socket;
}

QString Client::getNickname(void) const
{
    return m_nickname;
}

bool Client::getState(void) const
{
    return m_state;
}

/********************
 *      SETTERS     *
 ********************/
void Client::setNickname( QString& nickname )
{
    m_nickname = nickname;
}

void Client::setSocket(QTcpSocket *socket)
{
     m_socket = socket;
}

void Client::setState(bool state)
{
    m_state = state;
}
