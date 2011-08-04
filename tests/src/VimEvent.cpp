#include "VimEvent.hpp"



#include <cstdarg>
#include <stdexcept>

#include <sigc++/sigc++.h>



VimEvent::VimEvent(const std::string& name, int paramCount, VimValue::Type param1Type, ...)
:   mName(name)
{
    mParameterTypes.reserve(paramCount);
    va_list args;
    va_start(args, param1Type);
    mParameterTypes.push_back(param1Type);
    for ( ; --paramCount > 0 ; )
    {
        VimValue::Type type = static_cast<VimValue::Type>(va_arg(args, int)); // VimValue::Type is promoted to int
        mParameterTypes.push_back(type);
    }
    va_end(args);
}

