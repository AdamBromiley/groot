#ifndef LOG_H
#define LOG_H


#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>


enum LogLevel
{
        LOG_NONE,
        FATAL,
        ERROR,
        WARNING,
        INFO,
        DEBUG
};

enum LogTimeFormat
{
    LOG_TIME_NONE,
    LOG_TIME_RFC3339,
    LOG_TIME_RELATIVE
};

struct LogParameters
{
    FILE *log;
    bool verbose;
    enum LogLevel level;
    enum LogTimeFormat time;
    time_t startTime;
    clock_t referenceTicks;
    bool colour;
};


extern const enum LogLevel LOG_SEVERITY_MIN;
extern const enum LogLevel LOG_SEVERITY_MAX;

extern const enum LogTimeFormat LOG_TIME_FORMAT_MIN;
extern const enum LogTimeFormat LOG_TIME_FORMAT_MAX;


void logMessage(enum LogLevel level, const char *format, ...);

int openLog(const char *filename);
int closeLog(void);

void setLogVerbosity(bool verbosity);
void setLogLevel(enum LogLevel level);
void setLogTimeFormat(enum LogTimeFormat format);
int setLogReferenceTime(void);
int setLogStartTime(void);
void setLogColourMode(bool mode);

bool getLogVerbosity(void);
enum LogLevel getLogLevel(void);
enum LogTimeFormat getLogTimeFormat(void);
time_t getLogStartTime(void);
bool getLogColourMode(void);

void getLogLevelString(char *dest, enum LogLevel level, size_t n);
void getLogTimeFormatString(char *dest, enum LogTimeFormat format, size_t n);
void getLogStartTimeString(char *dest, size_t n);


#endif