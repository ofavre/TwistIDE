#include "VimSocketInterfaceServer.hpp"



#include <cstring>
#include <iostream>

#include <pthread.h>



VimSocketInterfaceServer::VimSocketInterfaceServer()
:   mSocket(-1)
,   mPort(0)
,   mListening(false)
{
    memset(&mSckaddr, 0, sizeof(mSckaddr));
}

unsigned short VimSocketInterfaceServer::get_realport() const
{
    return ntohs(mSckaddr.sin_port);
}

bool VimSocketInterfaceServer::create()
{
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket == -1)
    {
        perror("Cannot open a listening TCP socket");
        return false;
    }
    return true;
}

bool VimSocketInterfaceServer::bind()
{
    memset(&mSckaddr, 0, sizeof(mSckaddr));
    mSckaddr.sin_family = AF_INET;
    mSckaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    mSckaddr.sin_port = htons((unsigned short)mPort);
    if (::bind(mSocket, (struct sockaddr *)&mSckaddr, sizeof(mSckaddr)) == -1)
    {
        perror("bind() error");
        return false;
    }
    return true;
}

bool VimSocketInterfaceServer::listen()
{
    if (::listen(mSocket, 1) == -1)
    {
        perror("listen() error");
        mListening = false;
        return false;
    }
    mListening = true;
    return true;
}

bool VimSocketInterfaceServer::accept()
{
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    std::cout << "Accepting..." << std::endl;
    int sckConn = ::accept(mSocket, (sockaddr*)&addr, &len);
    if (sckConn < 0)
    {
        perror("Cannot accept connection");
        return false;
    }

    VimSocketInterfaceCommunicator* conn = new VimSocketInterfaceCommunicator(sckConn, addr);
    if (mNewClientSignal.empty()) {
        // Proper closing
        conn->close();
        delete conn;
    } else {
        // gives ownership
        mNewClientSignal.emit(conn);
    }
    return true;
}

void VimSocketInterfaceServer::accept_loop()
{
    while (mSocket != -1)
    {
        if (!accept())
            break;
    }
}

void* VimSocketInterfaceServer::accept_loop(void* pthat)
{
    reinterpret_cast<VimSocketInterfaceServer*>(pthat)->accept_loop();
    std::cout << "Accept thread finished" << std::endl;
    return NULL;
}

bool VimSocketInterfaceServer::accept_loop_async()
{
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if (pthread_create(&thread, &attr, &VimSocketInterfaceServer::accept_loop, reinterpret_cast<void*>(this)) != 0)
    {
        perror("Cannot start accept thread");
        return false;
    }
    std::cout << "Accept thread started" << std::endl;
    pthread_attr_destroy(&attr);
    return true;
}

void VimSocketInterfaceServer::close()
{
    if (mSocket == -1) return;
    std::cout << "Closing server" << std::endl;
    ::close(mSocket);
    mSocket = -1;
    mListening = false;
}

