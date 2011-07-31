#ifndef _VIMCOMMANDS_SPECIALKEYS_HPP
#define _VIMCOMMANDS_SPECIALKEYS_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandSpecialKeys : public VimCommand
{
    private:
        inline VimCommandSpecialKeys()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long seqno)
        {
            vim.send_command(bufID, seqno, "specialKeys");
        }
};



#endif // _VIMCOMMANDS_SPECIALKEYS_HPP
