#ifndef _VIMEVENTS_KEYATPOS_HPP
#define _VIMEVENTS_KEYATPOS_HPP 1



#include "VimEventManager.hpp"



class VimEventKeyAtPos: public VimEvent
{
    friend class VimEventManager;
    private:
        inline VimEventKeyAtPos()
        :   VimEvent(VimEventKeyAtPos::getName(), 3, VimValue::STRING, VimValue::LINE_NUMBER, VimValue::COLUMN_NUMBER)
        {
        }

    public:
        inline static const std::string getName()
        {
            return "keyAtPos";
        }

        inline void emit(VimSocketInterfaceCommunicator& vim, long bufID, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, parameters[0].getString(), parameters[1].getLineNumber(), parameters[2].getColumnNumber());
        }

        inline sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        inline sigc::signal<void,VimSocketInterfaceCommunicator&,long,std::string,long,long> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,std::string,long,long>  mSignal;
};



#endif // _VIMEVENTS_KEYATPOS_HPP

