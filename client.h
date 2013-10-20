#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include "channel.h"

class Client : public QObject       // this class inherits from QObjet to use slots and signals
{
    Q_OBJECT
private:
    QTcpSocket *m_socket;       // TCP socket of the client
    QString m_nickname;         // nickname of the client
    bool m_state;               // 0 the client has not set his nickname yet; 1 else

public:
    Client(QTcpSocket* socket, QObject *parent);       // constrtuctor
    ~Client();                                          // destructor
    void setNickname(QString &nickname );                // setter fot nickname
    void setSocket(QTcpSocket* socket);
    void setState(bool state);
    QTcpSocket* getSocket(void) const;                        // getter for socket
    QString getNickname(void) const;                          // getter for nickname
    bool getState(void) const;

public slots :
    void onDisconnection(void);                         // slot connected to SIGNAL disconnected of the QTcpSocket member of instance.
    void onDataReady();                             // slot connected to SIGNAL readyRead of the QTcpSocket member of instance.

};

#endif // CLIENT_H
