#include "frame.h"
#include <iostream>


/*!
*  \brief Constructor
*
*  \param frame : A QByteArray containing the formatted frame received.
*/
Frame::Frame(QByteArray &frame)
{
    QDataStream *stream = new QDataStream(frame);
    stream->readRawData((char*)&m_size, 2);
    stream->readRawData((char*)&m_id, 2);
    stream->readRawData((char*)&m_code, 1);
    quint16 arg_size = (m_size) - 3;        // 2 bytes for id; 1 byte for command code
    char args[arg_size];
    stream->readRawData(args, arg_size);
    delete stream;

    args[arg_size] = '\0';

    m_argList = getArg(args);
    m_nbArg = m_argList.size();

}

/*!
*  \brief Allows to get a ready to send formatted frame.
*
*  Allows to get a ready to send formatted frame according to the pseudo-irc protocol.
*  depending on the parameters given.
*
*  \param data : The arguments of the command separated by a '\n' character.
*  \param id : The id of the command.
*  \param code : The code of the command (see Command.h for an exhaustive list).
*
*  \return The formatted frame according to the parameters given.
*/
QByteArray Frame::getReadyToSendFrame(QString &data, quint16 id, quint8 code)
{
    data = data.simplified();
    quint16 size = data.size()+3; // 2 for m_id; 1 for m_code;
    quint16 _id(id);
    quint8 _code(code);



    data[size-3] = QChar('\n');
    size++; // we just have added \n character


    QByteArray frame((char*)&size, 2);
    frame.append((char*)&_id, 2);
    frame.append((char*)&_code, 1);
    frame.append(data);

    return frame;

}


/*!
*  \brief Method to get a QStringList containing the parameters of a command.
*
*  Allows to get a QStringList from a c string (char*) containing the arguments of a command.
*
*  \param data : The ac string (char*) containing the arguments of a command (separated by '\n' character).

*  \return A QStringList containing the arguments of the command.
*/
QStringList Frame::getArg(char*  data)
{
    QString args(data);
    args.section('\n', 0);
    QStringList list = args.split('\n', QString::SkipEmptyParts);

    return list;
}

/*!
*  \return The size of the arguments of the command.
*/
quint16 Frame::getSize(void) const
{
    return m_size;
}

/*!
*  \return The Id of the command contained in the frame.
*/
quint16 Frame::getId(void) const
{
    return m_id;
}

/*!
*  \return The code of the command contained in the frame.
*/
quint8 Frame::getCode(void) const
{
    return m_code;
}

/*!
*  \return A QStringList containing the arguments of the command.
*/
QStringList Frame::getArgList(void) const
{
    return m_argList;
}

/*!
*  \return The number of arguments contained in the command.
*/
quint16 Frame::getNbArg(void) const
{
    return m_nbArg;
}


