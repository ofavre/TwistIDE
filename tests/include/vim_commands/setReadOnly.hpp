#ifndef _VIMCOMMANDS_SETREADONLY_HPP
#define _VIMCOMMANDS_SETREADONLY_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandSetReadOnly : public VimCommand
{
    private:
        inline VimCommandSetReadOnly()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long seqno)
        {
            vim.send_command(bufID, seqno, "setReadOnly");
        }
};



#endif // _VIMCOMMANDS_SETREADONLY_HPP
