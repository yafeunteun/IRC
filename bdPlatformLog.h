#ifndef BDPLATFORMLOG_H
#define BDPLATFORMLOG_H

#include <stdarg.h>
#include <stdio.h>

typedef enum { _NONE, _WARNING, _ERROR, _DEBUG }_LogLevel;

class bdPlatformLog
{
    public:
        bdPlatformLog();

        static void publish( _LogLevel logLevel, const char *channel, const char *pathtofile, const char *method, int line, const char *message );
        static void bdLogMessage( _LogLevel logLevel, const char *level, const char *channel, const char *pathtofile, const char *method, int line, ... );

    protected:
    private:
};

#endif /* BDPLATFORMLOG_H */
