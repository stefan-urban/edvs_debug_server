#include <stdio.h>

#include "loggingobj.h"

LoggingObj::LoggingObj(char coord_x, char coord_y, bool polarity, unsigned int timestamp_us)
{
    this->x = coord_x;
    this->y = coord_y;
    this->p = polarity;
    this->timestamp = timestamp_us;
}

char* LoggingObj::out()
{
    char buf[100];

    sprintf(buf, "%3d %3d %d %10d", this->x, this->y, this->p, this->timestamp);

    return buf;
}
