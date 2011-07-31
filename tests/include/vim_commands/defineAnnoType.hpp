#ifndef _VIMCOMMANDS_DEFINEANNOTYPE_HPP
#define _VIMCOMMANDS_DEFINEANNOTYPE_HPP 1



#include "VimCommand.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimCommandDefineAnnoType : public VimCommand
{
    private:
        inline VimCommandDefineAnnoType()
        {
        }

    public:
        inline static void send(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, long typeNum, std::string typeName, std::string tooltip, std::string glyphFile, std::string fg, std::string bg)
        {
            vim.send_command(bufID, seqno, "defineAnnoType", VimValue::newNumber(typeNum), VimValue::newString(typeName), VimValue::newString(tooltip), VimValue::newString(glyphFile), VimValue::newColor(fg), VimValue::newColor(bg));
        }
};



#endif // _VIMCOMMANDS_DEFINEANNOTYPE_HPP
