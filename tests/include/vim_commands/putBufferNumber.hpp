#ifndef _VIMCOMMANDS_PUTBUFFERNUMBER_HPP
#define _VIMCOMMANDS_PUTBUFFERNUMBER_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandPutBufferNumber : public VimCommand
{
    private:
        inline VimCommandPutBufferNumber()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, std::string pathName)
        {
            vim.send_command(bufID, seqno, "putBufferNumber", VimValue::newPathName(pathName));
        }
};



#endif // _VIMCOMMANDS_PUTBUFFERNUMBER_HPP
