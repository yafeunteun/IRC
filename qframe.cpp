#include "qframe.h"


quint16 QFrame::getFrameSize(QString const &data)
{
    QString size = data.mid(0, 2);

    return size.toUInt();
}
quint16 QFrame::getCmdId(QString const &data)
{
    QString cmdId = data.mid(2, 2);

    return cmdId.toUInt();
}

quint8 QFrame::getCmdCode(QString const &data)
{
    QString cmdCode = data.mid(4, 1);

    return cmdCode.toUInt();
}

QStringList QFrame::getArg(QString  &data, qint8 start, qint8 end)
{
    data = data.replace("\n", " ").trimmed();
    QString args = data.mid(5);
    args.section(" ", start, end);                              // The string consists of the fields from position start to position end inclusive. If end is not specified, all fields from position start to the end of the string are included. Fields are numbered 0, 1, 2, etc., counting from the left, and -1, -2, etc., counting from right to left.
    QStringList list = args.split(" ", QString::SkipEmptyParts);

    return list;
}
