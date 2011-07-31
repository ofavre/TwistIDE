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
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, long off, long len)
        {
            vim.send_command(bufID, seqno, "guard", VimValue::newOffset(off), VimValue::newNumber(len));
        }
};



#endif // _VIMCOMMANDS_GUARD_HPP
