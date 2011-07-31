#ifndef _VIMCOMMANDS_SAVEDONE_HPP
#define _VIMCOMMANDS_SAVEDONE_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandSaveDone : public VimCommand
{
    private:
        inline VimCommandSaveDone()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long seqno)
        {
            vim.send_command(bufID, seqno, "saveDone");
        }
};



#endif // _VIMCOMMANDS_SAVEDONE_HPP
