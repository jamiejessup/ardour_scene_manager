#ifndef OSCSERVER_H
#define OSCSERVER_H

#include <unistd.h>
#include <vector>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "lo/lo.h"
#include "JackMIDI/midi.h"
#include "ControllerUpdate.h"
#include <jack/jack.h>
#include <jack/ringbuffer.h>

#define CC_MASK 0b10110000
#define CC_NIBBLE 0b1011
#define MASTER_CC 119


class OSCServer
{
    bool contacted;
    bool started;
    lo_address touchOSC;
    lo_server_thread serverThread;

    ControllerUpdateEvent cu;

    void firstContact(lo_address);
    void sendTrackBank(int bankNumber);
    void sendBusBank(int bankNumber);
    void sendMaster();

    pthread_mutex_t idMutex;




    //Controller specific stuff for banking
    int trackBank;
    int busBank;
    int numTrackBanks;
    int numBusBanks;
    char trackIds[8];
    char busIds[4];


    //reference to jack_ring_buffer to send data from Ardour to the controller
    jack_ringbuffer_t *controllerBuffer;



    /*
     * Call backs for OSC data
     *
     */
    int genericHandler(
            const char *path, const char *types, lo_arg ** argv,
            int argc, void *data);
    static int staticHandler(
            const char *path, const char *types, lo_arg ** argv,
            int argc, void *data, void *user_data);

public:
    OSCServer(jack_ringbuffer_t *cb);
    ~OSCServer();
    void activate(int numTB, int numBB);
    void setTrackIds(char *data);
    void setBusIds(char *data);
    void setIds(char *trackData, char *busData);
    void sendToController(std::string& path, float value);
    //reference for gui thread to send stuff to the controller
    jack_ringbuffer_t *controllerUpdate;
};

#endif // OSCSERVER_H
