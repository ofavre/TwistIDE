class VimValue;

#ifndef _VIMVALUE_HPP
#define _VIMVALUE_HPP 1



#include <string>
#include <istream>



class VimValue
{
    public:
        enum Type
        {
            INVALID         = 0,
            STRING          = 1,
            NUMBER          = 2,
            BOOLEAN         = 3,
            OFFSET          = 4,
            LINE_NUMBER     = 5,
            COLUMN_NUMBER   = 6,
            PATHNAME        = 7,
            COLOR           = 8
        };

        inline static const std::string TypeToString(Type type)
        {
            switch (type)
            {
                case STRING:
                    return "STRING";
                case NUMBER:
                    return "NUMBER";
                case BOOLEAN:
                    return "BOOLEAN";
                case OFFSET:
                    return "OFFSET";
                case LINE_NUMBER:
                    return "LINE_NUMBER";
                case COLUMN_NUMBER:
                    return "COLUMN_NUMBER";
                case PATHNAME:
                    return "PATHNAME";
                case COLOR:
                    return "COLOR";
                default:
                    return "UNEXISTING_TYPE";
            }
        }

    public:
        VimValue();
        VimValue(Type type, std::string str);
        VimValue(Type type, long nbr);
        VimValue(Type type, bool bol);

        inline static VimValue newString(const std::string& value)
        {
            return VimValue(STRING, value);
        }

        inline static VimValue newNumber(long value)
        {
            return VimValue(NUMBER, value);
        }

        inline static VimValue newBoolean(bool value)
        {
            return VimValue(BOOLEAN, value);
        }

        inline static VimValue newOffset(long value)
        {
            return VimValue(OFFSET, value);
        }

        inline static VimValue newLineNumber(long value)
        {
            return VimValue(LINE_NUMBER, value);
        }

        inline static VimValue newColumnNumber(long value)
        {
            return VimValue(COLUMN_NUMBER, value);
        }

        inline static VimValue newPathName(const std::string& value)
        {
            return VimValue(PATHNAME, value);
        }

        inline static VimValue newColor(const std::string& value)
        {
            return VimValue(COLOR, value);
        }

        static VimValue parse(Type type, std::istream& input);

        inline Type getType() const
        {
            return mType;
        }

        std::string toString() const;

        inline std::string getSomeString() const
        {
            return mString;
        }

        inline long getSomeNumeric() const
        {
            return mNumber;
        }

        inline bool getSomeBoolean() const
        {
            return mBoolean;
        }

        inline std::string getString() const
        {
            return mString;
        }

        inline long getNumber() const
        {
            return mNumber;
        }

        inline bool getBoolean() const
        {
            return mBoolean;
        }

        inline long getOffset() const
        {
            return mNumber;
        }

        inline long getLineNumber() const
        {
            return mNumber;
        }

        inline long getColumnNumber() const
        {
            return mNumber;
        }

        inline std::string getPathName() const
        {
            return mString;
        }

        inline std::string getColor() const
        {
            return mString;
        }

    private:
        Type        mType;
        std::string mString;
        long        mNumber;
        bool        mBoolean;
};



#endif // _VIMVALUE_HPP

