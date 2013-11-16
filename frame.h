#ifndef FRAME_H
#define FRAME_H

/*!
 * \file frame.h
 * \brief Frame analyser based on a Pseudo-Irc protocol.
 */

#include <QStringList>

class Frame
{

/*! \class Frame
* \brief This class provides tools for frame analysis
*
*  Frame allows to send formatted frames according to a Pseudo-Irc protocol from a simple string
*  plus tools for a complete analysis of the frames received on the Pseudo-Irc protocol compatible server.
*/

public:
    Frame(QByteArray &data);
    static QByteArray getReadyToSendFrame(QString &data, quint16 id, quint8 code);
    quint16 getSize(void) const;
    quint16 getId(void) const;
    quint8 getCode(void) const;
    QStringList getArgList(void) const;
    quint16 getNbArg(void) const;

private:
    QStringList getArg(char *data);     // see Frame.cpp
    quint16 m_size;                     /*!< The size of the data received */
    quint16 m_id;                       /*!< The id of the command contained in the data received */
    quint8 m_code;                      /*!< The code of the command contained in the data received */
    QStringList m_argList;              /*!< A list of the arguments passed to the command contained in the data received */
    quint8 m_nbArg;                     /*!< The number of arguments passed to the command contained in the data received*/
};


#endif // FRAME_H
