#include "GVim.hpp"



#include <iostream>
#include <sstream>



#include "vim_events/balloonEval.hpp"
#include "vim_events/balloonText.hpp"
#include "vim_events/buttonRelease.hpp"
#include "vim_events/disconnect.hpp"
#include "vim_events/fileOpened.hpp"
#include "vim_events/geometry.hpp"
#include "vim_events/insert.hpp"
#include "vim_events/keyAtPos.hpp"
#include "vim_events/keyCommand.hpp"
#include "vim_events/killed.hpp"
#include "vim_events/newDotAndMark.hpp"
#include "vim_events/remove.hpp"
#include "vim_events/save.hpp"
#include "vim_events/startupDone.hpp"
#include "vim_events/unmodified.hpp"
#include "vim_events/version.hpp"

#include "VimCommand.hpp"
#include "vim_commands/create.hpp"
#include "vim_commands/startDocumentListen.hpp"
#include "vim_commands/setTitle.hpp"
#include "vim_commands/setFullName.hpp"



GVim::GVim()
{
    mServer.set_port(0); // 0 for dynamic allocation
    mServer.signal_newClient().connect(sigc::mem_fun(*this, &GVim::on_vimsocket_newclient));
    setupSignals();

    set_title("TwistIDE - GVim embedding test");
    mSocket.signal_realize().connect(sigc::mem_fun(*this, &GVim::on_mSocket_realize));
    mSocket.signal_plug_removed().connect(sigc::mem_fun(*this, &GVim::on_mSocket_plug_removed));
    mSocket.signal_plug_added().connect(sigc::mem_fun(*this, &GVim::on_mSocket_plug_added));
    mSocket.show();

    mBox.pack_start(mSocket, true, true);
    add(mBox);
    mBox.show();

    set_default_size(640, 480);
    set_position(Gtk::WIN_POS_CENTER_ALWAYS);
}

GVim::~GVim()
{
    mServer.close();
    if (mpVim != NULL)
        delete mpVim;
}

void GVim::setupSignals()
{
    mEventManager.get_signal_special_auth().connect(sigc::mem_fun(*this, &GVim::on_vimclient_special_auth));
    mEventManager.addEvent<VimEventBalloonEval>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_balloonEval));
    mEventManager.addEvent<VimEventBalloonText>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_balloonText));
    mEventManager.addEvent<VimEventButtonRelease>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_buttonRelease));
    mEventManager.addEvent<VimEventDisconnect>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_disconnect));
    mEventManager.addEvent<VimEventFileOpened>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_fileOpened));
    mEventManager.addEvent<VimEventGeometry>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_geometry));
    mEventManager.addEvent<VimEventInsert>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_insert));
    mEventManager.addEvent<VimEventKeyCommand>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_keyCommand));
    mEventManager.addEvent<VimEventKeyAtPos>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_keyAtPos));
    mEventManager.addEvent<VimEventKilled>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_killed));
    mEventManager.addEvent<VimEventNewDotAndMark>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_newDotAndMark));
    mEventManager.addEvent<VimEventRemove>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_remove));
    mEventManager.addEvent<VimEventSave>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_save));
    mEventManager.addEvent<VimEventStartupDone>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_startupDone));
    mEventManager.addEvent<VimEventUnmodified>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_unmodified));
    mEventManager.addEvent<VimEventVersion>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_version));
}

bool GVim::startServer()
{
    if (mServer.created()) return true; // already started

    if (!mServer.create())
    {
        std::cout << "Cannot create a server socket." << std::endl;
        mServer.close();
        return false;
    }

    bool bind_ok = false;
    for (int foolkeeper = 10 ; foolkeeper-- > 0 ; )
    {
        if (!mServer.bind())
        {
            if (mServer.get_port() != 0)
            {
                unsigned short port = mServer.get_port();
                std::cout << "Port " << port << " occupied, trying next..." << std::endl;
                mServer.set_port(++port);
            }
            else
            {
                // Dynamic port attribution failure, serious matter
                bind_ok = false;
                break;
            }
        }
        else
        {
            bind_ok = true;
            break;
        }
    }
    if (!bind_ok)
    {
        std::cout << "Cannot bind server socket to a port." << std::endl;
        mServer.close();
        return false;
    }

    if (!mServer.listen())
    {
        std::cout << "Cannot listen on server socket." << std::endl;
        mServer.close();
        return false;
    }

    std::cout << "Server port: " << mServer.get_realport() << std::endl;

    return true;
}

void GVim::on_vimsocket_newclient(VimSocketInterfaceCommunicator* conn)
{
    mpVim = conn;
    mpVim->setEventManager(&mEventManager);
    mpVim->readloop_async();
}

void GVim::on_vimclient_special_auth(VimSocketInterfaceCommunicator& vim, std::string password)
{
    std::cout << "Vim authenticating with password \"" << password << "\"" << std::endl;
}

void GVim::on_vimclient_balloonEval(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, long offset, long len, std::string type)
{
    std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::balloonEval(offset="<<offset<<", len="<<len<<", type="<<type<<")" << std::endl;
}

void GVim::on_vimclient_balloonText(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, std::string text)
{
    std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::balloonText(text="<<text<<")" << std::endl;
}

void GVim::on_vimclient_buttonRelease(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, long button, long line, long col)
{
    std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::buttonRelease(button="<<button<<", line="<<line<<", col="<<col<<")" << std::endl;
}

void GVim::on_vimclient_disconnect(VimSocketInterfaceCommunicator& vim, long bufID, long seqno)
{
    std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::disconnect()" << std::endl;
    std::cout << "Vim is disconnecting!" << std::endl;
    // VimSocketInterfaceCommunicator takes care of its own shutdown properly
}

void GVim::on_vimclient_fileOpened(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, std::string pathname, bool open, bool modified)
{
    std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::fileOpened(pathname="<<pathname<<", open="<<open<<", modified="<<modified<<")" << std::endl;
}

void GVim::on_vimclient_geometry(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, long cols, long rows, long x, long y)
{
    std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::geometry(cols="<<cols<<", rows="<<rows<<", x="<<x<<", y="<<y<<")" << std::endl;
}

void GVim::on_vimclient_insert(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, long offset, std::string text)
{
    std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::insert(offset="<<offset<<", text="<<text<<")" << std::endl;
}

void GVim::on_vimclient_keyCommand(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, std::string keyName)
{
    std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::keyCommand(keyName="<<keyName<<")" << std::endl;
}

void GVim::on_vimclient_keyAtPos(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, std::string keyName, long lnum, long lcol)
{
    std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::keyAtPos(keyName="<<keyName<<", lnum="<<lnum<<", lcol="<<lcol<<")" << std::endl;
}

void GVim::on_vimclient_killed(VimSocketInterfaceCommunicator& vim, long bufID, long seqno)
{
    std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::killed()" << std::endl;
}

void GVim::on_vimclient_newDotAndMark(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, long offset1, long offset2)
{
    std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::newDotAndMark(offset1="<<offset1<<", offset2="<<offset2<<")" << std::endl;
}

void GVim::on_vimclient_remove(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, long offset, long length)
{
    std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::remove(offset="<<offset<<", length="<<length<<")" << std::endl;
}

void GVim::on_vimclient_save(VimSocketInterfaceCommunicator& vim, long bufID, long seqno)
{
    std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::save()" << std::endl;
}

void GVim::on_vimclient_startupDone(VimSocketInterfaceCommunicator& vim, long bufID, long seqno)
{
    std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::startupDone()" << std::endl;
    VimCommandCreate::send(vim, 1);
    VimCommandStartDocumentListen::send(vim, 1);
    VimCommandSetTitle::send(vim, 1, "Some title");
    VimCommandSetFullName::send(vim, 1, "~/some-full-name.txt");
}

void GVim::on_vimclient_unmodified(VimSocketInterfaceCommunicator& vim, long bufID, long seqno)
{
    std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::unmodified()" << std::endl;
}

void GVim::on_vimclient_version(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, std::string version)
{
    std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::version(version="<<version<<")" << std::endl;
}

void GVim::spawnGVim()
{
    if (!startServer())
    {
        std::cout << "Unable to start server, aborting!" << std::endl;
        // startServer should have been done prior to entering Gtk::Main loop
        // We can only wait to be exitted by hide()ing.
        signal_map().connect(sigc::mem_fun(*this, &GVim::on_map_quit));
        return;
    }

    Gdk::NativeWindow xid = mSocket.get_id();
    std::stringstream cmd;
    // Use final '&' to let gvim start when it can, we'll wait for it to join us
    cmd << "gvim --servername TwistIDE -nb:127.0.0.1:" << mServer.get_realport() << " --socketid " << xid << " &";
    std::cout << cmd.str() << std::endl;

    int rtn = system(cmd.str().c_str());
    if (rtn < 0 || !WIFEXITED(rtn) || WEXITSTATUS(rtn) != 0)
    {
        std::cerr << "Cannot run gvim!" << std::endl;
    }

    // Only accept one client
    mServer.accept();
    // Then quit server
    mServer.close();
}

void GVim::on_mSocket_realize()
{
    // Gtk::Socket created, has a NativeWindow id, we can spawn gvim inside it
    spawnGVim();
}

void GVim::on_map_quit()
{
    hide();
}

void GVim::on_mSocket_plug_added()
{
    std::cout << "mSocket_plug_added()" << std::endl;
    // Can't get focus on gvim! Grrr...
    //mSocket.child_focus(Gtk::DIR_TAB_FORWARD);
    //mSocket.get_plug_window()->focus(0);
    // GVim started!
    std::cout << "gvim --servername TwistIDE --remote-send 'iWelcome to TwistIDE!'" << std::endl;
    int rtn = system("gvim --servername TwistIDE --remote-send 'iWelcome to TwistIDE!'");
    if (rtn < 0 || !WIFEXITED(rtn) || WEXITSTATUS(rtn) != 0) {
        std::cerr << "Could not send commands to gvim!" << std::endl;
    }
}

bool GVim::on_mSocket_plug_removed()
{
    // GVim exitted, quitting
    hide(); // same as Gtk::Main::quit(); but for Gtk::Main::run(gvimWindow); calls
    return true;
}

