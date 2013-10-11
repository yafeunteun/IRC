#include "server.h"
#include "bdPlatformLog.h"


int main(void)
{
    bdPlatformLog::bdLogMessage(_ERROR, "err/", "main", __FILE__, __PRETTY_FUNCTION__, __LINE__, "This is an error message!");
    bdPlatformLog::bdLogMessage(_WARNING, "warn/", "main", __FILE__, __PRETTY_FUNCTION__, __LINE__, "This is a warning message!");
    bdPlatformLog::bdLogMessage(_DEBUG, "debug/", "main", __FILE__, __PRETTY_FUNCTION__, __LINE__, "This is a debug message!");
    bdPlatformLog::bdLogMessage(_NONE, "none/", "main", __FILE__, __PRETTY_FUNCTION__, __LINE__, "This is a normal message!");

    Server myserver;
    return 0;
}
