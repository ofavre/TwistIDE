class VimCommand;

#ifndef _VIMCOMMAND_HPP
#define _VIMCOMMAND_HPP 1



#include "VimSocketInterfaceCommunicator.hpp"



#include <stdexcept>



// A hint to the compiler
class VimCommand
{
    protected:
        inline VimCommand()
        {
            throw std::logic_error("Inherited VimCommand should have private constructor, and never be instantiated");
        }
};



#endif // _VIMCOMMAND_HPP
