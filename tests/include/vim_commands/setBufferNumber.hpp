#ifndef _VIMCOMMANDS_SETBUFFERNUMBER_HPP
#define _VIMCOMMANDS_SETBUFFERNUMBER_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandSetBufferNumber : public VimCommand
{
    private:
        inline VimCommandSetBufferNumber()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, std::string pathName)
        {
            vim.send_command(bufID, seqno, "setBufferNumber", VimValue::newPathName(pathName));
        }
};



#endif // _VIMCOMMANDS_SETBUFFERNUMBER_HPP
