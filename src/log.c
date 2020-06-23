#include "log.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


#define LOG_TIME_STR_LEN_MAX 32
#define LOG_SEVERITY_STR_LEN_MAX 16
#define LOG_MSG_LEN_MAX 256
#define LOG_ENTRY_LEN_MAX 512


/* Log severity range for user-input error checking */
const enum LogSeverity LOG_SEVERITY_MIN = LOG_NONE;
const enum LogSeverity LOG_SEVERITY_MAX = DEBUG;


/* Log file */
static FILE *logfile = NULL;

/* Logging modes */
static bool verbose = 0;
static enum LogSeverity loggingLevel = INFO;


static void getRFC3339Time(char *dest, size_t n);


/* Write to log */
void logMessage(enum LogSeverity level, const char *format, ...)
{
    /* Full log entry */
    char logEntry[LOG_ENTRY_LEN_MAX + 1];

    /* Log entry components */
    char timeString[LOG_TIME_STR_LEN_MAX + 1];
    char severityString[LOG_SEVERITY_STR_LEN_MAX + 1];
    char message[LOG_MSG_LEN_MAX + 1];

    va_list formatArguments;
    
    /* Ignore if there's nowhere to log to */
    if (!logfile && !verbose)
        return;

    /* Ignore if message not severe enough for the chosen logging level */
    if (loggingLevel < level || loggingLevel == LOG_NONE)
        return;

    getRFC3339Time(timeString, sizeof(timeString));
    getSeverityString(severityString, level, sizeof(severityString));

    /* Read all arguments to logMessage() after the format string */
    va_start(formatArguments, format);
    vsnprintf(message, sizeof(message), format, formatArguments);
    va_end(formatArguments);

    /* Construct log message */
    snprintf(logEntry, sizeof(logEntry), "[%s] %-*s %s\n", timeString, LOG_SEVERITY_STR_LEN_MAX, severityString, message);

    /* Write to log */
    if (logfile)
        fprintf(logfile, "%s", logEntry);

    if (verbose)
        fprintf(stderr, "%s", logEntry);

    return;
}


/* Open logging file */
int openLog(const char *filename)
{
    if (!filename)
        return 1;
    
    logfile = fopen(filename, "a");

    if (!logfile)
        return 1;

    return 0;
}


/* Close logging file */
int closeLog(void)
{
    int r;

    if (!logfile)
        return 1;
    
    r = fclose(logfile);
    logfile = NULL;

    if (r)
        return 1;

    return 0;
}


/* Enable/disable logging to stderr */
void setLogVerbosity(bool verbosity)
{
    if (verbosity)
        verbose = 1;
    else
        verbose = 0;
    
    return;
}


/* Get logging verbosity state */
bool getLogVerbosity(void)
{
    return verbose;
}


/* Set the minimum logging level */
void setLogLevel(enum LogSeverity level)
{
    if (level < LOG_SEVERITY_MIN)
        loggingLevel = LOG_SEVERITY_MIN;
    else if (level > LOG_SEVERITY_MAX)
        loggingLevel = LOG_SEVERITY_MAX;
    else
        loggingLevel = level;
    return;
}


/* Getting the minimum logging level */
enum LogSeverity getLogLevel(void)
{
    return loggingLevel;
}


/* Convert severity level enum to a string */
void getSeverityString(char *dest, enum LogSeverity severity, size_t n)
{
    const char *src;

    switch (severity)
    {
        case LOG_NONE:
            src = "NONE";
            break;
        case DEBUG:
            src = "DEBUG";
            break;
        case INFO:
            src = "INFO";
            break;
        case WARNING:
            src = "WARNING";
            break;
        case ERROR:
            src = "ERROR";
            break;
        case FATAL:
            src = "FATAL";
            break;
        default:
            src = "NONE";
            break;
    }

    strncpy(dest, src, n);
    dest[n - 1] = '\0';

    return;
}


/* Get date and time in RFC 3339 format - YYYY-MM-DD hh:mm:ss */
static void getRFC3339Time(char *dest, size_t n)
{
    time_t currentTime = time(NULL);
    struct tm *currentTimeStruct = localtime(&currentTime);

    strftime(dest, n, "%Y-%m-%d %H:%M:%S", currentTimeStruct);
    dest[n - 1] = '\0';

    return;
}