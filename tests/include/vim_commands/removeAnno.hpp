#ifndef _VIMCOMMANDS_REMOVEANNO_HPP
#define _VIMCOMMANDS_REMOVEANNO_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandRemoveAnno : public VimCommand
{
    private:
        inline VimCommandRemoveAnno()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, long serNum)
        {
            vim.send_command(bufID, seqno, "removeAnno", VimValue::newNumber(serNum));
        }
};



#endif // _VIMCOMMANDS_REMOVEANNO_HPP
