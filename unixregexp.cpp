#include "unixregexp.h"

UnixRegExp::UnixRegExp(const QString & pattern, Qt::CaseSensitivity cs)
    :QRegExp(pattern, cs, QRegExp::WildcardUnix )
{
}
