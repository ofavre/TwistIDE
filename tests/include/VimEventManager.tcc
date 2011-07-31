#include "VimEventManager.hpp"



#include <typeinfo>



template <class E>
E* VimEventManager::addEvent()
{
    const std::type_info& type = typeid(E);
    std::map<std::string, VimEvent*>::iterator pos;
    pos = mEventsByTypeName.find(type.name());
    if (pos != mEventsByTypeName.end())
        return dynamic_cast<E*>(pos->second);
    E* evt = new E();
    mEventsByTypeName.insert(std::pair<std::string,VimEvent*>(type.name(), evt));
    mEventsByName.insert(std::pair<std::string,VimEvent*>(evt->getName(), evt));
    return evt;
}

template <class E>
E* VimEventManager::getEvent()
{
    std::map<std::string, VimEvent*>::iterator pos;
    pos = mEventsByTypeName.find(typeid(E).name());
    if (pos != mEventsByTypeName.end())
        return dynamic_cast<E*>(pos->second);
    return NULL;
}

