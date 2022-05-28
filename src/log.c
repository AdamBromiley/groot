#include "log.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


/* Output buffer sizes */
#define LOG_TIME_STR_LEN_MAX 32
#define LOG_SEVERITY_STR_LEN_MAX 24
#define LOG_MSG_STR_LEN_MAX 256
#define LOG_ENTRY_STR_LEN_MAX 512

/* Colour escape sequences */
#define RESET_COLOUR "\033[0m"
#define RED_BG_TXT "\033[37;41m"
#define RED_TXT "\033[31m"
#define YELLOW_TXT "\033[33m"
#define GREEN_TXT "\033[32m"
#define CYAN_TXT "\033[36m"


typedef struct LogCTX
{
    FILE *log;
    bool verbose;
    LogLevel level;
    LogTimeFormat time;
    time_t startTime;
    clock_t referenceTicks;
    bool colour;
} LogCTX;


/* Range of enums for user-input checking */
const LogLevel LOG_LEVEL_MIN = LOG_NONE;
const LogLevel LOG_LEVEL_MAX = DEBUG;
const LogTimeFormat LOG_TIME_FORMAT_MIN = LOG_TIME_NONE;
const LogTimeFormat LOG_TIME_FORMAT_MAX = LOG_TIME_RELATIVE;


/* Global log context, filled with default parameters */
static LogCTX ctx =
{
    .log = NULL,
    .verbose = false,
    .level = INFO,
    .time = LOG_TIME_RFC3339,
    .colour = true
};


static void getTime(char *dest, LogTimeFormat format, size_t n);


/* Write to log */
void logMessage(LogLevel level, const char *format, ...)
{
    /* Number of spaces to right-pad message severity level */
    const int LOG_LEVEL_STR_PADDING = 8;

    /* Full log entry */
    char logEntry[LOG_ENTRY_STR_LEN_MAX];

    /* Log entry components */
    char timeString[LOG_TIME_STR_LEN_MAX];
    char severityString[LOG_SEVERITY_STR_LEN_MAX];
    char message[LOG_MSG_STR_LEN_MAX];

    va_list formatArguments;
    
    /* Ignore if there's nowhere to log to */
    if (!ctx.log && !ctx.verbose)
        return;

    /* Ignore if message not severe enough for the chosen logging level */
    if (ctx.level < level || ctx.level == LOG_NONE)
        return;

    getTime(timeString, ctx.time, sizeof(timeString));

    if (getLogLevelString(severityString, level, sizeof(severityString)))
    {
        strncpy(severityString, "NONE", sizeof(severityString));
        severityString[sizeof(severityString) - 1] = '\0';
    }

    /* Read all arguments to logMessage() after the format string */
    va_start(formatArguments, format);
    vsnprintf(message, sizeof(message), format, formatArguments);
    va_end(formatArguments);

    /* Construct log message */
    snprintf(logEntry, sizeof(logEntry), "%s %-*s %s\n", timeString, LOG_LEVEL_STR_PADDING, severityString, message);

    /* Write to log */
    if (ctx.log)
        fprintf(ctx.log, "%s", logEntry);

    if (ctx.verbose)
        fprintf(stderr, "%s", logEntry);
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
    int r = 0;

    if (ctx.log)
        r = fclose(ctx.log);

    ctx.log = NULL;

    return (r) ? 1 : 0;
}


/* Enable/disable logging to stderr */
void setLogVerbosity(bool verbosity)
{
    ctx.verbose = verbosity;
}


/* Get logging verbosity state */
bool getLogVerbosity(void)
{
    return ctx.verbose;
}


/* Set the minimum logging level */
void setLogLevel(LogLevel level)
{
    if (level < LOG_LEVEL_MIN)
        ctx.level = LOG_LEVEL_MIN;
    else if (level > LOG_LEVEL_MAX)
        ctx.level = LOG_LEVEL_MAX;
    else
        ctx.level = level;
}


/* Getting the minimum logging level */
LogLevel getLogLevel(void)
{
    return ctx.level;
}


/* Set the time string format */
void setLogTimeFormat(LogTimeFormat format)
{
    if (format < LOG_TIME_FORMAT_MIN)
        ctx.time = LOG_TIME_FORMAT_MIN;
    else if (format > LOG_TIME_FORMAT_MAX)
        ctx.time = LOG_TIME_FORMAT_MAX;
    else
        ctx.time = format;
}


/* Get the time string format */
LogTimeFormat getLogTimeFormat(void)
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
}


/* Get the log colouring mode */
bool getLogColourMode(void)
{
    return ctx.colour;
}


/* Convert LogLevel enum to a string */
int getLogLevelString(char *dest, LogLevel level, size_t n)
{
    const char *src;

    switch (level)
    {
        case LOG_NONE:
            src = "NONE";
            break;
        case DEBUG:
            src = (ctx.colour) ? CYAN_TXT "DEBUG" RESET_COLOUR : "DEBUG";
            break;
        case INFO:
            src = (ctx.colour) ? GREEN_TXT "INFO" RESET_COLOUR : "INFO";
            break;
        case WARNING:
            src = (ctx.colour) ? YELLOW_TXT "WARNING" RESET_COLOUR : "WARNING";
            break;
        case ERROR:
            src = (ctx.colour) ? RED_TXT "ERROR" RESET_COLOUR : "ERROR";
            break;
        case FATAL:
            src = (ctx.colour) ? RED_BG_TXT "FATAL" RESET_COLOUR : "FATAL";
            break;
        default:
            return 1;
    }

    strncpy(dest, src, n);
    dest[n - 1] = '\0';

    return 0;
}


/* Convert LogTimeFormat enum to a string */
int getLogTimeFormatString(char *dest, LogTimeFormat format, size_t n)
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
            return 1;
    }

    strncpy(dest, src, n);
    dest[n - 1] = '\0';

    return 0;
}


/* Get time string for log */
static void getTime(char *dest, LogTimeFormat format, size_t n)
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
}