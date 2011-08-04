#ifndef _VIMEVENTS_GEOMETRY_HPP
#define _VIMEVENTS_GEOMETRY_HPP 1



#include "VimEventManager.hpp"



class VimEventGeometry: public VimEvent
{
    friend class VimEventManager;
    private:
        inline VimEventGeometry()
        :   VimEvent(VimEventGeometry::getName(), 4, VimValue::COLUMN_NUMBER, VimValue::LINE_NUMBER, VimValue::COLUMN_NUMBER, VimValue::LINE_NUMBER)
        {
        }

    public:
        inline static const std::string getName()
        {
            return "geometry";
        }

        inline void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, parameters[0].getColumnNumber(), parameters[1].getLineNumber(), parameters[2].getColumnNumber(), parameters[3].getLineNumber());
        }

        inline sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        inline sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,long,long> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,long,long>  mSignal;
};



#endif // _VIMEVENTS_GEOMETRY_HPP
