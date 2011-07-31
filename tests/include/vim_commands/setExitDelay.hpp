#ifndef _VIMCOMMANDS_SETEXITDELAY_HPP
#define _VIMCOMMANDS_SETEXITDELAY_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandSetExitDelay : public VimCommand
{
    private:
        inline VimCommandSetExitDelay()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, long seconds)
        {
            vim.send_command(bufID, seqno, "setExitDelay", VimValue::newNumber(seconds));
        }
};



#endif // _VIMCOMMANDS_SETEXITDELAY_HPP
