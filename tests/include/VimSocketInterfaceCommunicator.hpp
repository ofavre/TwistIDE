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

        void send_command(long bufID, long seqno, std::string command);

        void send_command(long bufID, long seqno, std::string command, VimValue param1);
        void send_command(long bufID, long seqno, std::string command, VimValue param1, VimValue param2);
        void send_command(long bufID, long seqno, std::string command, VimValue param1, VimValue param2, VimValue param3);
        void send_command(long bufID, long seqno, std::string command, VimValue param1, VimValue param2, VimValue param3, VimValue param4);
        void send_command(long bufID, long seqno, std::string command, VimValue param1, VimValue param2, VimValue param3, VimValue param4, VimValue param5);
        void send_command(long bufID, long seqno, std::string command, VimValue param1, VimValue param2, VimValue param3, VimValue param4, VimValue param5, VimValue param6);

        void send_command(long bufID, long seqno, std::string command, const std::vector<VimValue>& parameters);

        void close();

    private:
        int                 mSocket;
        sockaddr_in         mSckaddr;
        VimEventManager*    mEventManager;
};



#endif // _VIMSOCKETINTERFACECOMMUNICATOR_HPP
