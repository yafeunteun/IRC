#ifndef FRAME_H
#define FRAME_H

#include <QStringList>

class Frame
{
public:
    Frame(QByteArray &data);
    static QByteArray getReadyToSendFrame(QString data, quint16 id, quint8 code);
    quint16 getSize(void) const;
    quint16 getId(void) const;
    quint8 getCode(void) const;
    QStringList getArgList(void) const;
    quint16 getNbArg(void) const;
    void debug(void) const;

private:
    QStringList getArg(char *data);
    quint16 m_size;
    quint16 m_id;
    quint8 m_code;
    QStringList m_argList;
    quint8 m_nbArg;
};


#endif // FRAME_H
