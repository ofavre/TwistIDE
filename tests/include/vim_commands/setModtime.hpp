#ifndef _VIMCOMMANDS_SETMODTIME_HPP
#define _VIMCOMMANDS_SETMODTIME_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandSetModtime : public VimCommand
{
    private:
        inline VimCommandSetModtime()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, long time)
        {
            vim.send_command(bufID, seqno, "setModtime", VimValue::newNumber(time));
        }
};



#endif // _VIMCOMMANDS_SETMODTIME_HPP
