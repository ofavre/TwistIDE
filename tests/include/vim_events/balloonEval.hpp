#ifndef _VIMEVENTS_BALLOONEVAL_HPP
#define _VIMEVENTS_BALLOONEVAL_HPP 1



#include "VimEventManager.hpp"



class VimEventBalloonEval : public VimEvent
{
    friend class VimEventManager;
    private:
        inline VimEventBalloonEval()
        :   VimEvent(VimEventBalloonEval::getName(), 3, VimValue::OFFSET, VimValue::NUMBER, VimValue::STRING)
        {
        }

    public:
        inline static const std::string getName()
        {
            return "balloonEval";
        }

        inline void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, parameters[0].getOffset(), parameters[1].getNumber(), parameters[2].getString());
        }

        inline sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        inline sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,std::string> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,std::string>  mSignal;
};



#endif // _VIMEVENTS_BALLOONEVAL_HPP
