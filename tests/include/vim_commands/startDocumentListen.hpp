#ifndef _VIMCOMMANDS_STARTDOCUMENTLISTEN_HPP
#define _VIMCOMMANDS_STARTDOCUMENTLISTEN_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandStartDocumentListen : public VimCommand
{
    private:
        inline VimCommandStartDocumentListen()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long seqno)
        {
            vim.send_command(bufID, seqno, "startDocumentListen");
        }
};



#endif // _VIMCOMMANDS_STARTDOCUMENTLISTEN_HPP
