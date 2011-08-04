#ifndef _VIMEVENTS_KILLED_HPP
#define _VIMEVENTS_KILLED_HPP 1



#include "VimEventManager.hpp"



class VimEventKilled: public VimEvent
{
    friend class VimEventManager;
    private:
        inline VimEventKilled()
        :   VimEvent(VimEventKilled::getName())
        {
        }

    public:
        inline static const std::string getName()
        {
            return "killed";
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



#endif // _VIMEVENTS_KILLED_HPP

