#ifndef _VIMEVENTS_INSERT_HPP
#define _VIMEVENTS_INSERT_HPP 1



#include "VimEventManager.hpp"



class VimEventInsert: public VimEvent
{
    friend class VimEventManager;
    private:
        inline VimEventInsert()
        :   VimEvent(VimEventInsert::getName(), 2, VimValue::OFFSET, VimValue::STRING)
        {
        }

    public:
        inline static const std::string getName()
        {
            return "insert";
        }

        inline void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, seqno, parameters[0].getOffset(), parameters[1].getString());
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
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,std::string>   mSignal;
};



#endif // _VIMEVENTS_INSERT_HPP

