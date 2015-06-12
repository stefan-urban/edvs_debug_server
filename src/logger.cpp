#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <string>
#include <iostream>

#include "logger.h"
#include "loggingobj.h"

Logger::Logger(std::string file)
{
    file.append(".dvs");
    this->logfile = file;

    this->bufferSize = 0;
}

void Logger::setThresold(int maxObjs)
{
    if (maxObjs > MAX_LOGGING_OBJECTS)
    {
        throw new std::exception();
    }

    this->thres = maxObjs;
}

void Logger::addObj(LoggingObj *obj)
{
    int i;

    this->buffer[this->bufferSize] = obj;
    this->bufferSize++;


    // If thresold is reached, write everything to logfile
    if (this->thres < this->bufferSize)
    {
        printf("Thresold reached ...");

        FILE *fp;
        fp = fopen(this->logfile.c_str(), "ab");

        for (i = 0; i < this->bufferSize; i++)
        {
            fprintf(fp, "%s\n", this->buffer[i]->out());
        }

        fclose(fp);

        this->bufferSize = 0;

        printf(" and finished with writing;");
    }
}
