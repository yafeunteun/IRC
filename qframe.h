#ifndef QFRAME_H
#define QFRAME_H

#include <QString>
#include <QStringList>

class QFrame
{
public:
    //void setFrameSize(quint16 size);
    //void setCmdId(quint16 id);
    //void setCmdCode(quint8 code);
    static quint16 getFrameSize(QString const &data);
    static quint16 getCmdId(QString const &data);
    static quint8 getCmdCode(QString const &data);
    static QStringList getArg(QString const &data, qint8 start, qint8 end = -1);
};

#endif // QFRAME_H
