#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <chrono>
#include <signal.h>
#include <pthread.h>

#include "tcpserialsocket.h"
#include "logger.h"
#include "loggingobj.h"


using namespace std::chrono;

#define NUMBER_OF_THREADS (7)
#define FIRST_PORT (7001)

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0)


int global_stop = 0;

void termination_handler(int signum)
{
    global_stop = 1;
}


struct thread_param {
    char *logfile;
    int port;
};

void* log_socket(void *param)
{
    struct thread_param *t = (struct thread_param *) param;

    Logger log(t->logfile);
    log.setThresold(100);


    // Setup local TCP socket
    std::string data;
    TcpSerialSocket tss("10.162.177.202", t->port);
    tss.conn();


    sleep(1);
    tss.send_data("??\n");

    sleep(1);

    tss.clear_buffer();
    tss.send_data("E+\n");

    seconds start = duration_cast< seconds >(
        system_clock::now().time_since_epoch()
    );

    while (global_stop == 0)
    {
        data = tss.receive(2);

        printf("Data = " BYTETOBINARYPATTERN "-" BYTETOBINARYPATTERN " %2.2x-%2.2x | ", BYTETOBINARY(data[0]), BYTETOBINARY(data[1]), data[0] & 0xFF, data[1] & 0xFF);

        if ((data[0] & 0x80) == 0)
        {
            printf("Warning: Wrong order!\n");
            tss.receive(1);
            continue;
        }

        printf("Recv: %s\n", data.c_str());

        // Calculate current time
        microseconds us = duration_cast< microseconds >(
            system_clock::now().time_since_epoch()
        );

        log.addObj(new LoggingObj(data[0] & 0x7F, data[1] & 0x7F, (data[1] & 0x80) ? 1 : 0, us.count()));
    }

    tss.clear_buffer();
    tss.send_data("E-\n");

    seconds end = duration_cast< seconds >(
        system_clock::now().time_since_epoch()
    );

    printf("Runtime: %d seconds\n", (int)(end.count() - start.count()));

    return (void *) 0;
}


pthread_t thread[NUMBER_OF_THREADS];
struct thread_param t[NUMBER_OF_THREADS];


char logfile[100] = "";
int dur = 20;

#define HELP_MESSAGE "\n\
Serial Client Logger\n\n\
Command line arguments:\n\
 -l logfile : Name of logfile (without .dvs extension) \n\
 -d duration : Duration of measurement\n\
\n\
Example: \n\
./serial-client-logger -l test1 -d 60\n\n"

int main(int argc, char** argv)
{
    // Get command line options
    int c;

    while ((c = getopt(argc, argv, "l:d:h?")) != -1)
    {
        switch (c)
        {
        case 'l':
            sscanf(optarg, "%s", logfile);
            printf("Device path: %s\n", (char*)logfile);
            break;

        case 'd':
            sscanf(optarg, "%d", &dur);
            printf("Duration: %d\n", dur);
            break;

        case 'h':
        case '?':
            printf(HELP_MESSAGE);
            return 0;
            break;

        default:
            break;
        }
    }

    // Setup right singal handling (allows to safely terminate applicaiton with Ctrl+C)
    // ... as described in http://www.gnu.org/software/libc/manual/html_node/Sigaction-Function-Example.html
    struct sigaction new_action, old_action;

    new_action.sa_handler = termination_handler;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;

    sigaction (SIGINT, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
    {
        sigaction (SIGINT, &new_action, NULL);
    }

    sigaction (SIGKILL, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
    {
        sigaction (SIGKILL, &new_action, NULL);
    }

    sigaction (SIGTERM, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
    {
        sigaction (SIGTERM, &new_action, NULL);
    }

    // Determine filename
    std::string filename;
    if (strlen(logfile) > 0)
    {
        // From command line arguement
        filename = std::string(logfile);
    }
    else
    {
        // Get logging filename
        std::cout << "Log-File-Name: ";


        std::cin >> filename;
    }

    char buf[NUMBER_OF_THREADS][100];

    // Create logger threads
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        // Logfile gets consecutive number
        sprintf((char*)(buf[i]), "%s_%d", filename.c_str(), i);
        t[i].logfile = buf[i];

        // Set port
        t[i].port = FIRST_PORT + i;

        // Start thread
        pthread_create(&(thread[i]), NULL, log_socket, (void *) &(t[i]));
    }


    seconds start = duration_cast< seconds >(
        system_clock::now().time_since_epoch()
    );


    while (global_stop == 0)
    {
        seconds end = duration_cast< seconds >(
            system_clock::now().time_since_epoch()
        );

        if ((int)(end.count() - start.count()) > dur)
        {
            global_stop = 1;
        }

        usleep(100000);
    }

    return 0;
}

