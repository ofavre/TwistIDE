#ifndef _VIMEVENTS_KEYCOMMAND_HPP
#define _VIMEVENTS_KEYCOMMAND_HPP 1



#include "VimEventManager.hpp"



class VimEventKeyCommand: public VimEvent
{
    friend class VimEventManager;
    private:
        VimEventKeyCommand()
        :   VimEvent(VimEventKeyCommand::getName(), 1, VimValue::STRING)
        {
        }

    public:
        static const std::string getName()
        {
            return "keyCommand";
        }

        void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, parameters[0].getString());
        }

        sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        sigc::signal<void,VimSocketInterfaceCommunicator&,long,std::string> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,std::string>    mSignal;
};



#endif // _VIMEVENTS_KEYCOMMAND_HPP

