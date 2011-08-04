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
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID)
        {
            vim.send_command(bufID, "specialKeys");
        }
};



#endif // _VIMCOMMANDS_SPECIALKEYS_HPP
