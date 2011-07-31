#include "VimSocketInterfaceCommunicator.hpp"



#include <stdexcept>
#include <cstring>
#include <iostream>

#include <pthread.h>



VimSocketInterfaceCommunicator::VimSocketInterfaceCommunicator(int socket, sockaddr_in addr)
:   mSocket(socket)
,   mSckaddr(addr)
{
    std::cout << "Vim port: " << mSckaddr.sin_port << std::endl;
}

VimSocketInterfaceCommunicator::~VimSocketInterfaceCommunicator()
{
    close(); // safe to call twice
}

void VimSocketInterfaceCommunicator::setEventManager(VimEventManager* manager)
{
    mEventManager = manager;
}

void VimSocketInterfaceCommunicator::readloop_async()
{
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if (pthread_create(&thread, &attr, &VimSocketInterfaceCommunicator::readloop, reinterpret_cast<void*>(this)) != 0)
    {
        perror("Cannot start client socket reading thread");
        throw std::runtime_error("pthread_create() failed");
    }
    std::cout << "Client socket reading thread started" << std::endl;
    pthread_attr_destroy(&attr);
}

void* VimSocketInterfaceCommunicator::readloop(void* pthat)
{
    reinterpret_cast<VimSocketInterfaceCommunicator*>(pthat)->readloop();
    std::cout << "Client socket reading thread finished" << std::endl;
    return NULL;
}

void VimSocketInterfaceCommunicator::readloop()
{
    std::cout << "Inside readloop" << std::endl;

    std::stringstream line;
    char buffer[1051];
    memset(buffer, 0, sizeof(buffer));
    while (mSocket != -1)
    {
        int n = read(mSocket, buffer, 1050);
        if (n < 0)
        {
            perror("ERROR reading from socket");
            break;
        }
        else if (n == 0)
        {
            std::cout << "Nothing more to read, exitting." << std::endl;
            if (mSocket != -1)
                ::close(mSocket);
            mSocket = -1;
            break;
        }
        buffer[n] = '\0';
        printf("Here is the message (%d): %s\n", n, buffer);
        //TODO: Analyze answers and events
        char* start = buffer;
        char* nl = strchr(start, '\n');
        for ( ; nl != NULL ; nl = strchr(start, '\n'))
        {
            *nl ='\0';
            std::cout << "Adding \"" << start << "\"" << std::endl;
            line << start;
            analyze_answer(line);
            line.clear(); // clear eof()
            start = nl+1;
        };
        std::cout << "Adding unfinished \"" << start << "\"" << std::endl;
        line << start;
    }

    std::cout << "Exitting readloop" << std::endl;
    close(); // safe to call twice
    return;
}

void VimSocketInterfaceCommunicator::analyze_answer(std::stringstream& line)
{
    std::cout << "Analyzing : \"" << line.str() << "\"" << std::endl;
    // Read first number: seqno or bufID
    char c;
    long n = 0;
    long bufID = -1;
    long seqno = -1;
    while (line.good())
    {
        line.get(c);
        if (c >= '0' && c <= '9')
        {
            n = n*10 + c-'0';
        }
        else
        {
            break;
        }
    }
    if (c == ' ')
    {
        seqno = n;
        std::cout << "seqno = " << seqno << std::endl;
        // Following are space separated words for return values
        //TODO: Have some model to emit an answer to the right function call
        //      Based on seqno.
    }
    else if (c == ':')
    {
        bufID = n;
        std::cout << "bufID = " << bufID << std::endl;
        std::string evtName;
        while (line.good())
        {
            line.get(c);
            if (c == '=')
            {
                break;
            }
            else
            {
                evtName.push_back(c);
            }
        }
        std::cout << "Event = " << evtName << std::endl;
        line >> seqno;
        std::cout << "seqno = " << seqno << std::endl;
        // Following are space separated words for parameters values
        if (mEventManager == NULL)
        {
            std::cout << "End of the adventure, no VimEventManager set." << std::endl;
        }
        else
        {
            VimEvent* evt = mEventManager->getEvent(evtName);
            if (evt == NULL)
            {
                std::cout << "UNKNOWN EVENT!" << std::endl;
            }
            else
            {
                std::cout << "Found event : " << evt->getName() << std::endl;
                const std::vector<VimValue::Type>& paramTypes = evt->getParameterTypes();
                std::vector<VimValue> params;
                params.reserve(paramTypes.size());
                for (std::vector<VimValue::Type>::const_iterator type = paramTypes.begin() ; type < paramTypes.end() ; type++)
                {
                    // Skip space separators if not already read
                    while (line.good() && line.peek() == ' ') line.get(c);
                    // Should be data for the current parameter
                    if (!line.good())
                        throw std::invalid_argument("Not enough data for to read all parameters");
                    // Parse next parameter
                    VimValue param = VimValue::parse(*type, line);
                    params.push_back(param);
                    std::cout << "Param : " << VimValue::TypeToString(param.getType()) << " = \"" << param.toString() << "\"" << std::endl;
                }
                // Emit the event
                evt->emit(*this, bufID, seqno, params);
            }
        }
    }
    else
    {
        line.unget();
        std::string w;
        line >> w;
        std::cout << "Special command \"" << w << "\"" << std::endl;
        if (w == "AUTH")
        {
            line >> w;
            std::cout << "Password : \"" << w << "\"" << std::endl;
            if (mEventManager == NULL)
            {
                std::cout << "End of the adventure, no VimEventManager set." << std::endl;
            }
            else
            {
                mEventManager->emit_special_auth(*this, w);
            }
        }
        else if (w == "ACCEPT")
        {
            // Unused
        }
        else if (w == "REJECT")
        {
            // Unused
        }
        else
        {
            std::cout << "ERROR !" << std::endl;
        }
    }
    // Eat the stream upto the end
    while (line.good())
    {
        std::string s;
        line >> s;
        std::cout << "UNREAD Word : \"" << s << "\"" << std::endl;
    }
}

void VimSocketInterfaceCommunicator::close()
{
    if (mSocket != -1)
    {
        ::close(mSocket);
        mSocket = -1;
    }
}

