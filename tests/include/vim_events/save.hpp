#ifndef _VIMEVENTS_SAVE_HPP
#define _VIMEVENTS_SAVE_HPP 1



#include "VimEventManager.hpp"



class VimEventSave: public VimEvent
{
    friend class VimEventManager;
    private:
        inline VimEventSave()
        :   VimEvent(VimEventSave::getName())
        {
        }

    public:
        inline static const std::string getName()
        {
            return "save";
        }

        inline void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, const std::vector<VimValue>& parameters)
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



#endif // _VIMEVENTS_SAVE_HPP

