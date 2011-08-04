#ifndef _VIMEVENTS_BALLOONTEXT_HPP
#define _VIMEVENTS_BALLOONTEXT_HPP 1



#include "VimEventManager.hpp"



class VimEventBalloonText : public VimEvent
{
    friend class VimEventManager;
    private:
        inline VimEventBalloonText()
        :   VimEvent(VimEventBalloonText::getName(), 1, VimValue::STRING)
        {
        }

    public:
        inline static const std::string getName()
        {
            return "balloonText";
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



#endif // _VIMEVENTS_BALLOONTEXT_HPP
