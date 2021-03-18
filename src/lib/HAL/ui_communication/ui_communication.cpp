#include "ui_communication.h"

UICommunication::UICommunication()
    : __context{1}, __socket{__context, ZMQ_REP}
{
    __socket.bind ("tcp://*:5555");
    __items[0] = {static_cast<void*>(__socket), 0, ZMQ_POLLIN, 0};
}

int UICommunication::is_req_waiting()
{
    zmq::poll (&__items [0], 1, 10);

	if (__items [0].revents & ZMQ_POLLIN) {
        return 1;
    }

    return 0;
}

std::string UICommunication::get_command()
{
    zmq::message_t request;
    __socket.recv (&request);
	return std::string(static_cast<char*>(request.data()), request.size());
}

void UICommunication::send_command(std::string command)
{
    zmq::message_t reply (command.length());
    memcpy (reply.data (), command.c_str(), command.length());
    __socket.send (reply);
}
