#include "server.h"
#include "bdPlatformLog.h"


int main(void)
{
    Server* s1 = Server::Instance();
    Server* s2 = Server::Instance();
    Server* s3 = Server::Instance();
    Server* s4 = Server::Instance();

    return 0;
}
