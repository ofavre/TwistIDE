#ifndef _VIMCOMMANDS_INITDONE_HPP
#define _VIMCOMMANDS_INITDONE_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandInitDone : public VimCommand
{
    private:
        inline VimCommandInitDone()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long seqno)
        {
            vim.send_command(bufID, seqno, "initDone");
        }
};



#endif // _VIMCOMMANDS_INITDONE_HPP
