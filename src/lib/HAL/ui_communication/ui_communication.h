#ifndef UI_COMMUNICATION_H
#define UI_COMMUNICATION_H

#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>

class UICommunication 
{
private:
    zmq::context_t __context;
    zmq::socket_t __socket;
    zmq::pollitem_t __items[1];

public:
    UICommunication();
    int is_req_waiting();
    std::string get_command();
    void send_command(std::string command);
};


#endif // UI_COMMUNICATION_H