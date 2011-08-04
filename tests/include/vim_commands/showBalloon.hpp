#ifndef _VIMCOMMANDS_SHOWBALLOON_HPP
#define _VIMCOMMANDS_SHOWBALLOON_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandShowBalloon : public VimCommand
{
private:
    inline VimCommandShowBalloon()
    {
    }
    
public:
    inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, std::string text)
    {
        vim.send_command(bufID, "showBalloon", VimValue::newString(text));
    }
};



#endif // _VIMCOMMANDS_SHOWBALLOON_HPP
