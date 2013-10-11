#include "bdPlatformLog.h"

bdPlatformLog::bdPlatformLog()
{
}

void bdPlatformLog::publish( _LogLevel logLevel, const char *channel, const char *pathtofile, const char *method, int line, const char *message )
{
	char buffer[0x800];

	switch (logLevel)
	{
		case _WARNING:
			if (sprintf(buffer, "[%s]%s(%u): %s - %s\n", "WARNING", pathtofile, line, method, message) > 0x800)
                bdLogMessage(_WARNING, "warn/", "bdPlatformLog", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Log message truncated.");

            puts(buffer);
			break;

		case _ERROR:
			if (sprintf(buffer, "[%s]%s(%u): %s - %s\n", "ERROR", pathtofile, line, method, message) > 0x800)
                bdLogMessage(_WARNING, "warn/", "bdPlatformLog", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Log message truncated.");

            fputs(buffer, stderr);
			break;

		case _DEBUG:
			if (sprintf(buffer, "[%s]%s(%u): %s - %s\n", "DEBUG", pathtofile, line, method, message) > 0x800)
                bdLogMessage(_WARNING, "warn/", "bdPlatformLog", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Log message truncated.");

            puts(buffer);
			break;

		default:
			if (sprintf(buffer, "%s(%u): %s - %s\n", pathtofile, line, method, message) > 0x800)
                bdLogMessage(_WARNING, "warn/", "bdPlatformLog", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Log message truncated.");

            puts(buffer);
			break;
	}
}

void bdPlatformLog::bdLogMessage( _LogLevel logLevel, const char *level, const char *channel, const char *pathtofile, const char *method, int line, ... )
{
	char message[0x100], channelName[0x100];
	char *format;
	va_list vl;

	va_start(vl, line);
	format = va_arg(vl, char*);

	if (vsprintf(message, format, vl) > 0x100)
        bdLogMessage(_WARNING, "warn/", "bdPlatformLog", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Message truncated.");
	va_end(vl);

	if (sprintf(channelName, "%s%s", level, channel) > 0x100)
        bdLogMessage(_WARNING, "warn/", "bdPlatformLog", __FILE__, __PRETTY_FUNCTION__, __LINE__, "Channel name truncated %s%s", level, channel);

    bdPlatformLog::publish(logLevel, channelName, pathtofile, method, line, message);
}
