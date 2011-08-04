#ifndef _VIMEVENTS_VERSION_HPP
#define _VIMEVENTS_VERSION_HPP 1



#include "VimEventManager.hpp"



class VimEventVersion: public VimEvent
{
    friend class VimEventManager;
    private:
        inline VimEventVersion()
        :   VimEvent(VimEventVersion::getName(), 1, VimValue::STRING)
        {
        }

    public:
        inline static const std::string getName()
        {
            return "version";
        }

        inline void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, parameters[0].getString());
        }

        inline sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        inline sigc::signal<void,VimSocketInterfaceCommunicator&,long,std::string> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,std::string>    mSignal;
};



#endif // _VIMEVENTS_VERSION_HPP

