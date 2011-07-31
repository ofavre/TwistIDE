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

void VimEvent::emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
{
    if (parameters.size() != mParameterTypes.size())
        throw std::invalid_argument("Bad parameter count");
    std::vector<VimValue::Type>::iterator types = mParameterTypes.begin();
    std::vector<VimValue>::const_iterator values = parameters.begin();
    for (int i = mParameterTypes.size() ; i > 0 ; i--, types++, values++)
    {
        if (values->getType() != *types)
            throw std::invalid_argument("Bad parameter type");
    }
    do_emit(vim, bufID, seqno, parameters);
}

