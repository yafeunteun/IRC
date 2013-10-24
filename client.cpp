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
    quint8 codeCmd = QFrame::getCmdCode(data), ret_val;

    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Frame size : %u ", QFrame::getFrameSize(data));
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Command ID : %u", QFrame::getCmdId(data));
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Command code : %u", QFrame::getCmdCode(data));


    switch(codeCmd){
    case NICK_CMD:
        nickCommand cmd(this, QFrame::getArg(data, 0, 0));
        quint8 ret_val = cmd.execute();
        if(ret_val == 0 )
            bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Nickname has been changed successfully !!! ");
        break;
    }

  // This part has been commented to compile and run properly
/*
    switch(codeCmd)
    {
        case NICK_CMD:
        {
            nickCommand cmd(this, QFrame::getArg(data, 0, 0));
            ret_val = cmd.execute();
            if(ret_val == 0)
                bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Nickname has been changed successfully !!! ");
            break;
        }
        case CLMSGCL_CMD:
        {
            pmCommand cmd(this, QFrame::getArg(data, 0, 0), QFrame::getArg(data, 1));
            ret_val = cmd.execute();
            if(ret_val == 0)
                bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Successfully sent private message!");
            break;
        }
        case CLMSGCH_CMD:
        {
            chCommand cmd(this, QFrame::getArg(data, 0, 0), QFrame::getArg(data, 1));
            ret_val = cmd.execute();
            if(ret_val == 0)
                bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Successfully sent message to the channel!");
            break;
        }
        case JOIN_CMD:
        {
            joinCommand cmd(this, QFrame::getArg(data, 0, 0));
            ret_val = cmd.execute();
            if(ret_val == 0)
                bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Successfully joined channel!");
            if(ret_val == 1)
                bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Successfully created channel!");
            break;
        }
        case LEAVE_CMD:
        {
            leaveCommand cmd(this, QFrame::getArg(data, 0, 0));
            ret_val = cmd.execute();
            if(ret_val == 0)
                bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Successfully left channel!");
            break;
        }
        case LIST_CMD:
        {
            listCommand cmd(this, QFrame::getArg(data, 0, 0));
            ret_val = cmd.execute(); //This operation never fails
            break;
        }
        case TOPIC_CMD:
        {
            topicCommand cmd(this, QFrame::getArg(data, 0, 0), QFrame::getArg(data, 1));
            ret_val = cmd.execute();
            if (ret_val == 0)
                bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Successfully modified topic!");
               break;
        }
        case WHOG_CMD:
        {
            whoGCommand cmd(this, QFrame::getArg(data, 0, 0));
            ret_val = cmd.execute(); //This command should never fail
            break;
        }
        case WHOC_CMD:
        {
            whoCCommand cmd(this, QFrame::getArg(data, 0, 0));
            ret_val = cmd.execute(); //This command should never fail
            break;
        }
        default:
        {
            bdPlatformLog::bdLogMessage(_WARNING, "warn/", "client", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Received unknown command ident %u.", codeCmd);
            break;
        }
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
