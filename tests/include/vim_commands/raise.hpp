#ifndef _VIMCOMMANDS_RAISE_HPP
#define _VIMCOMMANDS_RAISE_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandRaise : public VimCommand
{
    private:
        inline VimCommandRaise()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID)
        {
            vim.send_command(bufID, "raise");
        }
};



#endif // _VIMCOMMANDS_RAISE_HPP
