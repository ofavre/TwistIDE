#ifndef _VIMEVENTS_REMOVE_HPP
#define _VIMEVENTS_REMOVE_HPP 1



#include "VimEventManager.hpp"



class VimEventRemove: public VimEvent
{
    friend class VimEventManager;
    private:
        inline VimEventRemove()
        :   VimEvent(VimEventRemove::getName(), 2, VimValue::OFFSET, VimValue::NUMBER)
        {
        }

    public:
        inline static const std::string getName()
        {
            return "remove";
        }

        inline void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, parameters[0].getOffset(), parameters[1].getNumber());
        }

        inline sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        inline sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long>  mSignal;
};



#endif // _VIMEVENTS_REMOVE_HPP

