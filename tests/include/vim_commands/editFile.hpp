#ifndef _VIMCOMMANDS_EDITFILE_HPP
#define _VIMCOMMANDS_EDITFILE_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandEditFile : public VimCommand
{
    private:
        inline VimCommandEditFile()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, std::string pathName)
        {
            vim.send_command(bufID, seqno, "editFile", VimValue::newPathName(pathName));
        }
};



#endif // _VIMCOMMANDS_EDITFILE_HPP
