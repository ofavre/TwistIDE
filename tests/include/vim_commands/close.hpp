#ifndef _VIMCOMMANDS_CLOSE_HPP
#define _VIMCOMMANDS_CLOSE_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandClose : public VimCommand
{
    private:
        inline VimCommandClose()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID)
        {
            vim.send_command(bufID, "close");
        }
};



#endif // _VIMCOMMANDS_CLOSE_HPP
