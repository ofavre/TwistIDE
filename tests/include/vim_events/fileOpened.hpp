#ifndef _VIMEVENTS_FILEOPENED_HPP
#define _VIMEVENTS_FILEOPENED_HPP 1



#include "VimEventManager.hpp"



class VimEventFileOpened: public VimEvent
{
    friend class VimEventManager;
    private:
        inline VimEventFileOpened()
        :   VimEvent(VimEventFileOpened::getName(), 3, VimValue::PATHNAME, VimValue::BOOLEAN, VimValue::BOOLEAN)
        {
        }

    public:
        inline static const std::string getName()
        {
            return "fileOpened";
        }

        inline void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, parameters[0].getPathName(), parameters[1].getBoolean(), parameters[2].getBoolean());
        }

        inline sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        inline sigc::signal<void,VimSocketInterfaceCommunicator&,long,std::string,bool,bool> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,std::string,bool,bool>  mSignal;
};



#endif // _VIMEVENTS_FILEOPENED_HPP

