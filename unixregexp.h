#ifndef UNIXREGEXP_H
#define UNIXREGEXP_H

#include <QRegExp>

class UnixRegExp : public QRegExp
{
public:
    UnixRegExp(const QString &pattern, Qt::CaseSensitivity cs = Qt::CaseSensitive);
};

#endif // UNIXREGEXP_H
