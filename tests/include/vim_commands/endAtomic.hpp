#ifndef _VIMCOMMANDS_ENDATOMIC_HPP
#define _VIMCOMMANDS_ENDATOMIC_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandEndAtomic : public VimCommand
{
    private:
        inline VimCommandEndAtomic()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID)
        {
            vim.send_command(bufID, "endAtomic");
        }
};



#endif // _VIMCOMMANDS_ENDATOMIC_HPP
