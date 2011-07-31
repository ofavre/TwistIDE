#ifndef _VIMCOMMANDS_SAVE_HPP
#define _VIMCOMMANDS_SAVE_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandSave : public VimCommand
{
    private:
        inline VimCommandSave()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long seqno)
        {
            vim.send_command(bufID, seqno, "save");
        }
};



#endif // _VIMCOMMANDS_SAVE_HPP
