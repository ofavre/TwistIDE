#ifndef _VIMCOMMANDS_SETDOT_HPP
#define _VIMCOMMANDS_SETDOT_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandSetDot : public VimCommand
{
    private:
        inline VimCommandSetDot()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long off)
        {
            vim.send_command(bufID, "setDot", VimValue::newOffset(off));
        }
};



#endif // _VIMCOMMANDS_SETDOT_HPP
