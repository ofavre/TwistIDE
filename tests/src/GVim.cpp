#include "GVim.hpp"



#include <iostream>
#include <sstream>



#include "VimEvent.hpp"
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

#include "VimFunction.hpp"
#include "vim_functions/getCursor.hpp"
#include "vim_functions/getLength.hpp"
#include "vim_functions/getAnno.hpp"
#include "vim_functions/getModified.hpp"
#include "vim_functions/getText.hpp"
#include "vim_functions/insert.hpp"
#include "vim_functions/remove.hpp"
#include "vim_functions/saveAndExit.hpp"



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

void GVim::on_vimclient_balloonEval(VimSocketInterfaceCommunicator& vim, long bufID, long offset, long len, std::string type)
{
    std::cout << "Vim(bufID="<<bufID<<")::balloonEval(offset="<<offset<<", len="<<len<<", type="<<type<<")" << std::endl;
}

void GVim::on_vimclient_balloonText(VimSocketInterfaceCommunicator& vim, long bufID, std::string text)
{
    std::cout << "Vim(bufID="<<bufID<<")::balloonText(text="<<text<<")" << std::endl;
}

void GVim::on_vimclient_buttonRelease(VimSocketInterfaceCommunicator& vim, long bufID, long button, long line, long col)
{
    std::cout << "Vim(bufID="<<bufID<<")::buttonRelease(button="<<button<<", line="<<line<<", col="<<col<<")" << std::endl;
}

void GVim::on_vimclient_disconnect(VimSocketInterfaceCommunicator& vim, long bufID)
{
    std::cout << "Vim(bufID="<<bufID<<")::disconnect()" << std::endl;
    std::cout << "Vim is disconnecting!" << std::endl;
    // VimSocketInterfaceCommunicator takes care of its own shutdown properly
}

void GVim::on_vimclient_fileOpened(VimSocketInterfaceCommunicator& vim, long bufID, std::string pathname, bool open, bool modified)
{
    std::cout << "Vim(bufID="<<bufID<<")::fileOpened(pathname="<<pathname<<", open="<<open<<", modified="<<modified<<")" << std::endl;
}

void GVim::on_vimclient_geometry(VimSocketInterfaceCommunicator& vim, long bufID, long cols, long rows, long x, long y)
{
    std::cout << "Vim(bufID="<<bufID<<")::geometry(cols="<<cols<<", rows="<<rows<<", x="<<x<<", y="<<y<<")" << std::endl;
}

void GVim::on_vimclient_insert(VimSocketInterfaceCommunicator& vim, long bufID, long offset, std::string text)
{
    std::cout << "Vim(bufID="<<bufID<<")::insert(offset="<<offset<<", text="<<text<<")" << std::endl;
    VimFunctionGetCursor::call(vim, bufID, sigc::mem_fun(*this, &GVim::on_vimclient_getCursor_reply));
    VimFunctionGetLength::call(vim, bufID, sigc::mem_fun(*this, &GVim::on_vimclient_getLength_reply));
}

void GVim::on_vimclient_keyCommand(VimSocketInterfaceCommunicator& vim, long bufID, std::string keyName)
{
    std::cout << "Vim(bufID="<<bufID<<")::keyCommand(keyName="<<keyName<<")" << std::endl;
}

void GVim::on_vimclient_keyAtPos(VimSocketInterfaceCommunicator& vim, long bufID, std::string keyName, long lnum, long lcol)
{
    std::cout << "Vim(bufID="<<bufID<<")::keyAtPos(keyName="<<keyName<<", lnum="<<lnum<<", lcol="<<lcol<<")" << std::endl;
}

void GVim::on_vimclient_killed(VimSocketInterfaceCommunicator& vim, long bufID)
{
    std::cout << "Vim(bufID="<<bufID<<")::killed()" << std::endl;
}

void GVim::on_vimclient_newDotAndMark(VimSocketInterfaceCommunicator& vim, long bufID, long offset1, long offset2)
{
    std::cout << "Vim(bufID="<<bufID<<")::newDotAndMark(offset1="<<offset1<<", offset2="<<offset2<<")" << std::endl;
}

void GVim::on_vimclient_remove(VimSocketInterfaceCommunicator& vim, long bufID, long offset, long length)
{
    std::cout << "Vim(bufID="<<bufID<<")::remove(offset="<<offset<<", length="<<length<<")" << std::endl;
}

void GVim::on_vimclient_save(VimSocketInterfaceCommunicator& vim, long bufID)
{
    std::cout << "Vim(bufID="<<bufID<<")::save()" << std::endl;
}

void GVim::on_vimclient_startupDone(VimSocketInterfaceCommunicator& vim, long bufID)
{
    std::cout << "Vim(bufID="<<bufID<<")::startupDone()" << std::endl;
    VimCommandCreate::send(vim, 1);
    VimCommandStartDocumentListen::send(vim, 1);
    VimCommandSetTitle::send(vim, 1, "Some title");
    VimCommandSetFullName::send(vim, 1, "~/some-full-name.txt");
}

void GVim::on_vimclient_unmodified(VimSocketInterfaceCommunicator& vim, long bufID)
{
    std::cout << "Vim(bufID="<<bufID<<")::unmodified()" << std::endl;
}

void GVim::on_vimclient_version(VimSocketInterfaceCommunicator& vim, long bufID, std::string version)
{
    std::cout << "Vim(bufID="<<bufID<<")::version(version="<<version<<")" << std::endl;
}

void GVim::on_vimclient_getCursor_reply(VimSocketInterfaceCommunicator& vim, long bufID, long cursorBufID, long lnum, long col, long off)
{
    std::cout << "Vim(bufID="<<bufID<<")::getCursor() -> cursorBufID="<<cursorBufID<<", lnum="<<lnum<<", col="<<col<<", off="<<off << std::endl;
}

void GVim::on_vimclient_getLength_reply(VimSocketInterfaceCommunicator& vim, long bufID, long len)
{
    std::cout << "Vim(bufID="<<bufID<<")::getLength() -> len="<<len << std::endl;
    // Disabled tests (impacts normal typing behaviour)
    /*
    if (len == 10)
    {
        std::cout << "Getting unexisting annotation line" << std::endl;
        VimFunctionGetAnno::call(vim, bufID, sigc::mem_fun(*this, &GVim::on_vimclient_getAnno_reply), 1);
    }
    else if (len == 20)
    {
        std::cout << "Getting modified buffers" << std::endl;
        VimFunctionGetModified::call(vim, 0, sigc::mem_fun(*this, &GVim::on_vimclient_getModified_reply));
        std::cout << "Getting is buffer modified" << std::endl;
        VimFunctionGetModified::call(vim, bufID, sigc::mem_fun(*this, &GVim::on_vimclient_getModified_reply));
    }
    else if (len == 30)
    {
        VimFunctionGetText::call(vim, bufID, sigc::mem_fun(*this, &GVim::on_vimclient_getText_reply));
    }
    else if (len == 5)
    {
        VimFunctionInsert::call(vim, bufID, sigc::mem_fun(*this, &GVim::on_vimclient_insert_reply), len-1, "ok");
    }
    else if (len == 40)
    {
        VimFunctionRemove::call(vim, bufID, sigc::mem_fun(*this, &GVim::on_vimclient_remove_reply), -10, 80);
    }
    else if (len == 50)
    {
        VimFunctionSaveAndExit::call(vim, bufID, sigc::mem_fun(*this, &GVim::on_vimclient_saveAndExit_reply));
    }
    */
}

void GVim::on_vimclient_getAnno_reply(VimSocketInterfaceCommunicator& vim, long bufID, long lnum)
{
    std::cout << "Vim(bufID="<<bufID<<")::getAnno() -> lnum="<<lnum << std::endl;
}

void GVim::on_vimclient_getModified_reply(VimSocketInterfaceCommunicator& vim, long bufID, long count)
{
    std::cout << "Vim(bufID="<<bufID<<")::getModified() -> count="<<count << std::endl;
}

void GVim::on_vimclient_getText_reply(VimSocketInterfaceCommunicator& vim, long bufID, std::string text)
{
    std::cout << "Vim(bufID="<<bufID<<")::getText() -> text=\""<<text<<"\"" << std::endl;
}

void GVim::on_vimclient_insert_reply(VimSocketInterfaceCommunicator& vim, long bufID, bool success)
{
    std::cout << "Vim(bufID="<<bufID<<")::insert() -> success="<<success << std::endl;
}

void GVim::on_vimclient_remove_reply(VimSocketInterfaceCommunicator& vim, long bufID, bool success)
{
    std::cout << "Vim(bufID="<<bufID<<")::remove() -> success="<<success << std::endl;
}

void GVim::on_vimclient_saveAndExit_reply(VimSocketInterfaceCommunicator& vim, long bufID, long modifiedBuffers)
{
    std::cout << "Vim(bufID="<<bufID<<")::remove() -> modifiedBuffers="<<modifiedBuffers << std::endl;
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

