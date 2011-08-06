class VimFunction;

#ifndef _VIMFUNCTION_HPP
#define _VIMFUNCTION_HPP 1



#include <iostream>
#include <vector>

#include <sigc++/sigc++.h>



#include "VimValue.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimFunction
{
    protected:
        inline VimFunction(const std::string& name, long bufID)
        :   mName(name)
        ,   mBufID(bufID)
        ,   mParameterTypes()
        {
        }

        VimFunction(const std::string& name, long bufID, int paramCount, VimValue::Type param1Type, ...);

    public:
        inline VimFunction(const VimFunction& copy)
        :   mName(copy.mName)
        ,   mBufID(copy.mBufID)
        ,   mParameterTypes(copy.getParameterTypes())
        {
        }

        inline const std::string& getName() const
        {
            return mName;
        }

        inline const std::vector<VimValue::Type>& getParameterTypes() const
        {
            return mParameterTypes;
        }

        inline long getBufID() const
        {
            return mBufID;
        }

        virtual bool parseReply(std::istream& input, std::vector<VimValue>& parameters);

        virtual bool parseReplyParameter(std::istream& input, unsigned int index, VimValue& value);

        virtual void callback(VimSocketInterfaceCommunicator& vim, long bufID, const std::vector<VimValue>& parameters) = 0;

        inline virtual void error_callback(VimSocketInterfaceCommunicator& vim, long bufID, const std::string& message)
        {
            std::cerr << "Function call to \"" << getName() << "\" failed for buffer " << bufID << ": \"" << message << "\"" << std::endl;
        }

    private:
        const std::string           mName;
        long                        mBufID;
        std::vector<VimValue::Type> mParameterTypes;
};



#define VIMFUNCTION_CALLBACK_SLOT_PARAMS_PREFIX void,VimSocketInterfaceCommunicator&,long



#endif // _VIMFUNCTION_HPP

