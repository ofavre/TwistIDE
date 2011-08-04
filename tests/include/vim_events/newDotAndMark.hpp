#ifndef _VIMEVENTS_NEWDOTANDMARK_HPP
#define _VIMEVENTS_NEWDOTANDMARK_HPP 1



#include "VimEventManager.hpp"



class VimEventNewDotAndMark: public VimEvent
{
    friend class VimEventManager;
    private:
        inline VimEventNewDotAndMark()
        :   VimEvent(VimEventNewDotAndMark::getName(), 2, VimValue::OFFSET, VimValue::OFFSET)
        {
        }

    public:
        inline static const std::string getName()
        {
            return "newDotAndMark";
        }

        inline void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, parameters[0].getOffset(), parameters[1].getOffset());
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



#endif // _VIMEVENTS_NEWDOTANDMARK_HPP

