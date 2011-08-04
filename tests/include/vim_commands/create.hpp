#ifndef _VIMCOMMANDS_CREATE_HPP
#define _VIMCOMMANDS_CREATE_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandCreate : public VimCommand
{
    private:
        inline VimCommandCreate()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID)
        {
            vim.send_command(bufID, "create");
        }
};



#endif // _VIMCOMMANDS_CREATE_HPP
