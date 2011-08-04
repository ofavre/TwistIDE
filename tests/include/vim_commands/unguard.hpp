#ifndef _VIMCOMMANDS_UNGUARD_HPP
#define _VIMCOMMANDS_UNGUARD_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandUnguard : public VimCommand
{
    private:
        inline VimCommandUnguard()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long off, long len)
        {
            vim.send_command(bufID, "unguard", VimValue::newOffset(off), VimValue::newNumber(len));
        }
};



#endif // _VIMCOMMANDS_UNGUARD_HPP
