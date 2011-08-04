#ifndef _VIMEVENTS_BUTTONRELEASE_HPP
#define _VIMEVENTS_BUTTONRELEASE_HPP 1



#include "VimEventManager.hpp"



class VimEventButtonRelease : public VimEvent
{
    friend class VimEventManager;
    private:
        inline VimEventButtonRelease()
        :   VimEvent(VimEventButtonRelease::getName(), 1, VimValue::NUMBER, VimValue::LINE_NUMBER, VimValue::COLUMN_NUMBER)
        {
        }

    public:
        inline static const std::string getName()
        {
            return "buttonRelease";
        }

        inline void emit(VimSocketInterfaceCommunicator& vim, long bufID, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, parameters[0].getNumber(), parameters[1].getLineNumber(), parameters[2].getColumnNumber());
        }

        inline sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        inline sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,long> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,long> mSignal;
};



#endif
