class VimEvent;

#ifndef _VIMEVENT_HPP
#define _VIMEVENT_HPP 1



#include <string>
#include <vector>

#include <sigc++/sigc++.h>



#include "VimValue.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimEvent
{
    public:
        inline VimEvent(const std::string& name)
        :   mName(name)
        {
        }

        VimEvent(const std::string& name, int paramCount, VimValue::Type param1Type, ...);

        inline const std::string& getName()
        {
            return mName;
        }

        inline const std::vector<VimValue::Type>& getParameterTypes()
        {
            return mParameterTypes;
        }

        void emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters);

        virtual void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& typeCheckedParameters) = 0;

        virtual sigc::signal_base get_signalbase() = 0;

    private:
        const std::string           mName;
        std::vector<VimValue::Type> mParameterTypes;
};



#endif // _VIMEVENT_HPP

