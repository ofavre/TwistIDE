#ifndef _VIMCOMMANDS_SETFULLNAME_HPP
#define _VIMCOMMANDS_SETFULLNAME_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandSetFullName : public VimCommand
{
    private:
        inline VimCommandSetFullName()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, std::string pathName)
        {
            vim.send_command(bufID, seqno, "setFullName", VimValue::newPathName(pathName));
        }
};



#endif // _VIMCOMMANDS_SETFULLNAME_HPP
