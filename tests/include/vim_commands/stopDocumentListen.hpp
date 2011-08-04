#ifndef _VIMCOMMANDS_STOPDOCUMENTLISTEN_HPP
#define _VIMCOMMANDS_STOPDOCUMENTLISTEN_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandStopDocumentListen : public VimCommand
{
    private:
        inline VimCommandStopDocumentListen()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID)
        {
            vim.send_command(bufID, "stopDocumentListen");
        }
};



#endif // _VIMCOMMANDS_STOPDOCUMENTLISTEN_HPP
