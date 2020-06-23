#ifndef LOG_H
#define LOG_H


#include <stdbool.h>
#include <stddef.h>


enum LogSeverity
{
        LOG_NONE,
        FATAL,
        ERROR,
        WARNING,
        INFO,
        DEBUG
};


extern const enum LogSeverity LOG_SEVERITY_MIN;
extern const enum LogSeverity LOG_SEVERITY_MAX;


void logMessage(enum LogSeverity level, const char *format, ...);

int openLog(const char *filename);
int closeLog(void);

void setLogVerbosity(bool verbosity);
bool getLogVerbosity(void);
void setLogLevel(enum LogSeverity level);
enum LogSeverity getLogLevel(void);

void getSeverityString(char *dest, enum LogSeverity severity, size_t n);


#endif