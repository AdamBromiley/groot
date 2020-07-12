#ifndef LOG_H
#define LOG_H


#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>


typedef enum LogLevel
{
        LOG_NONE,
        FATAL,
        ERROR,
        WARNING,
        INFO,
        DEBUG
} LogLevel;

typedef enum LogTimeFormat
{
    LOG_TIME_NONE,
    LOG_TIME_RFC3339,
    LOG_TIME_RELATIVE
} LogTimeFormat;


extern const LogLevel LOG_LEVEL_MIN;
extern const LogLevel LOG_LEVEL_MAX;
extern const LogTimeFormat LOG_TIME_FORMAT_MIN;
extern const LogTimeFormat LOG_TIME_FORMAT_MAX;


void logMessage(LogLevel level, const char *format, ...);

int openLog(const char *filename);
int closeLog(void);

void setLogVerbosity(bool verbosity);
void setLogLevel(LogLevel level);
void setLogTimeFormat(LogTimeFormat format);
int setLogReferenceTime(void);
int setLogStartTime(void);
void setLogColourMode(bool mode);

bool getLogVerbosity(void);
LogLevel getLogLevel(void);
LogTimeFormat getLogTimeFormat(void);
time_t getLogStartTime(void);
bool getLogColourMode(void);

int getLogLevelString(char *dest, LogLevel level, size_t n);
int getLogTimeFormatString(char *dest, LogTimeFormat format, size_t n);


#endif