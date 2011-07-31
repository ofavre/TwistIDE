#ifndef _VIMCOMMANDS_NETBEANSBUFFER_HPP
#define _VIMCOMMANDS_NETBEANSBUFFER_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandNetbeansBuffer : public VimCommand
{
    private:
        inline VimCommandNetbeansBuffer()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, bool isNetbeansBuffer)
        {
            vim.send_command(bufID, seqno, "netbeansBuffer", VimValue::newBoolean(isNetbeansBuffer));
        }
};



#endif // _VIMCOMMANDS_NETBEANSBUFFER_HPP
