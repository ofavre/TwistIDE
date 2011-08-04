#ifndef _VIMCOMMANDS_ADDANNO_HPP
#define _VIMCOMMANDS_ADDANNO_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandAddAnno : public VimCommand
{
    private:
        inline VimCommandAddAnno()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long serNum, long typeNum, long off, long len)
        {
            vim.send_command(bufID, "addAnno", VimValue::newNumber(serNum), VimValue::newNumber(typeNum), VimValue::newOffset(off), VimValue::newNumber(len));
        }
};



#endif // _VIMCOMMANDS_ADDANNO_HPP
