#ifndef _VIMCOMMANDS_STARTATOMIC_HPP
#define _VIMCOMMANDS_STARTATOMIC_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandStartAtomic : public VimCommand
{
    private:
        inline VimCommandStartAtomic()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID)
        {
            vim.send_command(bufID, "startAtomic");
        }
};



#endif // _VIMCOMMANDS_STARTATOMIC_HPP
