class VimSocketInterfaceCommunicator;

#ifndef _VIMSOCKETINTERFACECOMMUNICATOR_HPP
#define _VIMSOCKETINTERFACECOMMUNICATOR_HPP 1



#include <string>
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>



#include "VimEventManager.hpp"



class VimSocketInterfaceCommunicator
{
    public:
        VimSocketInterfaceCommunicator(int socket, sockaddr_in addr);

        ~VimSocketInterfaceCommunicator();

        void setEventManager(VimEventManager* manager);

        void readloop_async();

        static void* readloop(void* pthat);

        void readloop();

        void analyze_answer(std::stringstream& line);

        void close();

    private:
        int                 mSocket;
        sockaddr_in         mSckaddr;
        VimEventManager*    mEventManager;
};



#endif // _VIMSOCKETINTERFACECOMMUNICATOR_HPP
