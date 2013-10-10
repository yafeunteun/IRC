//========================================================
//
// File: bdPlatformLog.cpp
// Initial Author: Molle Victorien
// Date: 30 Sept 2013
//
// Component: Call of Duty World at War - Dedicated server
// Sub-component: DemonWare SDK
//
// Purpose: A (nice) logging class
//
//========================================================

#include "bdPlatformLog.h"

///Constructor
bdPlatformLog::bdPlatformLog()
{
}

///Publish a message
///Params:
///LogLevel logLevel: the log level (NONE, WARNING, ERROR)
///const char *channel: unknown
///const char *pathtofile: the path to the file where the function was called
///const char *method: the method/function which called this method
///int line: the line of the file where the function was called
///const char *message: the message to log
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

///Log a message
///Params:
///LogLevel logLevel: the log level (NONE, WARNING, ERROR)
///const char *channel: unknown
///const char *pathtofile: the path to the file where the function was called
///const char *method: the method/function which called this method
///int line: the line of the file where the function was called
///...: the others parameters, always starting with the format of the output message
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
