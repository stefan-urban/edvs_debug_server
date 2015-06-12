#ifndef LOGGER_H
#define LOGGER_H

#include <string>

#include "loggingobj.h"

#define MAX_LOGGING_OBJECTS (512)

class Logger
{
public:
    /**
     * @brief Logger
     */
    Logger(std::string file);

    /**
     * @brief Adds a logging object to the current buffer
     * @param obj
     */
    void addObj(LoggingObj *obj);

    /**
     * @brief Sets the number of objects after which all buffered objects are written to the log file
     * @param maxObjs
     */
    void setThresold(int maxObjs);
private:
    int thres;
    std::string logfile;

    LoggingObj *buffer[MAX_LOGGING_OBJECTS];
    int bufferSize;
};

#endif // LOGGER_H
