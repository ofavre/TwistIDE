#ifndef _VIMEVENTS_STARTUPDONE_HPP
#define _VIMEVENTS_STARTUPDONE_HPP 1



#include "VimEventManager.hpp"



class VimEventStartupDone: public VimEvent
{
    friend class VimEventManager;
    private:
        inline VimEventStartupDone()
        :   VimEvent(VimEventStartupDone::getName())
        {
        }

    public:
        inline static const std::string getName()
        {
            return "startupDone";
        }

        inline void emit(VimSocketInterfaceCommunicator& vim, long bufID, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID);
        }

        inline sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        inline sigc::signal<void,VimSocketInterfaceCommunicator&,long> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long>    mSignal;
};



#endif // _VIMEVENTS_STARTUPDONE_HPP

