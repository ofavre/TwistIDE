#include "VimEventManager.hpp"



#include <typeinfo>



VimEvent* VimEventManager::getEvent(const std::string& name)
{
    std::map<std::string, VimEvent*>::iterator pos;
    pos = mEventsByName.find(name);
    if (pos != mEventsByName.end())
        return pos->second;
    return NULL;
}

sigc::signal<void, VimSocketInterfaceCommunicator&, std::string> VimEventManager::get_signal_special_auth()
{
    return mSpecialAuthSignal;
}

void VimEventManager::emit_special_auth(VimSocketInterfaceCommunicator& vim, std::string password)
{
    mSpecialAuthSignal.emit(vim, password);
}

