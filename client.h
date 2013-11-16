#ifndef CLIENT_H
#define CLIENT_H

/*!
 * \file client.h
 * \brief This file gathers informations on a client connected to the server using a TCP connection.
 */

#include <QTcpSocket>


class Channel;

/*! \class Client
  * \brief Class representing a Client connected to the server.
  * This class inherits from QObjet to use slots and signals
  */

class Client : public QObject
{
    Q_OBJECT

private:
    QTcpSocket *m_socket;       /*!< TCP communication socket of the client*/
    QString m_nickname;         /*!< Nickname of the client*/
    bool m_state;               /*!< Contains 0 if the client has not set his nickname yet; 1 else*/
    QString m_msg;              /*!< This attribute is usually set by the server in response to a command from the client
                                     It's QString depicting the result of the last command sent by the client*/

public:
    Client(QTcpSocket* socket, QObject *parent);
    ~Client();
    void setNickname(const QString &nickname );
    void setSocket(QTcpSocket* socket);
    void setState(bool state);
    QTcpSocket* getSocket(void) const;
    QString getNickname(void) const;
    QString getMsg(void) const;
    void setMsg(const QString &msg);
    bool getState(void) const;

public slots :
    void onDisconnection(void);
    void onDataReady();

};

#endif // CLIENT_H
