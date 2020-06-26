#include "../include/log.h"

#include <stdio.h>


#define BUFFER_SIZE 64


int main(void)
{
    int r;

    char levelBuffer[BUFFER_SIZE];
    char timeFormatBuffer[BUFFER_SIZE];
    char startTimeBuffer[BUFFER_SIZE];

    /* Get default parameters as strings then output them */
    getLogLevelString(levelBuffer, getLogLevel(), sizeof(levelBuffer));
    getLogTimeFormatString(timeFormatBuffer, getLogTimeFormat(), sizeof(timeFormatBuffer));

    printf("\
Default logging parameters:\n\
  Verbosity      = %s\n\
  Level          = %s\n\
  Time format    = %s\n\
  Colour mode    = %s\n\n",
  (getLogVerbosity()) ? "VERBOSE" : "QUIET",
  levelBuffer,
  timeFormatBuffer,
  (getLogColourMode()) ? "Enabled" : "Disabled"
  );

    /* 
     * Enable logging to stdout (can be used in addition to a file)
     * Default = false
     */
    setLogVerbosity(true);

    /* 
     * Ignore messages less severe than the set level
     * Default = INFO
     */
    setLogLevel(DEBUG);

    logMessage(FATAL, "Use this for abrupt program exits that cannot clean up");
    logMessage(ERROR, "Messages for programs failures that exit and clean up memory");
    logMessage(WARNING, "When the program can continue execution but in a less-than-ideal state");
    logMessage(INFO, "For the logging of normal program usage");
    logMessage(DEBUG, "Use this for output not required by the typical end-user");

    /*
     * Remove coloured severity levels
     * Default = true
     */
    setLogColourMode(false);

    logMessage(INFO, "This message is void of colour");

    setLogColourMode(true);

    /*
     * Set the log message time format
     * Default = LOG_TIME_RFC3339
     */
    setLogTimeFormat(LOG_TIME_RELATIVE);

    /* Set the reference time for log messages to the current time */
    r = setLogReferenceTime();

    if (r == 0 && setLogStartTime() == 0)
    {
        putchar('\n');

        /* Get reference time (there is an alternative to return time_t) */
        getLogStartTimeString(startTimeBuffer, sizeof(startTimeBuffer));
        logMessage(INFO, "This message time is CPU time relative to %s...", startTimeBuffer);
        logMessage(INFO, "... and this one...");
        logMessage(INFO, "... and every message after it... (they might be 0 because the calls are quick)");
    }
    else
    {
        logMessage(WARNING, "Reference time could not be set");
    }

    putchar('\n');

    return 0;
}