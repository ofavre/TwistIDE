#ifndef _VIMCOMMANDS_GUARD_HPP
#define _VIMCOMMANDS_GUARD_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandGuard : public VimCommand
{
    private:
        inline VimCommandGuard()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long off, long len)
        {
            vim.send_command(bufID, "guard", VimValue::newOffset(off), VimValue::newNumber(len));
        }
};



#endif // _VIMCOMMANDS_GUARD_HPP
