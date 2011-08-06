class GVim;

#ifndef _GVIM_HPP
#define _GVIM_HPP



#include <string>

#include <sigc++/sigc++.h>

#include <gtkmm.h>
#include <gtkmm/socket.h>



#include "VimEventManager.hpp"
#include "VimSocketInterfaceServer.hpp"
#include "VimSocketInterfaceCommunicator.hpp"



class GVim : public Gtk::Window
{
    public:
        GVim();

        virtual ~GVim();

        void setupSignals();

        bool startServer();

        void on_vimsocket_newclient(VimSocketInterfaceCommunicator* conn);

        void on_vimclient_special_auth(VimSocketInterfaceCommunicator& vim, std::string password);

        void on_vimclient_balloonEval(VimSocketInterfaceCommunicator& vim, long bufID, long offset, long len, std::string type);

        void on_vimclient_balloonText(VimSocketInterfaceCommunicator& vim, long bufID, std::string text);

        void on_vimclient_buttonRelease(VimSocketInterfaceCommunicator& vim, long bufID, long button, long line, long col);

        void on_vimclient_disconnect(VimSocketInterfaceCommunicator& vim, long bufID);

        void on_vimclient_fileOpened(VimSocketInterfaceCommunicator& vim, long bufID, std::string pathname, bool open, bool modified);

        void on_vimclient_geometry(VimSocketInterfaceCommunicator& vim, long bufID, long cols, long rows, long x, long y);

        void on_vimclient_insert(VimSocketInterfaceCommunicator& vim, long bufID, long offset, std::string text);

        void on_vimclient_keyCommand(VimSocketInterfaceCommunicator& vim, long bufID, std::string keyName);

        void on_vimclient_keyAtPos(VimSocketInterfaceCommunicator& vim, long bufID, std::string keyName, long lnum, long lcol);

        void on_vimclient_killed(VimSocketInterfaceCommunicator& vim, long bufID);

        void on_vimclient_newDotAndMark(VimSocketInterfaceCommunicator& vim, long bufID, long offset1, long offset2);

        void on_vimclient_remove(VimSocketInterfaceCommunicator& vim, long bufID, long offset, long length);

        void on_vimclient_save(VimSocketInterfaceCommunicator& vim, long bufID);

        void on_vimclient_startupDone(VimSocketInterfaceCommunicator& vim, long bufID);

        void on_vimclient_unmodified(VimSocketInterfaceCommunicator& vim, long bufID);

        void on_vimclient_version(VimSocketInterfaceCommunicator& vim, long bufID, std::string version);

        void on_vimclient_getCursor_reply(VimSocketInterfaceCommunicator& vim, long bufID, long cursorBufID, long lnum, long col, long off);

        void on_vimclient_getLength_reply(VimSocketInterfaceCommunicator& vim, long bufID, long len);

        void on_vimclient_getAnno_reply(VimSocketInterfaceCommunicator& vim, long bufID, long lnum);

        void on_vimclient_getModified_reply(VimSocketInterfaceCommunicator& vim, long bufID, long count);

        void on_vimclient_getText_reply(VimSocketInterfaceCommunicator& vim, long bufID, std::string text);

        void on_vimclient_insert_reply(VimSocketInterfaceCommunicator& vim, long bufID, bool success);

        void on_vimclient_remove_reply(VimSocketInterfaceCommunicator& vim, long bufID, bool success);

        void on_vimclient_saveAndExit_reply(VimSocketInterfaceCommunicator& vim, long bufID, long modifiedBuffers);

        void spawnGVim();

        void on_mSocket_realize();

        void on_map_quit();

        void on_mSocket_plug_added();

        bool on_mSocket_plug_removed();

        void on_mSocket_grab_focus();

    protected:
        Gtk::VBox       mBox;
        Gtk::Socket     mSocket;

        VimEventManager mEventManager;
        VimSocketInterfaceServer
                        mServer;
        VimSocketInterfaceCommunicator*
                        mpVim;
};



#endif // _GVIM_HPP

