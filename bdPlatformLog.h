#ifndef BDPLATFORMLOG_H
#define BDPLATFORMLOG_H

/*!
 * \file bdPlatformLog.h
 * \brief A (nice) logging class
 * \author Victorien MOLLE
 * \version 0.1
 */
#include <stdarg.h>
#include <stdio.h>

typedef enum
{ 
	_NONE, /*!< Defines a 'none' level. */
	_WARNING, /*!< Defines a 'warning' level. */
	_ERROR, /*!< Defines an 'error' level. */
	_DEBUG /*!< Defines a 'debug' level. */
}_LogLevel;

/*! \class bdPlatformLog
 * \brief The bdPlatformLog class
 *
 * This class write an output message specified by the developper
 * including more information such as the file name from where
 * the class is called, the line and the member function
 */
class bdPlatformLog
{
    public:
	/*!
	 * \brief Constructor
	 *
	 * The bdPlatformLog constructor
	 *
	 */
        bdPlatformLog();

	/*!
	 * \brief Publish a message
	 *
	 * Member function which publishes a new message
	 *
	 * \param logLevel: the log level (_NONE, _WARNING, _ERROR, _DEBUG)
	 * \param channel: unknown
	 * \param pathtofile: the path to the file from where the member function was called
	 * \param method: the method/function which called this method
	 * \param line: the line of the file where the function was called
	 * \param message: the message set by the developper
	 */
        static void publish( _LogLevel logLevel, const char *channel, const char *pathtofile, const char *method, int line, const char *message );

	/*!
	 * \brief Log a message
	 *
	 * Member function which logs a new message
	 *
	 * \param logLevel: the log level (_NONE, _WARNING, _ERROR, _DEBUG)
	 * \param channel: unknown
	 * \param pathtofile: the path to the file from where the member function was called
	 * \param method: the method/function which called this method
	 * \param line: the line of the file where the function was called
	 * \param ...: the others parameters, always starting with the format of the output message
	 */
        static void bdLogMessage( _LogLevel logLevel, const char *level, const char *channel, const char *pathtofile, const char *method, int line, ... );
};

#endif /* BDPLATFORMLOG_H */
