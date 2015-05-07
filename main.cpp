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

#define NUMBER_OF_THREADS (1)
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
    TcpSerialSocket tss("127.0.0.1", t->port);
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


    seconds end = duration_cast< seconds >(
        system_clock::now().time_since_epoch()
    );

    printf("Runtime: %d seconds\n", (int)(end.count() - start.count()));

    return (void *) 0;
}


pthread_t thread[NUMBER_OF_THREADS];
struct thread_param t[NUMBER_OF_THREADS];

int main()
{
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


    // Get logging filename
    std::cout << "Log-File-Name: ";

    std::string filename;
    std::cin >> filename;


//    struct thread_param t1;

//    t1.logfile = (char*) (filename.c_str());
//    t1.port = FIRST_PORT;

//    log_socket((void*) &t1);

//    while (global_stop == 0)
//    {
//        // run
//    }

//    return 0;


    // Create logger threads
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        char buf[100];

        // Logfile gets consecutive number
        sprintf(((char*)buf), "%s_%d", filename.c_str(), i);
        t[i].logfile = buf;

        // Set port
        t[i].port = FIRST_PORT + i;

        // Start thread
        pthread_create(&(thread[i]), NULL, log_socket, (void *) &(t[i]));
    }

    while (global_stop == 0)
    {
        // just wait
    }

    return 0;
}

