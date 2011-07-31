#include "VimValue.hpp"



#include <stdexcept>
#include <sstream>



VimValue::VimValue(Type type, std::string str)
{
    if (type != STRING && type != PATHNAME && type != COLOR)
        throw std::invalid_argument("Invalid VimValue type");
    mType = type;
    mString = str;
}

VimValue::VimValue(Type type, long nbr)
{
    if (type != NUMBER && type != OFFSET && type != LINE_NUMBER && type != COLUMN_NUMBER)
        throw std::invalid_argument("Invalid VimValue type");
    mType = type;
    mNumber = nbr;
}

VimValue::VimValue(Type type, bool bol)
{
    if (type != BOOLEAN)
        throw std::invalid_argument("Invalid VimValue type");
    mType = type;
    mBoolean = bol;
}

VimValue VimValue::parse(Type type, std::istream& input)
{
    if (type == NUMBER || type == OFFSET || type == LINE_NUMBER || type == COLUMN_NUMBER)
    {
        long n;
        input >> n;
        return VimValue(type, n);
    }
    else if (type == BOOLEAN)
    {
        std::string s;
        input >> s;
        return VimValue(type, s == "T");
    }
    else if (type == COLOR)
    {
        std::string s;
        input >> s;
        return VimValue(type, s);
    }
    else if (type == STRING || type == PATHNAME)
    {
        char c;
        input.get(c);
        if (c != '"')
            throw std::invalid_argument("Invalid input for parsing type STRING or PATHNAME");
        std::string s;
        while (input.good())
        {
            if (c == '\\')
            {
                input.get(c);
                if (c == '"')
                    s.push_back('"');
                else if (c == 'n')
                    s.push_back('\n');
                else if (c == 'r')
                    s.push_back('\r');
                else if (c == 'r')
                    s.push_back('\t');
                else if (c == '\\')
                    s.push_back('\\');
                // ADDITIONS
                else if (c == '0')
                    s.push_back('\0');
                else
                    s.push_back(c);
            }
            else
            {
                input.get(c);
                if (c == '\\')
                    continue;
                else if (c == '"')
                    break;
                else
                    s.push_back(c);
            }
        }
        // Read space separation, if any
        if (input.good())
        {
            input.get(c);
            // Put it back if the read character (if any) is not a space
            if (input.gcount() == 1 && c != ' ')
                // Denotes an error...
                input.unget();
        }
        return VimValue(type, s);
    }
    else
    {
        throw std::invalid_argument("Unknown or unparsable type");
    }
}

std::string VimValue::toString() const
{
    switch (mType)
    {
        case STRING:
        case PATHNAME:
        case COLOR:
            return mString;
        case NUMBER:
        case OFFSET:
        case LINE_NUMBER:
        case COLUMN_NUMBER:
            {
                std::stringstream tostr;
                tostr << mNumber;
                return tostr.str();
            }
            break;
        case BOOLEAN:
            if (mBoolean)
                return "T";
            else
                return "F";
        default:
            throw std::invalid_argument("Unknown or un-toString-able type");
    }
}

