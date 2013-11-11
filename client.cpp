#include "client.h"
#include "channel.h"
#include "command.h"


/********************
 *    CONSTRUCTOR   *
 ********************/

/*!
 *  \brief Constructor
 *  \param socket : the socket used to communicate with the server. Given by the server.
 *  \param parent : The adress of the Server instance. The client is binded to the server, when the server shuts down, the client is
 *  destroyed regularly by the application. It prevents problems like memory leak.
 */
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
/*!
*  \brief Destructeur
*   Frees up the memory allocated for the attribute m_socket.
*/
Client::~Client()
{
    delete m_socket;
}

/********************
 *      SLOTS       *
 ********************/

/*!
*  \brief Remove the client from the server list of clients when it has left the server.
*   SLOT connected to SIGNAL disconnected() of the QTcpSocket attribute.
*/
void Client::onDisconnection()
{
    Server* s = Server::Instance();
    s->delClient(this);
}

/*!
*  \Read data sent by the client program corresponding.
*
*  Read and execute a command sent by the client program corresponding to the instance of the client.
*  SLOT connected to SIGNAL readyRead() of the QTcpSocket attribute.
*/
void Client::onDataReady()
{

    QByteArray data = this->getSocket()->readAll();

    Frame frame = (data);

    if(frame.getCode() != CMD::C_NICK && this->getState() == 0)
    {
        this->m_msg = "You must have a nickname before ! Please use /nick <your nickname> without the '<>'";
        QByteArray response = Frame::getReadyToSendFrame(this->getMsg(), frame.getId(), ERROR::error);
        this->getSocket()->write(response);
        this->m_msg = "";                       // reset the message held by the client
        return;
    }

    quint8 ret_val = 0;

    Command* command = Command::getCommand(this, frame);

    if(command == NULL)
        return;

    ret_val = command->verify();

    if(ret_val == ERROR::esuccess)
    {
        ret_val = command->execute();
    }

    QByteArray response = Frame::getReadyToSendFrame(this->getMsg(), frame.getId(), ret_val);
    this->getSocket()->write(response);

    this->m_msg = "";           // reset the message held by the client

}

/********************
 *      GETTERS     *
 ********************/

/**
* Get the socket used by the client to communicate with the server.
*
* \return The address of the socket used by the client to communicate with the server.
*/
QTcpSocket* Client::getSocket(void) const
{
    return m_socket;
}

/**
* Get the nickname of the client
*
* \return A QString containing the nickname of the client.
*/
QString Client::getNickname(void) const
{
    return m_nickname;
}

/**
* Get the current state of the client
*
* \return A boolean containing true if the client has a nickname, false either.
*/
bool Client::getState(void) const
{
    return m_state;
}

/**
* Get the current message held by the client.
*
* \return A QString containing the current message held by the client.
*/
QString Client::getMsg(void) const
{
    return m_msg;
}

/********************
 *      SETTERS     *
 ********************/

/**
* Set the nickname of the client.
*
* \param nickname : A QString containing the new nickname for the client.
*/
void Client::setNickname(const QString &nickname )
{
    m_nickname = nickname;
}

/**
* Set the socket of the client.
*
* \param socket : A QTcpSocket pointer containing the address of the QTcpSocket for the client.
*/
void Client::setSocket(QTcpSocket *socket)
{
     m_socket = socket;
}

/**
* Set the state of the client.
*
* \param state : A boolean containing the new state of the client depending on wether it has set is nickname(true) or not(false).
*/
void Client::setState(bool state)
{
    m_state = state;
}

/**
* Set the message of the client.
*
* \param msg : A QString depicting the result of the last command sent by the client.
*/
void Client::setMsg(const QString& msg)
{
    m_msg = msg;
}

