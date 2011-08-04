#ifndef _VIMCOMMANDS_SETMODIFIED_HPP
#define _VIMCOMMANDS_SETMODIFIED_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandSetModified : public VimCommand
{
    private:
        inline VimCommandSetModified()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, bool modified)
        {
            vim.send_command(bufID, "setModified", VimValue::newBoolean(modified));
        }
};



#endif // _VIMCOMMANDS_SETMODIFIED_HPP
