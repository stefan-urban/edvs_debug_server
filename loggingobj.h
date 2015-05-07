#ifndef LOGGINGOBJ_H
#define LOGGINGOBJ_H

#include <string>

class LoggingObj
{
public:
    LoggingObj(char coord_x, char coord_y, bool polarity, unsigned int timestamp_us);
    char* out();
private:
    /**
     * @brief X coordinate
     */
    char x;

    /**
     * @brief Y coordinate
     */
    char y;

    /**
     * @brief Polarity
     */
    char p;

    /**
     * @brief Timestamp in microseconds
     */
    unsigned int timestamp;
};

#endif // LOGGINGOBJ_H
