#include "VimFunction.hpp"



#include <stdexcept>



VimFunction::VimFunction(const std::string& name, long bufID, int paramCount, VimValue::Type param1Type, ...)
:   mName(name)
,   mBufID(bufID)
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

bool VimFunction::parseReply(std::istream& input, std::vector<VimValue>& parameters)
{
    parameters.reserve(mParameterTypes.size());
    VimValue param;
    // Skip space separators if not already read
    char c;
    while (input.good() && input.peek() == ' ') input.get(c);
    try
    {
        while (parseReplyParameter(input, parameters.size(), param))
        {
            parameters.push_back(param);
            // Skip space separators if not already read
            while (input.good() && input.peek() == ' ') input.get(c);
        }
    }
    catch (std::invalid_argument& ex)
    {
        return false;
    }
    return true;
}

bool VimFunction::parseReplyParameter(std::istream& input, unsigned int index, VimValue& value)
{
    if (index >= mParameterTypes.size())
        return false; // End of parameters
    // Should be data for the current parameter
    if (!input.good())
        throw std::invalid_argument("Not enough data for to read all parameters");
    // Parse next parameter
    value = VimValue::parse(mParameterTypes[index], input);
    return true;
}

