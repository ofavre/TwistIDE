class VimEventManager;

#ifndef _VIMEVENTMANAGER_HPP
#define _VIMEVENTMANAGER_HPP 1



#include <string>
#include <map>

#include <sigc++/sigc++.h>



#include "VimEvent.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class VimEventManager
{
    public:
        inline VimEventManager()
        {
        }

        template <class E>
        E* addEvent();

        template <class E>
        E* getEvent();

        VimEvent* getEvent(const std::string& name);

        sigc::signal<void, VimSocketInterfaceCommunicator&, std::string> get_signal_special_auth();

        void emit_special_auth(VimSocketInterfaceCommunicator& vim, std::string password);

    private:
        std::map<std::string, VimEvent*>                            mEventsByName;
        std::map<std::string, VimEvent*>                            mEventsByTypeName;
        sigc::signal<void, VimSocketInterfaceCommunicator&, std::string>  mSpecialAuthSignal;
};



#include "VimEventManager.tcc"

#endif // _VIMEVENTMANAGER_HPP

