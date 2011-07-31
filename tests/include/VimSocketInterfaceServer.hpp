class VimSocketInterfaceServer;

#ifndef _VIMSOCKETINTERFACESERVER_HPP
#define _VIMSOCKETINTERFACESERVER_HPP 1



#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <sigc++/sigc++.h>



#include "VimSocketInterfaceCommunicator.hpp"



class VimSocketInterfaceServer
{
    public:
        VimSocketInterfaceServer();

        inline sigc::signal<void,VimSocketInterfaceCommunicator*> signal_newClient() const
        {
            return mNewClientSignal;
        }

        inline void set_port(unsigned short port)
        {
            mPort = port;
        }

        inline unsigned short get_port() const
        {
            return mPort;
        }

        unsigned short get_realport() const;

        bool create();

        bool bind();

        bool listen();

        bool accept();

        void accept_loop();

    private:
        static void* accept_loop(void* pthat);

    public:
        bool accept_loop_async();

        bool listening() const
        {
            return mListening;
        }

        void close();

        inline bool created() const
        {
            return mSocket != -1;
        }

    private:
        int             mSocket;
        sockaddr_in     mSckaddr;
        unsigned short  mPort;
        bool            mListening;
        sigc::signal<void,VimSocketInterfaceCommunicator*>
                        mNewClientSignal;
};



#endif // _VIMSOCKETINTERFACESERVER_HPP
