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
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&thread, &attr, &VimSocketInterfaceCommunicator::readloop, reinterpret_cast<void*>(this)) != 0)
    {
        perror("Cannot start client socket reading thread");
        throw std::runtime_error("pthread_create() failed");
    }
    pthread_attr_destroy(&attr);
}

void* VimSocketInterfaceCommunicator::readloop(void* pthat)
{
    std::cout << "Client socket reading thread started" << std::endl;
    reinterpret_cast<VimSocketInterfaceCommunicator*>(pthat)->readloop();
    std::cout << "Client socket reading thread finished" << std::endl;
    return NULL;
}

void VimSocketInterfaceCommunicator::readloop()
{
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
            break;
        }
        buffer[n] = '\0';
        // Analyze answers and events
        char* start = buffer;
        char* nl = strchr(start, '\n');
        for ( ; nl != NULL ; nl = strchr(start, '\n'))
        {
            *nl ='\0';
            line << start;
            analyze_answer(line);
            // Reset line as an empty buffer
            line.rdbuf()->str("");
            line.seekg(0);
            line.seekp(0);
            line.clear(); // clear eof()
            // Next line
            start = nl+1;
        };
        if (start[0] != '\0')
            std::cout << "Adding unfinished \"" << start << "\"" << std::endl;
        line << start;
    }

    close(); // safe to call twice
    return;
}

void VimSocketInterfaceCommunicator::analyze_answer(std::stringstream& line)
{
    std::cout << "Analyzing \"" << line.str() << "\"" << std::endl;
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
        // Following are space separated words for return values
        //TODO: Have some model to emit an answer to the right function call
        //      Based on seqno.
        std::cout << "TODO: Analyze answer according to the matching function call!" << std::endl;
    }
    else if (c == ':')
    {
        bufID = n;
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
        line >> seqno;
        // Following are space separated words for parameters values
        if (mEventManager == NULL)
        {
            std::cout << "No VimEventManager set, ignoring event." << std::endl;
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
                std::cout << "No VimEventManager set, ignoring special command." << std::endl;
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

void VimSocketInterfaceCommunicator::send_command(long bufID, long seqno, std::string command)
{
    send_command(bufID, seqno, command, std::vector<VimValue>());
}

void VimSocketInterfaceCommunicator::send_command(long bufID, long seqno, std::string command, VimValue param1)
{
    std::vector<VimValue> parameters;
    parameters.reserve(1);
    parameters.push_back(param1);
    send_command(bufID, seqno, command, parameters);
}

void VimSocketInterfaceCommunicator::send_command(long bufID, long seqno, std::string command, VimValue param1, VimValue param2)
{
    std::vector<VimValue> parameters;
    parameters.reserve(2);
    parameters.push_back(param1);
    parameters.push_back(param2);
    send_command(bufID, seqno, command, parameters);
}

void VimSocketInterfaceCommunicator::send_command(long bufID, long seqno, std::string command, VimValue param1, VimValue param2, VimValue param3)
{
    std::vector<VimValue> parameters;
    parameters.reserve(3);
    parameters.push_back(param1);
    parameters.push_back(param2);
    parameters.push_back(param3);
    send_command(bufID, seqno, command, parameters);
}

void VimSocketInterfaceCommunicator::send_command(long bufID, long seqno, std::string command, VimValue param1, VimValue param2, VimValue param3, VimValue param4)
{
    std::vector<VimValue> parameters;
    parameters.reserve(4);
    parameters.push_back(param1);
    parameters.push_back(param2);
    parameters.push_back(param3);
    parameters.push_back(param4);
    send_command(bufID, seqno, command, parameters);
}

void VimSocketInterfaceCommunicator::send_command(long bufID, long seqno, std::string command, VimValue param1, VimValue param2, VimValue param3, VimValue param4, VimValue param5)
{
    std::vector<VimValue> parameters;
    parameters.reserve(5);
    parameters.push_back(param1);
    parameters.push_back(param2);
    parameters.push_back(param3);
    parameters.push_back(param4);
    parameters.push_back(param5);
    send_command(bufID, seqno, command, parameters);
}

void VimSocketInterfaceCommunicator::send_command(long bufID, long seqno, std::string command, VimValue param1, VimValue param2, VimValue param3, VimValue param4, VimValue param5, VimValue param6)
{
    std::vector<VimValue> parameters;
    parameters.reserve(6);
    parameters.push_back(param1);
    parameters.push_back(param2);
    parameters.push_back(param3);
    parameters.push_back(param4);
    parameters.push_back(param5);
    parameters.push_back(param6);
    send_command(bufID, seqno, command, parameters);
}

void VimSocketInterfaceCommunicator::send_command(long bufID, long seqno, std::string command, const std::vector<VimValue>& parameters)
{
    std::stringstream buff;
    buff << bufID << ":" << command << "!" << seqno;
    for (std::vector<VimValue>::const_iterator it = parameters.begin() ; it < parameters.end() ; it++)
        buff << " " << it->toString();
    std::cout << "Sending command \"" << buff.str() << "\"" << std::endl;
    buff << std::endl;
    send(mSocket, buff.str().c_str(), buff.str().length(), 0);
}

void VimSocketInterfaceCommunicator::close()
{
    if (mSocket == -1) return;
    std::cout << "Closing client socket" << std::endl;
    ::close(mSocket);
    mSocket = -1;
}

