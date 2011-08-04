#ifndef _VIMCOMMANDS_SETTITLE_HPP
#define _VIMCOMMANDS_SETTITLE_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandSetTitle : public VimCommand
{
    private:
        inline VimCommandSetTitle()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, std::string name)
        {
            vim.send_command(bufID, "setTitle", VimValue::newString(name));
        }
};



#endif // _VIMCOMMANDS_SETTITLE_HPP
