#include "log.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


#define LOG_TIME_STR_LEN_MAX 32
#define LOG_SEVERITY_STR_LEN_MAX 24
#define LOG_MSG_LEN_MAX 256
#define LOG_ENTRY_LEN_MAX 512

#define RESET_COLOUR "\033[0m"
#define RED_BG_TXT "\033[37;41m"
#define RED_TXT "\033[31m"
#define YELLOW_TXT "\033[33m"
#define GREEN_TXT "\033[32m"
#define CYAN_TXT "\033[36m"


/* Range of enums for user-input checking */
const enum LogLevel LOG_SEVERITY_MIN = LOG_NONE;
const enum LogLevel LOG_SEVERITY_MAX = DEBUG;
const enum LogTimeFormat LOG_TIME_FORMAT_MIN = LOG_TIME_NONE;
const enum LogTimeFormat LOG_TIME_FORMAT_MAX = LOG_TIME_RELATIVE;


/* Global log context, filled with default parameters */
static struct LogParameters ctx =
{
    .log = NULL,
    .verbose = false,
    .level = INFO,
    .time = LOG_TIME_RFC3339,
    .colour = true
};


static void getTime(char *dest, enum LogTimeFormat format, size_t n);


/* Write to log */
void logMessage(enum LogLevel level, const char *format, ...)
{
    /* Full log entry */
    char logEntry[LOG_ENTRY_LEN_MAX + 1];

    /* Log entry components */
    char timeString[LOG_TIME_STR_LEN_MAX + 1];
    char severityString[LOG_SEVERITY_STR_LEN_MAX + 1];
    char message[LOG_MSG_LEN_MAX + 1];

    va_list formatArguments;
    
    /* Ignore if there's nowhere to log to */
    if (!ctx.log && !ctx.verbose)
        return;

    /* Ignore if message not severe enough for the chosen logging level */
    if (ctx.level < level || ctx.level == LOG_NONE)
        return;

    getTime(timeString, ctx.time, sizeof(timeString));
    getLogLevelString(severityString, level, sizeof(severityString));

    /* Read all arguments to logMessage() after the format string */
    va_start(formatArguments, format);
    vsnprintf(message, sizeof(message), format, formatArguments);
    va_end(formatArguments);

    /* Construct log message */
    snprintf(logEntry, sizeof(logEntry), "%s %s %s\n", timeString,
        severityString, message);

    /* Write to log */
    if (ctx.log)
        fprintf(ctx.log, "%s", logEntry);

    if (ctx.verbose)
        fprintf(stderr, "%s", logEntry);

    return;
}


/* Open logging file */
int openLog(const char *filename)
{
    if (!filename)
        return 1;
    
    ctx.log = fopen(filename, "a");

    if (!ctx.log)
        return 1;

    return 0;
}


/* Close logging file */
int closeLog(void)
{
    int r;

    if (!ctx.log)
        return 1;
    
    r = fclose(ctx.log);
    ctx.log = NULL;

    if (r)
        return 1;

    return 0;
}


/* Enable/disable logging to stderr */
void setLogVerbosity(bool verbosity)
{
    ctx.verbose = verbosity;    
    return;
}


/* Get logging verbosity state */
bool getLogVerbosity(void)
{
    return ctx.verbose;
}


/* Set the minimum logging level */
void setLogLevel(enum LogLevel level)
{
    if (level < LOG_SEVERITY_MIN)
        ctx.level = LOG_SEVERITY_MIN;
    else if (level > LOG_SEVERITY_MAX)
        ctx.level = LOG_SEVERITY_MAX;
    else
        ctx.level = level;

    return;
}


/* Getting the minimum logging level */
enum LogLevel getLogLevel(void)
{
    return ctx.level;
}


/* Set the time string format */
void setLogTimeFormat(enum LogTimeFormat format)
{
    if (format < LOG_TIME_FORMAT_MIN)
        ctx.time = LOG_TIME_FORMAT_MIN;
    else if (format > LOG_TIME_FORMAT_MAX)
        ctx.time = LOG_TIME_FORMAT_MAX;
    else
        ctx.time = format;

    return;
}


/* Get the time string format */
enum LogTimeFormat getLogTimeFormat(void)
{
    return ctx.time;
}


/* Set the reference time for relative log time values */
int setLogReferenceTime(void)
{
    ctx.referenceTicks = clock();
    
    if (ctx.referenceTicks == (clock_t) -1)
        return 1;

    return 0;
}


/* Set a time value in the global context */
int setLogStartTime(void)
{
    ctx.startTime = time(NULL);
    
    if (ctx.startTime == (time_t) -1)
        return 1;

    return 0;
}


/* Get the time value from the context */
time_t getLogStartTime(void)
{
    return ctx.startTime;
}


/* Set the log colouring mode */
void setLogColourMode(bool mode)
{
    ctx.colour = mode;
    return;
}


/* Get the log colouring mode */
bool getLogColourMode(void)
{
    return ctx.colour;
}


/* Convert LogLevel enum to a string */
void getLogLevelString(char *dest, enum LogLevel level, size_t n)
{
    const char *src;

    switch (level)
    {
        case LOG_NONE:
            src = "NONE    ";
            break;
        case DEBUG:
            src = (ctx.colour) ? CYAN_TXT "DEBUG   " RESET_COLOUR : "DEBUG   ";
            break;
        case INFO:
            src = (ctx.colour) ? GREEN_TXT "INFO    " RESET_COLOUR : "INFO    ";
            break;
        case WARNING:
            src = (ctx.colour) ? YELLOW_TXT "WARNING " RESET_COLOUR : "WARNING ";
            break;
        case ERROR:
            src = (ctx.colour) ? RED_TXT "ERROR   " RESET_COLOUR : "ERROR   ";
            break;
        case FATAL:
            src = (ctx.colour) ? RED_BG_TXT "FATAL   " RESET_COLOUR : "FATAL   ";
            break;
        default:
            src = "NONE    ";
            break;
    }

    strncpy(dest, src, n);
    dest[n - 1] = '\0';

    return;
}


/* Convert LogTimeFormat enum to a string */
void getLogTimeFormatString(char *dest, enum LogTimeFormat format, size_t n)
{
    const char *src;

    switch (format)
    {
        case LOG_TIME_NONE:
            src = "-";
            break;
        case LOG_TIME_RFC3339:
            src = "RFC 3339";
            break;
        case LOG_TIME_RELATIVE:
            src = "Relative";
            break;
        default:
            src = "-";
            break;
    }

    strncpy(dest, src, n);
    dest[n - 1] = '\0';

    return;
}


/* Convert reference time to a string */
void getLogStartTimeString(char *dest, size_t n)
{
    struct tm *t = localtime(&(ctx.startTime));

    if (t)
        strftime(dest, n, "[%Y-%m-%d %H:%M:%S]", t);
    else
        strncpy(dest, "-", n);
    
    dest[n - 1] = '\0';

    return;
}


/* Get time string for log */
static void getTime(char *dest, enum LogTimeFormat format, size_t n)
{
    time_t currentTime;
    struct tm *currentTimeStruct;

    clock_t currentTicks;
    float elapsedTime;

    /* Set in case of an error (just print a blank time) */
    dest[0] = '\0';

    switch (format)
    {
        case LOG_TIME_NONE:
            return;
        case LOG_TIME_RFC3339:
            /* Get date and time in RFC 3339 format - YYYY-MM-DD hh:mm:ss */
            currentTime = time(NULL);
            currentTimeStruct = localtime(&currentTime);
            if (currentTimeStruct)
                strftime(dest, n, "[%Y-%m-%d %H:%M:%S]", currentTimeStruct);
            break;
        case LOG_TIME_RELATIVE:
            /* Get padded seconds since reference-time set */
            currentTicks = clock();
            elapsedTime = (float) (currentTicks - ctx.referenceTicks) / CLOCKS_PER_SEC;
            snprintf(dest, n, "[%-10f]", elapsedTime);
            break;
        default:
            return;
    }
    
    dest[n - 1] = '\0';

    return;
}