#ifndef _VIMCOMMANDS_SETVISIBLE_HPP
#define _VIMCOMMANDS_SETVISIBLE_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandSetVisible : public VimCommand
{
    private:
        inline VimCommandSetVisible()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, bool visible)
        {
            vim.send_command(bufID, seqno, "setVisible", VimValue::newBoolean(visible));
        }
};



#endif // _VIMCOMMANDS_SETVISIBLE_HPP